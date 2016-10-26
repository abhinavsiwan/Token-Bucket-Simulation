#include "defs.h"
#include "global.h"

long packetDropCount;
int fileFlag;
char* fileName;

int readFile(FILE* fileName,Packets* packet)
{
	char buf[1026];

	if(fgets(buf,sizeof(buf),fileName) == NULL)
	{
		fprintf(stderr, "Malformed Input!!!, Insufficient packets\n");
     	exit(1);
	}
	char* startPtr = buf;
	char* tokenPtr = buf;
	
	while(*tokenPtr != '\t' && *tokenPtr != ' ')
		tokenPtr++;

	if(*tokenPtr == '\0')
	{
		fprintf(stderr, "Malformed Input!!!, Invalid input\n");
	 	exit(1);
	}
	*tokenPtr++ = '\0';

	//Store packet's IAtime
	packet->IAtime = atof(startPtr);
	//updating lambda to IA time for the packet to sleep
	lambda = atof(startPtr);
	
	//If IAT is greater than 10000, make it as 10000.
	if(lambda > 10000)
		lambda = 10000;
	
	//lambda = lambda/1000;
	
	while(*tokenPtr == '\t' || *tokenPtr == ' ')
		tokenPtr++;
	startPtr = tokenPtr;

	while(*tokenPtr != '\t' && *tokenPtr != ' ')
		tokenPtr++;

	if(*tokenPtr == '\0')
	{
		fprintf(stderr, "Invalid input\n");
	 	exit(1);
	}
	*tokenPtr++ = '\0';

	//Store the number of tokens required to process a packet
	packet->tokens = atoi(startPtr);

	while(*tokenPtr == '\t' || *tokenPtr == ' ')
		tokenPtr++;
	startPtr = tokenPtr;

	while(*tokenPtr != '\n' && *tokenPtr != '\0')
		tokenPtr++;
	*tokenPtr = '\0';

	//Store the service time for the packet
	packet->serviceTime = atof(startPtr);
	//If packet->service time is > 10000, make it as 10000
	if(packet->serviceTime > 10000)
		packet->serviceTime = 10000;
	return 0;
 }

