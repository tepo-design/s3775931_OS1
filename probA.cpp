#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

#define PRODUCER_AMOUNT 5
#define CONSUMER_AMOUNT 5

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t spotsFree = PTHREAD_COND_INITIALIZER; 
pthread_cond_t foodAvailable = PTHREAD_COND_INITIALIZER;

int bucket[10];
int count = 0;
bool running = true;

void* producer(void* args) {
    while (running) {
      while (count == 9)
      {
        pthread_cond_wait(&spotsFree, &mutex);
      }

      int randomNumber = rand() % 100;

      bucket[count] = randomNumber;
      count++;

      pthread_cond_signal(&foodAvailable);
      pthread_mutex_unlock(&mutex);

      printf("Produced %d\n", randomNumber);
      // sleep(1);
    }
    return 0;
}

void* consumer(void* args) {
    while (running) {
        pthread_mutex_lock(&mutex);
 
        while (count == 0)
        {
          pthread_cond_wait(&foodAvailable, &mutex);
        }

        int eating = bucket[count - 1];
        count--;


        pthread_cond_signal(&spotsFree);
        pthread_mutex_unlock(&mutex);

        printf("Consumed %d\n", eating);
        sleep(1);
    }
    return 0;
}

int main(int argc, char* argv[]) {
    srand(time(NULL));
    pthread_mutex_init(&mutex, NULL);

    pthread_t producers[5];
    pthread_t consumers[5];

    for (int i = 0; i < PRODUCER_AMOUNT; i++)
    {
      pthread_create(&producers[i], NULL, &producer, NULL);
    }
    
    for (int i = 0; i < CONSUMER_AMOUNT; i++)
    {
      pthread_create(&consumers[i], NULL, &consumer, NULL);
    }

    sleep(10);
    running = false;
    pthread_mutex_destroy(&mutex);
    return 0;
}