//--------------------------------------------
// NAME: Emil Dermendjiev
// EA6
// PROBLEM: #4
// FILE NAME: xxxxxx.yyy.zzz (unix file name)
// FILE PURPOSE:
// This is a program that simulates a RTS game
// using threads
// ...
//---------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

//---------------------------------------------
//define section
//number of workers a player has
#define WORKERS 4

//size of resources
#define RESOURCE_SIZE 8

//training a warrior costs 20 resources
#define TRAINING_COST 20

//building a barrack costs 100 resources
#define BARRACK_COST 100

//resources on the map
#define MAP_RESOURCES 4096

//the number of workers and warriors combined can not be over 100
#define WORKERS_WARRIORS_MAXNUM 100
//
//------------------------------------------------
//
//------------------------------------------------
//creating a structure with everything a player has
typedef struct baseStation
{
    int gatheredResources;
    int usedResources;
    int mapResources;
    int barracks;
    int trainedWarrior;
    char command;
}Station;
//Initializing all elements in the Base structure
Station Base = {0, 0, MAP_RESOURCES, 0, 0, 0};
//------------------------------------------------

//------------------------------------------------
//functions section
//--------------------------------------------
// FUNCTION: void *workers(void *num)
// Starting point for threads
// Executing all other functions
// PARAMETERS:
// void *num - index for each thread
void *workers(void *num);
//------------------------------------------------
//--------------------------------------------
// FUNCTION: void *commandExecution()
// Starting point for commandThread
// Gets command from user
// PARAMETERS:
// None
void *commandExecution();
//------------------------------------------------
//--------------------------------------------
// FUNCTION: void resourceSearch(int num)
// Simulating searching for resources and printing to screen
// PARAMETERS:
// void *num - index for each thread
void resourceSearch(int num);
//----------------------------------------------
//--------------------------------------------
// FUNCTION: void transportingResources(int num)
// simulating transporting of resources and printing to screen
// PARAMETERS:
// void *num - index for each thread
void transportingResources(int num);
//--------------------------------------------
//--------------------------------------------
// FUNCTION: void unloadingResources(int num)
// simulating unloading resources to base and printing to screen
// increasing gathered resources and decreasing the available resources on the map
// PARAMETERS:
// void *num - index for each thread
void unloadingResources(int num);
//--------------------------------------------
//--------------------------------------------
// FUNCTION: void getCommand()
// gets command from user
// PARAMETERS:
// None
void getCommand();
//--------------------------------------------
//--------------------------------------------
// FUNCTION: void commandCheck()
// checks for a command from user and executes it
// PARAMETERS:
// None
void commandCheck();
//--------------------------------------------
//--------------------------------------------
// FUNCTION: void buildBarrack(int num)
// simulates building a barrack and prints to screan
// decreases gathered resources and increases number of barracks
// PARAMETERS:
// void *num - index for each thread
void buildBarrack(int num);
//--------------------------------------------
//--------------------------------------------
// FUNCTION: void trainWarrior()
// simulates training a warrior and prints to screen
// decreases gathered resources and increases number of trained warriors
// PARAMETERS:
// None
void trainWarrior();
//----------------------------------------------
//----------------------------------------------
//Creating mutex for thread syncronization
pthread_mutex_t result_mutex;

int main()
{
    size_t idx = 0;
    //creating a thread for each worker
    pthread_t threads[WORKERS];
    //creating thread for commands
    pthread_t commandThread;

    //initializing mutex
	if(0 != pthread_mutex_init(&result_mutex, NULL))
    {
        printf("Mutex init error\n");
    }

    //Starting a thread for each worker
    for (idx = 0; idx < WORKERS; idx++)
    {
        if (0 != pthread_create(&threads[idx], NULL, workers, (void *)idx))
        {
            fprintf(stderr, "Error creating thread\n");
        }
    }

    //starting the command thread
    if (0 != pthread_create(&commandThread, NULL, commandExecution, NULL))
    {
        fprintf(stderr, "Error creating thread\n");
    }

    for(idx = 0; idx < WORKERS; idx++)
    {
        if(0 != pthread_join(threads[idx], NULL))
        {
           printf("No errors\n");
        }
    }

    //destroying mutex
    pthread_mutex_destroy(&result_mutex);

    //printing all resources
    printf("Map resources: %d\n", MAP_RESOURCES);
    printf("Gathered resources: %d\n", Base.gatheredResources);
    printf("Used resorces: %d\n", Base.usedResources);
    printf("Available resorces: %d\n", Base.mapResources);


    return 0;
}

