#include "defs.h"
#include "global.h"

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cv = PTHREAD_COND_INITIALIZER;

//Emulation Parameters
float r = 1.5;  		//token rate
float lambda = 1;   //packet generation rate
float mu = 0.35;  	//service time
int P = 3;   		//No of tokens required
int B = 10;   		//Token Bucketsize
long num = 20; 		 //Number of packets
long emulationTime;  //Emulation start time

//variables for thread termination check
int move_packets_done = FALSE;
int serve_packets_done = FALSE;
int packets_generated = FALSE;

FILE *file;

//time object for emulation start and emulation end time
struct timeval time1,timeEnd;

//statictics calculation
double avgPacketackIATime;
double avgPacketServiceTime;
double avgTimeSpentQ1;
double avgTimeSpentQ2;
double avgTimeSpentS1;
double avgTimeSpentS2;
double avgPacketTimeSystem;
double avgtimeSpentinSystemSD;

int fileFlag = FALSE;
char* fileName;

sigset_t set;
int mkill = 0;

My402List list_Q1;
My402List list_Q2;

long packetCount = 0;
long packets_generated_count = 0;
//to keep track of the number of packets processed in the system
long packetProcessed = 0;

void validateDigit(char *temp)
{
	int i;
	for(i=0;i<strlen(temp);i++)
	{
		if(isdigit(temp[i]) == 0)
		{
			if(!(temp[i] == '.'))
			{
				fprintf(stderr, "malformed command\n");
				fprintf(stderr, "Usage:  warmup2 [-lambda lambda] [-mu mu] [-r r] [-B B] [-P P] [-n num] [-t tsfile]\n");
				exit(1);
			}
		}
	}
}

void readCommandLineArguments(int argc, char* argv[])
{
	if(argc%2 == 0)
	{
		fprintf(stderr, "malformed command\n");
		fprintf(stderr, "Usage:  warmup2 [-lambda lambda] [-mu mu] [-r r] [-B B] [-P P] [-n num] [-t tsfile]\n");
		exit(1);
	}

	int i=0;
	
	for(i=1;i<argc-1;i=i+2)
	{
		if(!strcmp(argv[i],"-lambda"))
		{
			validateDigit(argv[i+1]);
			lambda = atof(argv[i+1]);
		}
		else if(!strcmp(argv[i],"-mu"))
		{
			validateDigit(argv[i+1]);
			mu = atof(argv[i+1]);
		}
		else if(!strcmp(argv[i],"-r"))
		{
			validateDigit(argv[i+1]);
			r = atof(argv[i+1]);
		}
		else if(!strcmp(argv[i],"-B"))
		{
			validateDigit(argv[i+1]);
			B = atoi(argv[i+1]);
		}
		else if(!strcmp(argv[i],"-P"))
		{
			validateDigit(argv[i+1]);
			P = atoi(argv[i+1]);
		}
		else if(!strcmp(argv[i],"-n"))
		{
			validateDigit(argv[i+1]);
			num = atol(argv[i+1]);
			//If packet count is greater than long value range
			if(num > 2147483647 || num <= 0) 
			{
				fprintf(stderr, "Invalid packet count, not in range\n");
				exit(1);
			}
		}
		else if(!strcmp(argv[i],"-t"))
		{
			struct stat fileStat;
			fileName = argv[i+1];
			fileFlag = TRUE;
			file = fopen(fileName,"r");
			if(file == NULL)
			{
				fprintf(stderr, "input file %s does not exist\n", fileName);
				exit(1);
			}

			stat(fileName, &fileStat);
			if(S_ISDIR(fileStat.st_mode))
			{
				fprintf(stderr,"input file %s is a directry\n", fileName);
				exit(1);
			}
			
			char buf[1026];

			fgets(buf,sizeof(buf),file);
			char *startPtr = buf;
			char *tokenPtr = buf;

			while(*tokenPtr != '\n' && *tokenPtr != '\0')
				tokenPtr++;
			*tokenPtr = '\0';

			validateDigit(startPtr);
			num = atol(startPtr);
			//If packet count is greater than long value range
			if(num > 2147483647 || num <= 0) 
			{
				fprintf(stderr, "Invalid packet count, not in range\n");
				exit(1);
			}
		}
		else
		{
			fprintf(stderr, "malformed command\n");
			fprintf(stderr, "Usage:  warmup2 [-lambda lambda] [-mu mu] [-r r] [-B B] [-P P] [-n num] [-t tsfile]\n");
			exit(1);
		}
	}
}

