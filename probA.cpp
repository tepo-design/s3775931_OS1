#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <iostream>
using std::cout;

// PROBLEM A

#define PRODUCER_AMOUNT 5
#define CONSUMER_AMOUNT 5
#define BUCKET_SIZE 10

pthread_mutex_t mutex;
pthread_cond_t spotsFree = PTHREAD_COND_INITIALIZER; 
pthread_cond_t foodAvailable = PTHREAD_COND_INITIALIZER;

int bucket[BUCKET_SIZE];
int count = 0;
int maximumIndex = BUCKET_SIZE - 1;
int minimumIndex = 0;
bool runningA = true;


void* producer(void* args) 
{
    while (runningA) 
    {
      // initiate lock so only one thread has access to the bucket  
      pthread_mutex_lock(&mutex);
    
      // while the number of buckets filled is equal to the maximum amount, wait for spots to free   
      while (count == maximumIndex)
      {
        pthread_cond_wait(&spotsFree, &mutex);
      }

      int randomNumber = rand() % 100;
    
      // fill the next empty bucket spot with a random number between 0 and 99  
      bucket[count] = randomNumber;
      // once filled, increase the count
      count++;

      // signal to the producers consumers that there is food available
      pthread_cond_signal(&foodAvailable);
      std::cout << "Produced " << randomNumber << std::endl;

      // unlock thread to give another thread access
      pthread_mutex_unlock(&mutex);
      sleep(1);
    }
    return 0;
}

void* consumer(void* args) 
{
    while (runningA) 
    {
        pthread_mutex_lock(&mutex);

        // while the number of buckets filled is empty, wait until the signal that there is food available
        while (count == minimumIndex)
        {
          pthread_cond_wait(&foodAvailable, &mutex);
        }

        // eat the latest number to be put into the bucket array
        int eating = bucket[count - 1];
        // decrease count to signify something has been eaten 
        count--;

        // signal to the producer that there are bucket spots available to be filled
        pthread_cond_signal(&spotsFree);
        std::cout << "Consumed " << eating << std::endl;

        // unlock thread to give another thread access
        pthread_mutex_unlock(&mutex);
        sleep(1);
    }
    return 0;
}

int probA() {
    srand(time(NULL));
    pthread_mutex_init(&mutex, NULL);

    // initialise producer and consumer thread arrays
    pthread_t producers[PRODUCER_AMOUNT];
    pthread_t consumers[CONSUMER_AMOUNT];

    // create producer threads
    for (int i = 0; i < PRODUCER_AMOUNT; i++)
    {
      pthread_create(&producers[i], NULL, &producer, (void *)&producers);
    }
    
    // create consumer threads
    for (int i = 0; i < CONSUMER_AMOUNT; i++)
    {
      pthread_create(&consumers[i], NULL, &consumer, NULL);
    }

    // run the program for 10 seconds
    sleep(10);
    runningA = false;

    for (int i = 0; i < PRODUCER_AMOUNT; i++)
    {
        pthread_join(producers[i], NULL);
    }

    for (int i = 0; i < CONSUMER_AMOUNT; i++)
    {
        pthread_join(consumers[i], NULL);
    }

    return 0;
}