void *commandExecution()
{
    //while there are resources and the trained warriors are less than 20 the game continues
    //the user can type a command at any time
    while((Base.mapResources > RESOURCE_SIZE) && (Base.trainedWarrior < 20))
    {
        getCommand();
    }
    return 0;
}

void *workers(void *num)
{
    size_t threadNum = (size_t)num;

    //while there are resources and the trained warriors are less than 20 the game continues
    //all workers are searching, transporting, unloading and checking for a command
    while((Base.mapResources > RESOURCE_SIZE) && (Base.trainedWarrior < 20))
    {
        //first thread locks the three operations
        pthread_mutex_lock(&result_mutex);

        resourceSearch(threadNum);
        transportingResources(threadNum);
        unloadingResources(threadNum);

        //unlocking the operations for the next thread
        pthread_mutex_unlock(&result_mutex);

        commandCheck(threadNum);

    }

    return 0;
}

void resourceSearch(int num)
{
    if(Base.mapResources >= RESOURCE_SIZE)
    {
        printf("Worker %d is searching\n", num+1);
        sleep(1);
    }
    else
    {
        printf("There are no more resources on this map\n");
    }
}

void transportingResources(int num)
{
    if(Base.mapResources >= RESOURCE_SIZE)
    {
        printf("Worker %d is transporting\n", num+1);
        sleep(1);
    }

}

void unloadingResources(int num)
{
    if(Base.mapResources >= RESOURCE_SIZE)
    {
        //after every search resources are gathered so we add the resources the the gathered in the base
        Base.gatheredResources += RESOURCE_SIZE;
        //after gathering there are less map resources
        Base.mapResources -= RESOURCE_SIZE;

        printf("Worker %d unloaded resources to Base station\n", num+1);
        sleep(1);
    }

}

void getCommand()
{
    scanf("%c", &Base.command);
    fflush(stdin);
}

void buildBarrack(int num)
{
    //building barrack takes 100 resources so we check if we have 100 or more
    if(Base.gatheredResources >= BARRACK_COST)
    {
        //building is done by only one worker so the thread locks this operation
        pthread_mutex_lock(&result_mutex);

        printf("Worker %d is constructing new building barracks\n", num+1);

        //taking 100 resources from the gathered in the base
        Base.gatheredResources -= BARRACK_COST;
        //keeping count of used resources
        Base.usedResources += BARRACK_COST;
        //reseting the command
        Base.command = 0;

        //unlocking
        pthread_mutex_unlock(&result_mutex);

        sleep(10);
        //building one barrack
        Base.barracks++;
        printf("Worker %d completed new building barracks\n", num+1);

    }
    else
    {
        printf("Not enough resources\n");
    }
}

void trainWarrior()
{
    //training a warrior costs 20 resources and at least one barrack is needed
    if((Base.gatheredResources >= TRAINING_COST) && (Base.barracks > 0))
    {
        //training is done by only one worker at a time so lock is needed
        pthread_mutex_lock(&result_mutex);

        printf("Warrior is being trained\n");
        //taking the needed resources from the gathered at the base
        Base.gatheredResources -= TRAINING_COST;
        //keeping count of used resources
        Base.usedResources += TRAINING_COST;
        //reseting the command
        Base.command = 0;
        //unlocking
        pthread_mutex_unlock(&result_mutex);

        sleep(4);
        //increasing the number of trained warriors
        Base.trainedWarrior++;
        printf("Warrior is ready for duty\n");
    }
    else
    {
        printf("Operation not supported\n");
    }
}

void commandCheck(int num)
{
    //checking the received command
    if('b' == Base.command)
    {
        buildBarrack(num);
        //reseting the command value after work is done
        Base.command = 0;
    }
    else if('w' == Base.command)
    {
        trainWarrior();
        Base.command = 0;
    }
    else if(0 == Base.command)
    {
        //if there is no command from user just wait 2 seconds
        sleep(2);
    }
    else
    {
        printf("Invalid command\n");
        Base.command = 0;
    }
}
