#include "defs.h"
#include "global.h"

My402List list_Q1;
My402List list_Q2;

void My402ListDeleteAllQ(My402List* head,int num)
{
	struct timeval timeCtrlC;
	double unlinkTime;

	if(My402ListEmpty(head) == TRUE)
	{
		return;
	}

	My402ListElem* cur = head->anchor.next;
	while(cur != &head->anchor)
	{
		Packets *packet =(Packets*)cur->obj;
		My402ListElem* temp = cur->next;

		gettimeofday(&timeCtrlC,NULL);
        unlinkTime = (timeCtrlC.tv_sec-time1.tv_sec)*1000000;
        unlinkTime = unlinkTime+(timeCtrlC.tv_usec-time1.tv_usec);

		fprintf(stdout, "%012.3fms: p%ld unliked from Q%d, <Cntrl+C> encountered\n",unlinkTime/1000,packet->packetID,num);
		free(cur);
		cur = temp;
	}
	head->anchor.next = NULL;
	head->anchor.prev = NULL;
	head->num_members = 0;
}

void* signal1(void* arg)
{
	//int sig = 0;
	for(;;)
	{
		sigwait(&set);
		mkill = 1;
		
		pthread_mutex_lock(&mutex);
		My402ListDeleteAllQ(&list_Q1,1);
		My402ListDeleteAllQ(&list_Q2,2);
		move_packets_done = TRUE;
		serve_packets_done = TRUE;
		packets_generated = TRUE;
		packetCount = num;
		pthread_mutex_unlock(&mutex);
		
	}

}


