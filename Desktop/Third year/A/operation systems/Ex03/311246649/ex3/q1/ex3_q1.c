#include "ex3_q1.h"


int main(int argc, char* argv[])
{
    pthread_t producers[N_PROD];
    pthread_t consumers[N_CONS];
    long current_consumer_id, current_producer_id;

    for(int i = 0; i < N_PROD; i++)
    {
        current_producer_id = (long)i+1;
        pthread_create( &producers[i], NULL, producers_wait_func, (void*)current_producer_id);
    }
    print_message(ALL_PROD_CREATED);

    for(int i = 0; i < N_CONS; i++)
    {
        current_consumer_id = (long)i+1;
        pthread_create( &consumers[i], NULL, consumers_routine, (void*)current_consumer_id);
        pthread_mutex_lock(&created_mtx); 
        num_consumers_created++;
        if(num_consumers_created == N_CONS)
                 pthread_cond_broadcast(&consumers_created_cv);
        pthread_mutex_unlock(&created_mtx);
    }
    print_message(ALL_CONS_CREATED);

    for (int j=0; j < N_PROD; j++)
        pthread_join(producers[j] ,NULL);     
    print_message(PROD_TERMINATED);

    for (int j=0; j < N_CONS; j++)
        pthread_join(consumers[j] ,NULL);  
     print_message(CONS_TERMINATED);

    print_list();

    destroy_locks_and_conds();
    free_list();
    pthread_exit(NULL);
}

void free_list()
{
    list_node* curr_node = list_head;
    list_node* node_to_delete;
    while(curr_node != NULL)
    {
        node_to_delete = curr_node;
        curr_node = curr_node->next;
        free(node_to_delete->item);
        free(node_to_delete);
    }
}

void print_message(char* message_to_print)
{
    if(strcmp(message_to_print,ALL_PROD_CREATED)==0)
    {
        pthread_mutex_lock(&print_mtx);
        printf(ALL_PROD_CREATED);
        pthread_mutex_unlock(&print_mtx);
    }
    else if(strcmp(message_to_print,ALL_CONS_CREATED)==0)
    {
        pthread_mutex_lock(&print_mtx);
        printf(ALL_CONS_CREATED);
        pthread_mutex_unlock(&print_mtx);
    }
    else if(strcmp(message_to_print,PROD_TERMINATED)==0)
    {
        pthread_mutex_lock(&print_mtx);
        printf(PROD_TERMINATED);
        pthread_mutex_unlock(&print_mtx);
    }
    else if(strcmp(message_to_print,CONS_TERMINATED)==0)
    {
        printf(CONS_TERMINATED);
    }
}

void destroy_locks_and_conds()
{
     pthread_mutex_destroy(&cnt_mtx);
     pthread_mutex_destroy(&print_mtx);
    pthread_mutex_destroy(&list_mtx);
     pthread_mutex_destroy(&created_mtx);
     pthread_cond_destroy(&consumers_created_cv);
     pthread_cond_destroy(&items_in_list_threshold_cv);
}

void* producers_wait_func(void* id)
{
    pthread_mutex_lock(&created_mtx);
    while(num_consumers_created < N_CONS)
         pthread_cond_wait(&consumers_created_cv, &created_mtx);
    pthread_mutex_unlock(&created_mtx);

    producers_routine((long)id);
}

void producers_routine(long id)
{
    pthread_mutex_lock(&cnt_mtx);
    while(num_items_created < TOTAL_ITEMS)
    {  
        num_items_created++;
        pthread_mutex_unlock(&cnt_mtx);
        producer_actions(id);
        pthread_mutex_lock(&cnt_mtx);
        num_items_in_list++;

        if(num_items_in_list == ITEM_START_CNT)
            pthread_cond_broadcast(&items_in_list_threshold_cv);

        if(num_items_in_list > num_items_done) // there are item in NOT_DONE status!
            pthread_cond_broadcast(&items_not_done_cv);

    }
    pthread_mutex_unlock(&cnt_mtx);
    producer_finished(id);
}

void producer_finished(long id)
{
    pthread_mutex_lock(&print_mtx);
    write_producer_is_done((int)id);
    pthread_mutex_unlock(&print_mtx);
    pthread_exit(NULL);
}

void producer_actions(long id)
{
    item* item_to_add = initialize_item();

    pthread_mutex_lock(&list_mtx);
    add_to_list(item_to_add);
    pthread_mutex_lock(&print_mtx);
    write_adding_item((int)id,item_to_add);
    pthread_mutex_unlock(&print_mtx);
    pthread_mutex_unlock(&list_mtx);
}

item* initialize_item()
{
    int num1,num2;
    BOOL is_both_prime = FALSE;
    while(!is_both_prime)
    {
        pthread_mutex_lock(&rand_mtx);
        num1 = get_random_in_range();
        num2 = get_random_in_range();
        pthread_mutex_unlock(&rand_mtx);

        if(is_prime(num1) && is_prime(num2))
            is_both_prime = TRUE;
    }

    item* item_to_add =(item*)malloc(sizeof(item));
    item_to_add->prod = num1*num2;
    item_to_add->status = NOT_DONE;
    return item_to_add;
}

void* consumers_routine(void* id)
{
    long my_id = (long)id;
    pthread_mutex_lock(&cnt_mtx);
    while(num_items_in_list < ITEM_START_CNT)
         pthread_cond_wait(&items_in_list_threshold_cv, &cnt_mtx);
    pthread_mutex_unlock(&cnt_mtx);
    
    pthread_mutex_lock(&cnt_mtx);
    while(num_items_done < TOTAL_ITEMS)
    {
        while(num_items_done == num_items_in_list)
        {
           pthread_cond_wait(&items_not_done_cv, &cnt_mtx);
           if(num_items_done == TOTAL_ITEMS)
           {
              pthread_mutex_unlock(&cnt_mtx);
              consumer_finished(my_id);
           }
        }
        num_items_done++;
        pthread_mutex_unlock(&cnt_mtx);
        consumer_actions(my_id);
        pthread_mutex_lock(&cnt_mtx);
    }
    pthread_mutex_unlock(&cnt_mtx);
    consumer_finished(my_id);
}

void consumer_finished(long id)
{
    pthread_mutex_lock(&print_mtx);
    write_consumer_is_done((int)id);
    pthread_mutex_unlock(&print_mtx);
    pthread_exit(NULL);
}

void consumer_actions(long id)
{   
    pthread_mutex_lock(&list_mtx);
    item*  item_to_do = get_undone_from_list();
    assert(item_to_do != NULL);
    pthread_mutex_lock(&print_mtx);
    write_getting_item((int)id,item_to_do);
    pthread_mutex_unlock(&print_mtx);
    pthread_mutex_unlock(&list_mtx);
  
    item_to_do->status = DONE;
    int res = set_two_factors(item_to_do);
    assert(res == 0);
}

