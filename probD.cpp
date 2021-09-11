#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <semaphore.h>
#include <iostream>
#include <sstream>
using std::cout;

#define BARBER_THREADS 2
#define NUMBER_SEATS 5
#define SEATS_ARE_FULL 0 

void addCxToQueue(int cxNumber);
void removeCxFromQueue();

pthread_mutex_t appointmentInProgress;
pthread_mutex_t updateQueue;
pthread_mutex_t updateSeatsAvailable;
sem_t customer;
sem_t barber;
pthread_t customerCreator;
bool running = true;
int seatsAvailable = NUMBER_SEATS;
int queue[NUMBER_SEATS];
int placeInQueue = 1;
int noCustomersVisited = 1;
int maxRandomNumber = 100;
int sleepTimeDivisor = 10;


void* cutHair(void* args)
{
    while (running)
    {
        sem_wait(&customer);

        pthread_mutex_lock(&updateSeatsAvailable);

        pthread_mutex_lock(&updateQueue);
        removeCxFromQueue();
        pthread_mutex_unlock(&updateQueue);
        
        if (seatsAvailable < NUMBER_SEATS)
        {
            seatsAvailable++;
        }
            
        pthread_mutex_unlock(&updateSeatsAvailable);

        sem_post(&barber);

        pthread_mutex_lock(&appointmentInProgress);

        std::stringstream cutting;
        cutting << "Barber " << pthread_self() << " is cutting a customer's hair.\n";
        std::cout << cutting.str();
        cutting.str("");
        cutting.clear();

        int haircutTime = rand() % maxRandomNumber;
        sleep(haircutTime/sleepTimeDivisor);

        pthread_mutex_unlock(&appointmentInProgress);

        std::stringstream complete;
        complete << "Haircut complete.\n";
        std::cout << complete.str();
        complete.str("");
        complete.clear();

        sleep(1);
    }
    pthread_exit(NULL);
}

void* waitingRoom(void* args)
{
    while(running)
    {   
        pthread_mutex_lock(&updateSeatsAvailable);
        if (seatsAvailable > SEATS_ARE_FULL)
        {
            std::stringstream thanks;
            thanks << "Thanks for coming Customer " << noCustomersVisited << "! We have a seat for you.\n";
            std::cout << thanks.str();
            thanks.str("");
            thanks.clear();
            sleep(1);

            pthread_mutex_lock(&updateQueue);
            addCxToQueue(noCustomersVisited);
            pthread_mutex_unlock(&updateQueue);
            
            noCustomersVisited++;
            seatsAvailable--;
            int cxWaiting = NUMBER_SEATS - seatsAvailable;

            std::stringstream cxQueue;
            cxQueue << "There are currently " << cxWaiting << " customers waiting.\nCustomer " << queue[0] << " is up next!\n";
            std::cout << cxQueue.str(); 
            cxQueue.str("");
            cxQueue.clear();

            sleep(1);

            pthread_mutex_unlock(&updateSeatsAvailable);

            sem_post(&customer);
            sem_wait(&barber);

        }
        else
        {
            std::stringstream sorry;
            sorry << "So sorry, " << noCustomersVisited << " we are full. Come back another time!\n";
            std::cout << sorry.str();
            sorry.str("");
            sorry.clear();

            noCustomersVisited++;

            pthread_mutex_unlock(&updateSeatsAvailable);
        }
        sleep(1);
    }
    return 0;
}

void addCxToQueue(int cxNumber)
{
    queue[placeInQueue - 1] = cxNumber;
}

void removeCxFromQueue()
{
    for (int i = 0; i < NUMBER_SEATS - 1; i++)
    {
        queue[i] = queue[i+1];
    }
}

void* arrivingCustomers(void* args)
{
    while(running)
    {
        std::stringstream cxLooking;
        cxLooking << "Customer " << noCustomersVisited << " is entering to look for availability. Precede to counter.\n";
        std::cout << cxLooking.str();
        cxLooking.str("");
        cxLooking.clear();

        pthread_t continuousCustomers;
        pthread_attr_t contCxAttribute;
        pthread_attr_init(&contCxAttribute);
        pthread_attr_setdetachstate(&contCxAttribute, 1);
        pthread_create(&continuousCustomers, &contCxAttribute, &waitingRoom, NULL);
        pthread_attr_destroy(&contCxAttribute); 

        int nextCx = rand() % maxRandomNumber;
        sleep(nextCx/sleepTimeDivisor);
    }
    pthread_exit(NULL);
}


int probD()
{
    srand(time(NULL));
    sem_init(&customer, 0, 0);
    sem_init(&barber, 0, 0);
    pthread_mutex_init(&appointmentInProgress, NULL);
    pthread_mutex_init(&updateQueue, NULL);
    pthread_mutex_init(&updateSeatsAvailable, NULL);

    pthread_t barbers[BARBER_THREADS];

    pthread_create(&customerCreator, NULL, &arrivingCustomers, NULL);

    for (int i = 0; i < BARBER_THREADS; i++)
    {
        pthread_create(&barbers[i], NULL, &cutHair, NULL);
    }

    sleep(10);
    running = false;
    pthread_join(customerCreator, NULL);
    for (int i = 0; i < BARBER_THREADS; i++)
    {
        pthread_join(barbers[i], NULL);
    }
    
    return 0;
}