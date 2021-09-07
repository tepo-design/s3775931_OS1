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

pthread_mutex_t appointmentInProgress;
pthread_mutex_t printText;
pthread_mutex_t updateSeatsAvailable;
sem_t customer;
sem_t barber;
pthread_t customerCreator;
pthread_t continuousCustomers;
bool running = true;
int seatsAvailable = 5;

void* cutHair(void* args)
{
    while (running)
    {
        sem_wait(&customer);
        pthread_mutex_lock(&updateSeatsAvailable);
        if (seatsAvailable < 5)
        {
            seatsAvailable++;

        }
        pthread_mutex_unlock(&updateSeatsAvailable);
        sem_post(&barber);
        pthread_mutex_lock(&appointmentInProgress);

        int haircutTime = rand() % 100;

        std::stringstream cutting;
        cutting << "Barber " << pthread_self() << " is cutting a customer's hair.\n";
        std::cout << cutting.str();

        sleep(1);
        pthread_mutex_unlock(&appointmentInProgress);

        std::stringstream complete;
        complete << "Haircut complete.\n";
        std::cout << complete.str();
        sleep(1);

    }
    pthread_exit(NULL);
}

void* waitingRoom(void* args)
{
    while(running)
    {   
        pthread_mutex_lock(&updateSeatsAvailable);
        if (seatsAvailable > 0)
        {
            std::stringstream thanks;
            thanks << "Thanks for coming Thread " << pthread_self() << "! We have a seat for you.\n";
            std::cout << thanks.str();
            sleep(1);

            seatsAvailable--;

            int cxWaiting = 5 - seatsAvailable;
            printf("%d", cxWaiting);
            std::stringstream cxQueue;
            cxQueue << "There are currently " << cxWaiting << " customers waiting.\n";
            std::cout << cxQueue.str();
            sleep(1);
            pthread_mutex_unlock(&updateSeatsAvailable);


            sem_post(&customer);
            sem_wait(&barber);

            std::stringstream customerBeingServed;
            customerBeingServed << "Thread " << pthread_self() << " is getting their hair cut!";
            std::cout << customerBeingServed.str();

            // pthread_mutex_lock(&printText);
            // std::cout << "You're up!" << std::endl;
            // pthread_mutex_unlock(&printText);
            sleep(1);

        }
        else
        {
            // pthread_mutex_lock(&printText);
            std::stringstream sorry;
            sorry << "So sorry, we are full. Come back another time!\n";
            std::cout << sorry.str();
            pthread_mutex_unlock(&updateSeatsAvailable);

            // pthread_mutex_unlock(&printText);
            sleep(1);
        }
        //pthread_mutex_unlock(&updateSeatsAvailable);

        sleep(1);
    }
    pthread_exit(NULL);
}

void* arrivingCustomers(void* args)
{
    while(running)
    {
        // pthread_mutex_lock(&printText);
        std::stringstream cxLooking;
        cxLooking << "Customer is entering to look for availability. Precede to counter.\n";
        std::cout << cxLooking.str();
        // pthread_mutex_unlock(&printText);
        pthread_create(&continuousCustomers, NULL, &waitingRoom, NULL);
        int nextCx = rand() % 100;
        sleep(1);
    }
}


int main()
{
    srand(time(NULL));
    sem_init(&customer, 0, 0);
    sem_init(&barber, 0, 0);
    pthread_mutex_init(&appointmentInProgress, NULL);
    pthread_mutex_init(&printText, NULL);

    pthread_mutex_init(&updateSeatsAvailable, NULL);
    pthread_t barbers[BARBER_THREADS];

    pthread_create(&customerCreator, NULL, &arrivingCustomers, NULL);

    for (int i = 0; i < BARBER_THREADS; i++)
    {
        pthread_create(&barbers[i], NULL, &cutHair, NULL);
    }

    sleep(30);
    running = false;
    return 0;
}
