#include <stdio.h>
#include <stdlib.h>
#include "my402list.h"

//Length of My402List
int  My402ListLength(My402List* head)
{
	return head->num_members;
}

//Check if My402List is empty
int  My402ListEmpty(My402List* head)
{
	if(head->num_members == 0)
		return TRUE;
	return FALSE;
}

//Append to the My402List
int  My402ListAppend(My402List* head, void* data)
{
	My402ListElem* temp = (My402ListElem*)malloc(sizeof(My402ListElem));
	if(temp == NULL)
	{
		fprintf(stderr, "failed to allocate memory.\n");
		return FALSE;
	}
	temp->obj = data;

	if(My402ListEmpty(head) == TRUE)
	{
		//Add to the front
		head->anchor.next = temp;
		head->anchor.prev = temp;
		temp->next = &head->anchor;
		temp->prev = &head->anchor;

		//incremnt the count of node numbers.
		head->num_members += 1;
	}
	else
	{
		//Add temp after last
		My402ListElem* last = head->anchor.prev;

		last->next = temp;
		temp->prev = last;
		head->anchor.prev = temp;
		temp->next = &head->anchor;

		//incremnt the count of node numbers.
		head->num_members += 1;
	}
	return TRUE;
}

//Prepend to the My402List
int  My402ListPrepend(My402List* head, void* data)
{
	My402ListElem* temp = (My402ListElem*)malloc(sizeof(My402ListElem));
	if(temp == NULL)
	{
		fprintf(stderr, "failed to allocate memory.\n");
		return FALSE;
	}
	temp->obj = data;

	if(My402ListEmpty(head) == TRUE)
	{
		//Add to the front
		head->anchor.next = temp;
		head->anchor.prev = temp;
		temp->next = &head->anchor;
		temp->prev = &head->anchor;

		//incremnt the count of node numbers.
		head->num_members += 1;
	}
	else
	{
		//Add temp before first
		My402ListElem* first = head->anchor.next;

		head->anchor.next = temp;
		temp->prev = &head->anchor;
		temp->next = first;
		first->prev = temp;

		//incremnt the count of node numbers.
		head->num_members += 1;
	}
	return TRUE;
}

//Unlink the given My402ListElem node
void My402ListUnlink(My402List* head, My402ListElem* node)
{
	if(My402ListEmpty(head) == TRUE)
	{
		printf("List is Empty.\n");
		return;
	}
	node->prev->next = node->next;
	node->next->prev = node->prev;
	free(node);
	head->num_members = head->num_members -1;

	//Check if List is empty after deleting the only one element
	if(My402ListEmpty(head) == TRUE)
	{
		head->anchor.next = NULL;
		head->anchor.prev = NULL;
	}
}

//Unlink and delete all nodes from the list
void My402ListUnlinkAll(My402List* head)
{
	if(My402ListEmpty(head) == TRUE)
	{
		printf("List is Empty.\n");
		return;
	}

	My402ListElem* cur = head->anchor.next;

	while(cur != &head->anchor)
	{
		My402ListElem* temp = cur->next;
		free(cur);
		//These two lines can be removed
		//head->anchor.next = temp;
		//temp->prev = &head->anchor;
		//head->num_members = head->num_members -1;

		cur = temp;
	}
	head->anchor.next = NULL;
	head->anchor.prev = NULL;
	head->num_members = 0;
}

//Insert obj between elem and elem->next. If elem is NULL, then this is the same as Append(). 
//This function returns TRUE if the operation is performed successfully and returns FALSE otherwise.
int  My402ListInsertAfter(My402List* head, void* data, My402ListElem* node)
{
	if(My402ListEmpty(head) == TRUE)
		return FALSE;

	if(node == NULL)
		My402ListAppend(head, data);
	else
	{
		My402ListElem* temp = (My402ListElem*)malloc(sizeof(My402ListElem));
		temp->obj = data;

		My402ListElem* cur = node->next;
		node->next = temp;
		temp->prev = node;
		temp->next = cur;
		cur->prev = temp;

		//incremnt the count of node numbers.
		head->num_members += 1;
	}
	return TRUE;
}

//Insert obj between elem and elem->prev. If elem is NULL, then this is the same as Prepend().
//This function returns TRUE if the operation is performed successfully and returns FALSE otherwise.
int  My402ListInsertBefore(My402List* head, void* data, My402ListElem* node)
{
	if(My402ListEmpty(head) == TRUE)
		return FALSE;

	if(node == NULL)
		My402ListPrepend(head, data);
	else
	{
		My402ListElem* temp = (My402ListElem*)malloc(sizeof(My402ListElem));
		temp->obj = data;

		My402ListElem* cur = node->prev;

		cur->next = temp;
		node->prev = temp;
		temp->next = node;
		temp->prev = cur;


		//incremnt the count of node numbers.
		head->num_members += 1;
	}
	return TRUE;
}

//returns the first list element or null
My402ListElem *My402ListFirst(My402List* head)
{
	if(My402ListEmpty(head) == TRUE)
		return NULL;

	return head->anchor.next;
}

//returns the last list element or null
My402ListElem *My402ListLast(My402List* head)
{
	if(My402ListEmpty(head) == TRUE)
		return NULL;

	return head->anchor.prev;
}

//Returns elem->next or NULL if elem is the last item on the list.
My402ListElem *My402ListNext(My402List* head, My402ListElem* node)
{
	if(My402ListEmpty(head) == TRUE)
		return NULL;

	//to check if the passed node value is NULL
	if(node == NULL)
		return NULL;

	//to check if node is the last node
	if(node->next == &head->anchor)
		return NULL;

	return node->next;
}

//Returns elem->prev or NULL if elem is the first item on the list.
My402ListElem *My402ListPrev(My402List* head, My402ListElem* node)
{
	if(My402ListEmpty(head) == TRUE)
		return NULL;

	//to check if the passed node value is NULL
	if(node == NULL)
		return NULL;
	
	//to check if node is the first node
	if(node->prev == &head->anchor)
		return NULL;

	return node->prev;

}

//Returns the list element elem such that elem->obj == obj. Returns NULL if no such element can be found.
My402ListElem *My402ListFind(My402List* head, void* data)
{
	if(My402ListEmpty(head) == TRUE)
		return NULL;

	My402ListElem* cur = head->anchor.next;

	while(cur != &head->anchor)
	{
		if(cur->obj == data)
			break;
		cur = cur->next;
	}

	if(cur == &head->anchor)
	{
		printf("Element not found\n");
		return NULL;
	}
	return cur;
}

//Initialize the list into an empty list
int My402ListInit(My402List* head)
{
	if(My402ListEmpty(head) == TRUE)
	{
		head = (My402List*)malloc(sizeof(My402List));
		if(head == NULL)
		{
			fprintf(stderr, "failed to allocate memory.\n");
			return FALSE;
		}
	}
	head->anchor.next = NULL;
	head->anchor.prev = NULL;
	head->num_members = 0;
	return TRUE;
}