void *generatePacket(void *arg)
{
	Move *list = (Move*)arg;
	Packets *prevPacket = (Packets*)malloc(sizeof(Packets));

	struct timeval time2,time3,time4,time5;

	for(packetCount =0;packetCount<num;packetCount++)
	{
		if(mkill == 1)
			break;
		//wakes up, creates a packet object
		Packets *packet =(Packets*)malloc(sizeof(Packets));
		packet->packetID = packetCount+1;

		if(fileFlag == TRUE)
		{
			readFile(file,packet);
			usleep(1000*lambda);

			//Packet arrival time & interarrival time
			gettimeofday(&time2,NULL);
			packet->arrivalTime = (time2.tv_sec - time1.tv_sec)*1000000;
	       	packet->arrivalTime = packet->arrivalTime + (time2.tv_usec - time1.tv_usec);
		}
		else
		{
			if(1/lambda > 10)
				usleep(10000000);
			else
				usleep(1000000/lambda);

			//Packet arrival time & interarrival time
			gettimeofday(&time2,NULL);
			packet->arrivalTime = (time2.tv_sec - time1.tv_sec)*1000000;
	       	packet->arrivalTime = packet->arrivalTime + (time2.tv_usec - time1.tv_usec);
			
			//read from commandLine or from global variables
			packet->tokens = P;
			packet->serviceTime = 1000.0/mu;

			if(packet->serviceTime > 10000)
				packet->serviceTime = 10000;
			
			if(prevPacket == NULL)
			{
				packet->IAtime = packet->arrivalTime;
				packet->IAtime = packet->IAtime/1000;
			}
			else
			{
				packet->IAtime = packet->arrivalTime - prevPacket->arrivalTime;
				packet->IAtime = packet->IAtime/1000;
			}
		}

		//handle <Cntrl+C> case
		if(mkill == 1)
			break;

		//calculating average IA time for statistics
		avgPacketackIATime = avgPacketackIATime + packet->IAtime;

		//Packet Dropping check
		if(packet->tokens > B)
		{
			fprintf(stdout,"%012.3fms: p%ld arrives, needs %ld tokens, inter-arrival time = %.3fms,packet dropped\n",\
				(packet->arrivalTime)/1000,packet->packetID,packet->tokens,packet->IAtime);
			packetDropCount++;
		}
		else
			fprintf(stdout,"%012.3fms: p%ld arrives, needs %ld tokens, inter-arrival time = %.3fms\n",\
				(packet->arrivalTime)/1000,packet->packetID,packet->tokens,packet->IAtime);

		packets_generated_count++;

		//locks mutex
		pthread_mutex_lock(&mutex);
		//when packet enters Q1 and it is the first packet in the queue,
		//moves the packet to Q2 if there are enough tokens
		if(My402ListEmpty(list->Q1) && packet->tokens <= tokenCount)
		{
			tokenCount -= packet->tokens;
			My402ListAppend(list->Q1, (void*)packet);

			gettimeofday(&time3,NULL);
			packet->Q1arrivalTime = (time3.tv_sec - time1.tv_sec)*1000000;
       		packet->Q1arrivalTime = packet->Q1arrivalTime + (time3.tv_usec - time1.tv_usec);

			fprintf(stdout, "%012.3fms: p%ld enters Q1\n",(packet->Q1arrivalTime)/1000,packet->packetID);

			My402ListElem *packetElem = My402ListFirst(list->Q1);
			My402ListUnlink(list->Q1, packetElem);

			gettimeofday(&time4,NULL);
			packet->Q1LeaveTime = (time4.tv_sec - time1.tv_sec)*1000000;
       		packet->Q1LeaveTime = packet->Q1LeaveTime + (time4.tv_usec - time1.tv_usec);

       		//For statistics
       		//avgTimeSpentQ1 = avgTimeSpentQ1 + ((packet->Q1LeaveTime)-(packet->Q1arrivalTime));

			fprintf(stdout, "%012.3fms: p%ld leaves Q1, time in Q1 = %.3fms, token bucket now has %ld token\n",\
				(packet->Q1LeaveTime)/1000,packet->packetID,((packet->Q1LeaveTime)-(packet->Q1arrivalTime))/1000,tokenCount);

			My402ListAppend(list->Q2, (void*)packet);
			gettimeofday(&time5,NULL);
			packet->Q2arrivalTime = (time5.tv_sec - time1.tv_sec)*1000000;
       		packet->Q2arrivalTime = packet->Q2arrivalTime + (time5.tv_usec - time1.tv_usec);

			fprintf(stdout, "%012.3fms: p%ld enters Q2\n",(packet->Q2arrivalTime)/1000,packet->packetID);		
		}
		else if(packet->tokens <= B)
		{
			//when a new packet arrives,if there were already packets queued in Q1, then the only action
			//is to enqueue the packet to Q1
			My402ListAppend(list->Q1, (void*)packet);

			gettimeofday(&time3,NULL);
			packet->Q1arrivalTime = (time3.tv_sec - time1.tv_sec)*1000000;
       		packet->Q1arrivalTime = packet->Q1arrivalTime + (time3.tv_usec - time1.tv_usec);

			fprintf(stdout, "%012.3fms: p%ld enters Q1\n",(packet->Q1arrivalTime)/1000,packet->packetID);
		}
		prevPacket = packet;
		//If packet is added to Q2 and Q2 was empty before, broadcast signal
		if(My402ListLength(list->Q2) > 0)
		{
			//Broadcast signal code
			pthread_cond_broadcast(&cv);
		}
		pthread_mutex_unlock(&mutex);
		
	}
	packets_generated = TRUE;
	return (void*)0;
}

