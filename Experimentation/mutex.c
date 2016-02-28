//******************* Created by: ARVP 2016 *******************//
//******************* last Updated: 2/27/16 ******************* //

#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<string.h>
#include<unistd.h>

pthread_t tid[5];
int counter;
pthread_mutex_t lock;

void* begin(void *arg)
{
    pthread_mutex_lock(&lock); 								// create mutex lock mechanism -> Lock mutex due to thread beginning
    unsigned long i=0;
    counter+=1;
    printf("\n Job %d started \n", counter);						// indicate process is starting
    for(i=0; i<(0xFFFFFFFF);i++);							// wait for some time - i.e; modify this to standard operations
    printf("\n Job %d finished \n", counter); 						// indicate process is ending
    printf("\n ------------------- \n");
    pthread_mutex_unlock(&lock);							// create mutex unlock mechanism -> Unlock mutex due to thread finishing
    return NULL;
}

int main(void)
{
    int i=0;
    int err;

    if (pthread_mutex_init(&lock, NULL) != 0) 						 // if loop to detect if there is an error in the threading
    {
        printf("\n Mutex Error \n");             
        return 1;
    }

    while(i<5)
    {
        err = pthread_create(&(tid[i]), NULL, &begin, NULL);				// temporal loop - standard
        if (err!=0)
            printf("\n Unable to create thread :[%s]", strerror(err));
        i++;
    }

    pthread_join(tid[0], NULL);								// different threads
    pthread_join(tid[1], NULL);
    pthread_join(tid[2], NULL);
    pthread_join(tid[3], NULL);
    pthread_join(tid[4], NULL);

    return 0;
}
