#include "defs.h"

typedef struct packetGenerator
{
	long packetID;
	long tokens;
	float serviceTime;
	double arrivalTime;
	double IAtime;
	double Q1arrivalTime;
	double Q1LeaveTime;
	double Q2arrivalTime;
	double Q2LeaveTime;
	double SbeginTime;
	double SdepartTime;
}Packets;

Packets *pcktPtr;

typedef struct moveArguments
{
	My402List* Q1;
	My402List* Q2;
}Move;

typedef struct serverArguments
{
	My402List* Q2;
	int number;
}Serve;

//Command Line Arguments
extern float r;
extern float lambda;
extern float mu;
extern int P;
extern int B;
extern long num;

//variables for token generation
extern long tokenID;
extern long tokenCount;
extern long tokenDropCount;
extern double tokenArrivalTime;

//varaibales for Thread kill condition
extern int move_packets_done;
extern int serve_packets_done;
extern int packets_generated;

//statictics calculation
extern double avgPacketackIATime;
extern double avgPacketServiceTime;
extern double avgTimeSpentQ1;
extern double avgTimeSpentQ2;
extern double avgTimeSpentS1;
extern double avgTimeSpentS2;
extern double avgPacketTimeSystem;
extern double avgtimeSpentinSystemSD;

extern long packetDropCount;
extern long Q2checkCount;

//Thread first procedure
extern void *generateToken(void *arg);
extern void *generatePacket(void *arg);
extern void *server(void *arg);
extern void* signal1(void* arg);

extern pthread_mutex_t mutex;
extern pthread_cond_t cv;

//global variable for start time and End time
extern struct timeval time1,timeEnd;

//to read from file
extern FILE *file;
extern int fileFlag;
extern char* fileName;

//To handle sig_wait condition (CTRL+C)
extern sigset_t set;
extern int mkill;

//global variable to keep count of packets generated
extern long packetCount;
extern long packets_generated_count;

//to keep track of packets processed after servicing from servers
extern long packetProcessed;

