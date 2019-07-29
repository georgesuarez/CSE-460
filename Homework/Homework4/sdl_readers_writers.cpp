#include <SDL2/SDL.h>
#include <SDL2/SDL_thread.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <signal.h>
#include <unistd.h>
#include <iostream>
#include <fstream>

using namespace std;

SDL_bool condition = SDL_FALSE;
SDL_mutex *mutex1;
SDL_cond *readerQueue; //condition variable
SDL_cond *writerQueue; //condition variable

int readerCount = 0;
int writerCount = 0;
bool quit = false;

string fileName = "counter.txt";

int reader(void *data)
{
    while (!quit)
    {
        SDL_Delay(rand() % 3000);
        SDL_LockMutex(mutex1);
        while (!(writerCount == 0))
        {
            SDL_CondWait(readerQueue, mutex1);
        }

        readerCount++;

        SDL_UnlockMutex(mutex1);

        //read
        int count = -1;
        ifstream inFile;
        inFile.open(fileName.c_str());

        if (inFile.good())
        {
            inFile >> count;
            inFile.close();
        }

        SDL_LockMutex(mutex1);
        printf("\nThis is %s thread\n", (char *)data);
        if (--readerCount == 0)
        {
            SDL_CondSignal(writerQueue);
        }
        SDL_UnlockMutex(mutex1);
    }
}

int writer(void *data)
{
    while (!quit)
    {
        SDL_Delay(rand() % 3000);
        SDL_LockMutex(mutex1);
        while (!((readerCount == 0) && (writerCount == 0)))
        {
            SDL_CondWait(writerQueue, mutex1);
        }

        writerCount++;

        SDL_UnlockMutex(mutex1);

        int count = -1;
        ifstream inFile;
        inFile.open(fileName.c_str());
        if (inFile.good())
        {
            inFile >> count;
            inFile.close();
        }

        ofstream outFile;
        outFile.open(fileName.c_str());
        if (outFile.good())
        {
            outFile << count;
            outFile.close();
        }

        SDL_LockMutex(mutex1);
        printf("\nThis is %s thread %d\n", (char *)data, count);
        writerCount--; //only one writer at one time
        count++;
        SDL_CondSignal(writerQueue);
        SDL_CondBroadcast(readerQueue);
        SDL_UnlockMutex(mutex1);
    }
}

void func(int sig)
{
    quit = true;
}

const int READERS = 20;
const int WRITERS = 3;

int main()
{
    SDL_Thread *idr[20], *idw[3]; //thread identifiers
    char *readerNames[] = {(char *)"reader 1",
                                  (char *)"reader 2",
                                  (char *)"reader 3",
                                  (char *)"reader 4",
                                  (char *)"reader 5",
                                  (char *)"reader 6",
                                  (char *)"reader 7",
                                  (char *)"reader 8",
                                  (char *)"reader 9",
                                  (char *)"reader 10",
                                  (char *)"reader 11",
                                  (char *)"reader 12",
                                  (char *)"reader 13",
                                  (char *)"reader 14",
                                  (char *)"reader 15",
                                  (char *)"reader 16",
                                  (char *)"reader 17",
                                  (char *)"reader 18",
                                  (char *)"reader 19",
                                  (char *)"reader 20"};

    char *writerNames[] = {(char *)"writer 1", (char *)"writer 2", (char *)"writer 3"};

    mutex1 = SDL_CreateMutex();
    readerQueue = SDL_CreateCond();
    writerQueue = SDL_CreateCond();

    for (int i = 0; i < READERS; i++)
    {
        idr[i] = SDL_CreateThread(reader, "Reader Thread", readerNames[i]);
    }

    for (int i = 0; i < WRITERS; i++)
    {
        idw[i] = SDL_CreateThread(writer, "Writer Thread", writerNames[i]);
    }

    (void)signal(SIGINT, func);

    for (int i = 0; i < READERS; i++)
    {
        SDL_WaitThread(idr[i], NULL);
    }

    for (int i = 0; i < WRITERS; i++)
    {
        SDL_WaitThread(idw[i], NULL);
    }

    SDL_DestroyCond(readerQueue);
    SDL_DestroyCond(writerQueue);
    SDL_DestroyMutex(mutex1);
    return 0;
}