void statictics()
{
	double totalEmulationTime;
	double tokenDrop;
	double packetDrop;

	double var, ex;

	//total emulation time for calculation of average number of packets
	totalEmulationTime = (timeEnd.tv_sec - time1.tv_sec)*1000000;
   	totalEmulationTime = totalEmulationTime + (timeEnd.tv_usec - time1.tv_usec);

   	//token drop probability
   	tokenDrop = ((float)tokenDropCount/(float)(tokenID));

   	//packet drop probability
   	packetDrop = ((float)packetDropCount/(float)(packetCount));
   	//packetDrop = ((float)packetDropCount/(float)(packetDropCount + packetProcessed));

   	//variance and standard deviation calculation
   	var = avgtimeSpentinSystemSD/packetProcessed;
   	ex = avgPacketTimeSystem/1000;

   	var = var - ((ex/packetProcessed)*(ex/packetProcessed));
   	var = sqrt(var);

   	// fprintf(stdout, "Number : %ld, Token Id : %ld, Packet Count : %ld\n",num, tokenID, packets_generated_count);
   	// fprintf(stdout, "Token Drop Count : %ld, Packet Drop Count : %ld\n",tokenDropCount, packetDropCount);

	fprintf(stdout, "Statistics:\n\n");
   	fprintf(stdout, "       average packet inter-arrival time = %f\n",avgPacketackIATime/(1000*packets_generated_count));
   	if(num == packetDropCount)
   		fprintf(stdout,"    average packet service time = \"N/A\" since all packets are dropped,no packets got serviced\n");
	else
		fprintf(stdout, "       average packet service time = %f\n\n",(avgTimeSpentS1 + avgTimeSpentS2)/(1000000*(packetProcessed)));


	fprintf(stdout, "       average number of packets in Q1 = %f\n",avgTimeSpentQ1/totalEmulationTime);
	fprintf(stdout, "       average number of packets in Q2 = %f\n",avgTimeSpentQ2/totalEmulationTime);
	fprintf(stdout, "       average number of packets at S1 = %f\n", avgTimeSpentS1/totalEmulationTime) ;
	fprintf(stdout, "       average number of packets at S2 = %f\n\n", avgTimeSpentS2/totalEmulationTime);

	if(num == packetDropCount)
		fprintf(stdout,"    average time a packet spent in system = \"N/A\" all packets are dropped\n");
	else
		fprintf(stdout, "       average time a packet spent in system = %f\n", avgPacketTimeSystem/(1000000*(packetProcessed)));
	
	if(num == packetDropCount)
		fprintf(stdout,"    standard deviation for time spent in system = \"N/A\" all packets are dropped\n");
	else
		fprintf(stdout, "       standard deviation for time spent in system = %f\n\n",var/1000);

	fprintf(stdout, "       token drop probability = %f\n", tokenDrop);
	fprintf(stdout, "       packet drop probability = %f\n", packetDrop);
}

int main(int argc, char* argv[])
{
	//read command line arguments
	readCommandLineArguments(argc,argv);

	memset(&list_Q1, 0, sizeof(My402List));
	memset(&list_Q2, 0, sizeof(My402List));

	My402ListInit(&list_Q1);
	My402ListInit(&list_Q2);

	//Print emulation parameters
	fprintf(stdout, "Emulation Parameters:\n");
	if(fileFlag == TRUE)
	{
		//check the num value when reading from file
		fprintf(stdout, "       number to arrive = %ld\n",num);
		fprintf(stdout, "       r = %f\n",r);
		fprintf(stdout, "       B = %d\n",B);
		fprintf(stdout, "       tsfile = %s\n", fileName);
	}
	else
	{
		fprintf(stdout, "       number to arrive = %ld\n",num);
		fprintf(stdout, "       lambda = %f\n",lambda);
		fprintf(stdout, "       mu = %f\n",mu);
		fprintf(stdout, "       r = %f\n",r);
		fprintf(stdout, "       B = %d\n",B);
		fprintf(stdout, "       P = %d\n",P);
	}
	
	
	//Initialize emulation time
	double emulEnd;
	fprintf(stdout,"00000000.000ms: emulation begins\n");
	gettimeofday(&time1,NULL);

	//Argument list for Mover thread
	Move *arg_list = (Move*)malloc(sizeof(Move));
	arg_list->Q1 = &list_Q1;
	arg_list->Q2 = &list_Q2;

	//Argument List for Server Packet
	Serve *argS1 =(Serve*)malloc(sizeof(Serve));
	argS1->Q2 = &list_Q2;
	argS1->number = 1; 

	Serve *argS2 =(Serve*)malloc(sizeof(Serve));
	argS2->Q2 = &list_Q2;
	argS2->number = 2;

	//create four threads
	pthread_t threadID[4],signalHandler;
	//for handling ctrl+C
	sigemptyset(&set);
	sigaddset(&set, SIGINT);
	sigprocmask(SIG_BLOCK, &set, 0);

	pthread_create(&signalHandler,0,signal1,(void*)0);

	pthread_create(&threadID[0],0,generateToken,(void*)arg_list);
	pthread_create(&threadID[1],0,generatePacket,(void*)arg_list);
	pthread_create(&threadID[2],0,server,(void*)argS1);
	pthread_create(&threadID[3],0,server,(void*)argS2);

	//parent thread calling pthread_join, waiting for child threads to die
	int i=0;
	for(i=0;i<4;i++)
	{
		pthread_join(threadID[i], 0);
	}

	//Emulation end time
	gettimeofday(&timeEnd,NULL);
   	emulEnd=(timeEnd.tv_sec-time1.tv_sec)*1000000;
   	emulEnd=emulEnd+(timeEnd.tv_usec-time1.tv_usec);
   	fprintf(stdout,"%012.3fms: emulation ends\n\n",(emulEnd)/1000);

   	statictics();
   	if(fileFlag == TRUE)
   	{
      fclose(file); 
   	}

	exit(0);
}
