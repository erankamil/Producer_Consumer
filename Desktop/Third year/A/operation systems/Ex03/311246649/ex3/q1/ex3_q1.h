#include "ex3_q1_given.h"
#include <stdio.h>
#include <unistd.h>
#include<stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <math.h>
#include <string.h>
#include <sys/wait.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/socket.h>
#include <netdb.h>
#include <assert.h>

typedef int BOOL;
#define TRUE 1
#define FALSE 0

int  num_items_created = 0;
int num_items_in_list = 0; 
int num_items_done = 0;
int num_consumers_created = 0;

pthread_mutex_t cnt_mtx = PTHREAD_MUTEX_INITIALIZER; 
pthread_mutex_t created_mtx = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t print_mtx = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t rand_mtx = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t list_mtx = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t consumers_created_cv = PTHREAD_COND_INITIALIZER;
pthread_cond_t items_in_list_threshold_cv = PTHREAD_COND_INITIALIZER;
pthread_cond_t items_not_done_cv = PTHREAD_COND_INITIALIZER;


struct list_node* list_head = NULL;
struct list_node* list_tail = NULL;


void* producers_wait_func(void* id);
void producers_routine(long id);
void producer_finished(long id);
void producer_actions(long id);
item* initialize_item();
void* consumers_routine(void* id);
void consumer_finished(long id);
void consumer_actions(long id);
void destroy_locks_and_conds();
void print_message(char* message_to_print);
void free_list();
