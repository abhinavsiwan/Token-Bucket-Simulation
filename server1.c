#include "defs.h"
#include "global.h"

void *server(void *arg)
{
	Serve *list =(Serve*)arg;
	My402ListElem *cur;
	struct timeval time2,time3;
	double servicetime, timeInSystem;
	for(;;)
	{
		pthread_mutex_lock(&mutex);
		if(My402ListEmpty(list->Q2) && move_packets_done == TRUE)
		{
			//This thread terminates when Q2 is empty and all packets are moved from Q1 to Q2
			pthread_mutex_unlock(&mutex);
			return (void*)0;
		}

		//Server thread will go to wait condition when Q2 is empty and packets are getting moved from Q1 to Q2
		while (My402ListEmpty(list->Q2) && move_packets_done == FALSE)
		{
	        pthread_cond_wait(&cv, &mutex);
		}

		cur = My402ListFirst(list->Q2);
		if(cur != NULL)
		{
			Packets *packet = (Packets*)cur->obj;
			My402ListUnlink(list->Q2,cur);

			gettimeofday(&time2,NULL);
			packet->Q2LeaveTime = (time2.tv_sec - time1.tv_sec)*1000000;
			packet->Q2LeaveTime = packet->Q2LeaveTime + (time2.tv_usec - time1.tv_usec);
			
			//avgTimeSpentQ2 = avgTimeSpentQ2 + ((packet->Q2LeaveTime)-(packet->Q2arrivalTime));

			fprintf(stdout, "%012.3fms: p%ld leaves Q2, time in Q2 = %.3fms\n",\
				(packet->Q2LeaveTime)/1000,packet->packetID, ((packet->Q2LeaveTime)-(packet->Q2arrivalTime))/1000);

			pthread_mutex_unlock(&mutex);
			//Get the time to begin service at Server
			gettimeofday(&time2,NULL);
			packet->SbeginTime = (time2.tv_sec - time1.tv_sec)*1000000;
			packet->SbeginTime = packet->SbeginTime + (time2.tv_usec - time1.tv_usec);

			fprintf(stdout, "%012.3fms: p%ld begins service at S%d, requesting %.0fms of service\n",\
			 	packet->SbeginTime/1000,packet->packetID,list->number,packet->serviceTime);
			
			if(fileFlag == TRUE)
			{
				usleep(1000*(packet->serviceTime));
			}
			else
			{
				if(1/mu > 10)
					usleep(10000000);
				else
					usleep(1000000/mu);
			}

			//For statistics
			avgTimeSpentQ1 = avgTimeSpentQ1 + ((packet->Q1LeaveTime)-(packet->Q1arrivalTime));
			avgTimeSpentQ2 = avgTimeSpentQ2 + ((packet->Q2LeaveTime)-(packet->Q2arrivalTime));

			//Get the time to depart from Server
			gettimeofday(&time3,NULL);
			packet->SdepartTime = (time3.tv_sec - time1.tv_sec)*1000000;
			packet->SdepartTime = packet->SdepartTime + (time3.tv_usec - time1.tv_usec);

			//The time spent in system for a packet is the difference between the time the packet departed from the
			//server and the time that packet arrived
			servicetime = packet->SdepartTime - packet->SbeginTime;
			timeInSystem = (packet->SdepartTime) - (packet->arrivalTime);

			//for statistics :average number of packets at S1 or S2
			if(list->number == 1)
			{
				avgTimeSpentS1 = avgTimeSpentS1 + servicetime;
			}
			else
			{
				avgTimeSpentS2 = avgTimeSpentS2 + servicetime;
			}
			//for statistics : avg time a packet spent in the system
			avgPacketTimeSystem = avgPacketTimeSystem + timeInSystem;

			//variance calculation for time spent in system
			avgtimeSpentinSystemSD = avgtimeSpentinSystemSD + ((timeInSystem/1000)*(timeInSystem/1000));
			
			fprintf(stdout, "%012.3fms: p%ld departs from S%d, service Time = %.3fms, time in system = %.3fms\n",\
			 	packet->SdepartTime/1000,packet->packetID,list->number,servicetime/1000,timeInSystem/1000);

			//to keep track of the number of packets processed in the system
			packetProcessed++;

		}
		else
			pthread_mutex_unlock(&mutex);
	}
}

