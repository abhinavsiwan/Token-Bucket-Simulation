#include "defs.h"
#include "global.h"
 
 //Token Generator variables
long tokenID = 0;
long tokenCount = 0;
long tokenDropCount = 0;
double tokenArrivalTime = 0;

void *generateToken(void *arg)
{
	Move *list = (Move*)arg;
	struct timeval time2, time3,time4;

	for(;;)
	{
		//Sleeps for an interval, trying to match the inter-arrival time for tokens
		if(1/r > 10)
			usleep(10000000);
		else
			usleep(1000000/r);

		//wakes up, locks the mutex and increment the token count
		pthread_mutex_lock(&mutex);
		
		if(My402ListEmpty(list->Q1) && packets_generated == TRUE)
		{
			//This thread terminates when Q1 is empty and all the packets are generated
			move_packets_done = TRUE;
			pthread_mutex_unlock(&mutex);

			//Broadcast signal code
			pthread_cond_broadcast(&cv);
			return ((void*)0);
		}

		//Token arrival time
		gettimeofday(&time2,NULL);
		tokenID++;
		tokenArrivalTime=(time2.tv_sec-time1.tv_sec)*1000000;
        tokenArrivalTime=tokenArrivalTime+(time2.tv_usec-time1.tv_usec);
        
		if(tokenCount == B)
		{
			tokenDropCount++;
			fprintf(stdout, "%012.3fms: token t%ld arrives, dropped\n",(tokenArrivalTime)/1000,tokenID);
		}
		else
		{	
			tokenCount++;
			fprintf(stdout,"%012.3fms: token t%ld arrives, token bucket now has %ld tokens\n",\
				(tokenArrivalTime)/1000,tokenID,tokenCount);
		}

		//check if it can move first packet from Q1 to Q2
		My402ListElem *packetElem = My402ListFirst(list->Q1);
		if(packetElem != NULL)
		{
			Packets *packet = (Packets*)packetElem->obj;
			if(packet->tokens <= tokenCount)
			{
				tokenCount -= packet->tokens;
				My402ListUnlink(list->Q1, packetElem);

				//to store the Q1LeaveTime
				gettimeofday(&time3,NULL);
				packet->Q1LeaveTime = (time3.tv_sec - time1.tv_sec)*1000000;
           		packet->Q1LeaveTime = packet->Q1LeaveTime + (time3.tv_usec - time1.tv_usec);

               	//for statistics
               	//avgTimeSpentQ1=avgTimeSpentQ1+((packet->Q1LeaveTime)-(packet->Q1arrivalTime));

				fprintf(stdout, "%012.3fms: p%ld leaves Q1, time in Q1 = %.3fms token bucket now has %ld token\n",\
					(packet->Q1LeaveTime)/1000,packet->packetID,((packet->Q1LeaveTime)-(packet->Q1arrivalTime))/1000,tokenCount);

				My402ListAppend(list->Q2, (void*)packet);
				//to store the Q2arrivalTime
				gettimeofday(&time4,NULL);
				packet->Q2arrivalTime = (time4.tv_sec - time1.tv_sec)*1000000;
           		packet->Q2arrivalTime = packet->Q2arrivalTime + (time4.tv_usec - time1.tv_usec);

				fprintf(stdout,"%012.3fms: p%ld enters Q2\n",(packet->Q2arrivalTime)/1000,packet->packetID);
			}
		}
		//If packet is added to Q2 and Q2 was empty before, broadcast signal
		if(My402ListLength(list->Q2) > 0)
		{
			//Broadcast signal code
			pthread_cond_broadcast(&cv);
		}
		pthread_mutex_unlock(&mutex);
	}
}


