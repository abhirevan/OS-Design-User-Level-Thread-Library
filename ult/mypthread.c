#include "ucontext.h"
#include "mypthread.h"
#include "stdio.h"
#include "time.h"
#include "stdlib.h"

// Implementation for circular linked list
int numOfNodes = 0;
//Return numOfThreads
int getcount() {
	return numOfNodes;
}

//Return Head of Linked List
mypthread_t* returnHead() {
	return (head->n);
}

//Return Tail of Linked List
mypthread_t* returnTail() {
	return (tail->n);
}

//Create node creates node for LinkedList
void create_node() {
	mypthread_t data;
	temp = (struct node *) malloc(1 * sizeof(struct node));
	temp->prev = NULL;
	temp->next = NULL;
	numOfNodes++;
}

//Insert node at the end of LinkedList
void enqueue(mypthread_t* data) {
	if (head == NULL) { //for the first node
		create_node();
		head = temp;
		tail = temp;
	} else {
		create_node();
		tail->next = temp;
		temp->prev = tail;
		tail = temp;
		tail->next = head;
		head->prev = tail;
	}
	temp->n = data;
}

mypthread_t* searchThread(int threadId) {
	temp = head;
	while (temp != NULL) {
		if (temp->n->th_id == threadId)
			return (temp->n);
		else {
			temp = temp->next;
		}
	}
	printf("Error! %d Not found!", threadId);
	exit(0);
}

mypthread_t* searchNextActiveThread(int threadId) {
	//First search from head to find threadId
	temp = head;

	while (temp != NULL) {
		if (temp->n->th_id == threadId) {
			//seachNode=temp;
			break;
		} else {
			temp = temp->next;
		}
	}
	if (temp->next != NULL) {
		temp = temp->next;
		while (temp != NULL) {
			if (temp->n->state == PS_ACTIVE) {
				return temp->n;
			} else {
				temp = temp->next;
			}
		}

	} else {
		exit(0);
	}

	printf("No active thread found after ThreadId%d\n", threadId);
	exit(0);
}

int curr_thread_id = 1; // Start the execution from main

int numOfThreads = 1;

int mypthread_create(mypthread_t *thread, const mypthread_attr_t *attr,
		void *(*start_routine)(void *), void *arg) {

	if (getcount() == 0) {
		//Create main thread
		mypthread_t* main_thread = (mypthread_t *) malloc(sizeof(mypthread_t));
		main_thread->th_id = numOfThreads++;
		ucontext_t* context = (ucontext_t*) malloc(sizeof(ucontext_t));
		main_thread->ctx = context;
		main_thread->ctx->uc_stack.ss_sp = (char*) malloc(sizeof(char) * 4096);
		main_thread->ctx->uc_stack.ss_size = 4096;
		main_thread->state = PS_ACTIVE;
		enqueue(main_thread);
	}

	ucontext_t* context = (ucontext_t*) malloc(sizeof(ucontext_t));
	thread->ctx = context;
	getcontext(thread->ctx);
	(*thread).ctx->uc_stack.ss_sp = (char*) malloc(sizeof(char) * 4096);
	(*thread).ctx->uc_stack.ss_size = 4096;
	(*thread).state = PS_ACTIVE;
	thread->th_id = numOfThreads++;
	makecontext(thread->ctx, (void (*)()) start_routine, 1, arg);
	enqueue(thread);
	return 0;
}

void mypthread_exit(void *retval) {

	mypthread_t* current_thread = searchThread(curr_thread_id);
	//Change the state as dead
	current_thread->state = PS_DEAD;
	free(current_thread->ctx);
	if (current_thread->joinfrom_th != 0) //Means that this was joined from somewhere
			{
		mypthread_t* join_thread = searchThread(current_thread->joinfrom_th);
		join_thread->state = PS_ACTIVE;
		current_thread->joinfrom_th;
	}
	//find the next active thread from round robin
	mypthread_t* next_thread = searchNextActiveThread(current_thread->th_id);
	if (curr_thread_id == next_thread->th_id)//Should not be the same thread
		return;
	curr_thread_id = next_thread->th_id;
	setcontext(next_thread->ctx);

}

int mypthread_yield(void) {

	mypthread_t* current_thread = searchThread(curr_thread_id);
	//find the next active thread from round robin
	mypthread_t* next_thread = searchNextActiveThread(current_thread->th_id);

	if (curr_thread_id == next_thread->th_id)
		return 0;

	curr_thread_id = next_thread->th_id;
	swapcontext(current_thread->ctx, next_thread->ctx);
	return 0;
}

int mypthread_join(mypthread_t thread, void **retval) {
	int target_th_id = thread.th_id;
	mypthread_t* current_thread = searchThread(curr_thread_id);
	mypthread_t* target_thread = searchThread(thread.th_id);
	if (target_thread->state != PS_ACTIVE) {
		return 0;
	} else {
		current_thread->state = PS_BLOCKED;
		target_thread->joinfrom_th = curr_thread_id;
		curr_thread_id = target_th_id;
		swapcontext(current_thread->ctx, target_thread->ctx);
	}
	return 0;
}
