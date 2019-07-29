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
            SDL_CondWait(readerQueue, mutex1);

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
        printf("\nThis is %s thread: %d\n", (char *)data, count);
        printf("Counter value: %d\n", count);
        if (--readerCount == 0)
            SDL_CondSignal(writerQueue);
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
            SDL_CondWait(writerQueue, mutex1);

        writerCount++;

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
            outFile << ++count;
            outFile.close();
        }

        SDL_LockMutex(mutex1);
        writerCount--; //only one writer at one time
        printf("\nThis is %s thread: %d\n", (char *)data, count);
        printf("Counter value: %d\n", count);
        SDL_CondSignal(writerQueue);
        SDL_CondBroadcast(readerQueue);
        SDL_UnlockMutex(mutex1);
    }
}

int main()
{
    SDL_Thread *idr[20], *idw[3]; //thread identifiers
    char readerNames[20][10];
    char writerNames[3][10];

    for (int i = 0; i < 20; i++)
    {
        cout << readerNames[i] << "Reader: " << i + 1 << endl;
        idr[i] = SDL_CreateThread(reader, "Reader Thread", readerNames[i]);
    }

    for (int i = 0; i < 3; i++)
    {
        cout << writerNames[i] << "Writer: " << i + 1 << endl;
        idw[i] = SDL_CreateThread(writer, "Writer Thread", writerNames[i]);
    }

    mutex1 = SDL_CreateMutex();
    readerQueue = SDL_CreateCond();
    writerQueue = SDL_CreateCond();

    for (int i = 0; i < 20; i++)
    {
        SDL_WaitThread(idr[i], NULL);
    }

    for (int i = 0; i < 3; i++)
    {
        SDL_WaitThread(idw[i], NULL);
    }

    SDL_DestroyCond(readerQueue);
    SDL_DestroyCond(writerQueue);
    SDL_DestroyMutex(mutex1);
    return 0;
}
