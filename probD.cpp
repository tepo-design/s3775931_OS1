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
bool runningD = true;
int seatsAvailable = NUMBER_SEATS;
int queue[NUMBER_SEATS];
int placeInQueue = 1;
int noCustomersVisited = 1;
int maxRandomNumber = 100;
int sleepTimeDivisor = 10;
float postOutputSleep = 1;

void* cutHair(void* args)
{
    while (runningD)
    {
        // wait for customer to become available
        sem_wait(&customer);

        // lock the seats so only this thread can read/write
        pthread_mutex_lock(&updateSeatsAvailable);

        // lock the queue so only this thread can read/write
        pthread_mutex_lock(&updateQueue);
        removeCxFromQueue();
        pthread_mutex_unlock(&updateQueue);
        
        // increase seats available
        if (seatsAvailable < NUMBER_SEATS)
        {
            seatsAvailable++;
        }

        // release the seats so other threads can access read/write        
        pthread_mutex_unlock(&updateSeatsAvailable);

        // signal that barber is available to cut hair
        sem_post(&barber);


        pthread_mutex_lock(&appointmentInProgress);

        std::stringstream cutting;
        cutting << "Barber " << pthread_self() << " is cutting a customer's hair.\n";
        std::cout << cutting.str();
        cutting.str("");
        cutting.clear();

        // have barber thread sleep to signify hair cut time
        int haircutTime = rand() % maxRandomNumber;
        sleep(haircutTime/sleepTimeDivisor);

        pthread_mutex_unlock(&appointmentInProgress);

        std::stringstream complete;
        complete << "Haircut complete.\n";
        std::cout << complete.str();
        complete.str("");
        complete.clear();

        sleep(postOutputSleep);
    }
    pthread_exit(NULL);
}

void* waitingRoom(void* args)
{
    while(runningD)
    {   
        // lock available seats so conditional if statement is only accessible by one thread
        pthread_mutex_lock(&updateSeatsAvailable);
        // if there are seats available
        if (seatsAvailable > SEATS_ARE_FULL)
        {
            std::stringstream thanks;
            thanks << "Thanks for coming Customer " << noCustomersVisited << "! We have a seat for you.\n";
            std::cout << thanks.str();
            thanks.str("");
            thanks.clear();
            sleep(postOutputSleep);

            // lock the queue so it is only accessible by one thread at a time
            pthread_mutex_lock(&updateQueue);
            addCxToQueue(noCustomersVisited);
            pthread_mutex_unlock(&updateQueue);
            
            // increase how many customers have visited the store
            noCustomersVisited++;
            seatsAvailable--;
            int cxWaiting = NUMBER_SEATS - seatsAvailable;

            std::stringstream cxQueue;
            cxQueue << "There are currently " << cxWaiting << " customers waiting.\nCustomer " << queue[0] << " is up next!\n";
            std::cout << cxQueue.str(); 
            cxQueue.str("");
            cxQueue.clear();

            sleep(postOutputSleep);

            // unlock seats
            pthread_mutex_unlock(&updateSeatsAvailable);

            // signify customer is ready for haircut
            sem_post(&customer);
        
            // wait for the barber to be ready
            sem_wait(&barber);

        }
        else
        {
            // no seats available in the waiting room, advise the customer
            std::stringstream sorry;
            sorry << "So sorry, " << noCustomersVisited << " we are full. Come back another time!\n";
            std::cout << sorry.str();
            sorry.str("");
            sorry.clear();

            // still increase number of visits to the store as they still visited even if we were full
            noCustomersVisited++;

            pthread_mutex_unlock(&updateSeatsAvailable);
        }
        sleep(postOutputSleep);
    }
    return 0;
}

// add customer to the queue if there are enough chairs available
void addCxToQueue(int cxNumber)
{
    queue[placeInQueue - 1] = cxNumber;
}

// remove the customer from the queue once they have been served
void removeCxFromQueue()
{
    for (int i = 0; i < NUMBER_SEATS - 1; i++)
    {
        queue[i] = queue[i+1];
    }
}

// generate a continuous stream of customers 
void* arrivingCustomers(void* args)
{
    while(runningD)
    {
        std::stringstream cxLooking;
        cxLooking << "Customer " << noCustomersVisited << " is entering to look for availability. Precede to counter.\n";
        std::cout << cxLooking.str();
        cxLooking.str("");
        cxLooking.clear();
        sleep(postOutputSleep);

        // create a customer thread to enter the waiting room
        // set a detached attribute so memory is deallocated when thread ends
        pthread_t continuousCustomers;
        pthread_attr_t contCxAttribute;
        pthread_attr_init(&contCxAttribute);
        pthread_attr_setdetachstate(&contCxAttribute, 1);
        pthread_create(&continuousCustomers, &contCxAttribute, &waitingRoom, NULL);
        pthread_attr_destroy(&contCxAttribute); 
        pthread_join(continuousCustomers, NULL);

        int nextCx = rand() % maxRandomNumber;
        sleep(nextCx/sleepTimeDivisor);
    }
    pthread_exit(NULL);
}

// main function to run Problem D - Sleeping Barber problem
int probD()
{
    srand(time(NULL));

    // initialise metaphors
    sem_init(&customer, 0, 0);
    sem_init(&barber, 0, 0);
    // initalise mutexes
    pthread_mutex_init(&appointmentInProgress, NULL);
    pthread_mutex_init(&updateQueue, NULL);
    pthread_mutex_init(&updateSeatsAvailable, NULL);

    // create an array of barber threads using a global variable so any amount of barber threads can be used
    pthread_t barbers[BARBER_THREADS];

    // create the customerCreator thread that will initialise the arrivingCustomers function
    pthread_create(&customerCreator, NULL, &arrivingCustomers, NULL);

    // create all barber threads and send them to the cutHair function
    for (int i = 0; i < BARBER_THREADS; i++)
    {
        pthread_create(&barbers[i], NULL, &cutHair, NULL);
    }

    // run program for 10 seconds
    sleep(10);
    runningD = false;
    pthread_join(customerCreator, NULL);
    for (int i = 0; i < BARBER_THREADS; i++)
    {
        pthread_join(barbers[i], NULL);
    }
    
    return 0;
}