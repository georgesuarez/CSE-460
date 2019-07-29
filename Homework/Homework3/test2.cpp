#include <SDL2/SDL.h>
#include <SDL2/SDL_thread.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <fstream>
#include <sstream>
#include <iostream>
using namespace std;
//SDL_bool condition = SDL_FALSE;
SDL_mutex *mutex;
SDL_cond *readerQ;
SDL_cond *writerQ;
int readers = 0;
int writers = 0;
class RW
{
  public:
    int reader(void *data);
    int writer(void *data);
};
int RW::reader(void *data)
{
    while (1)
    {
        SDL_Delay(rand() % 3000);
        SDL_LockMutex(mutex);
        while (!(writers == 0))
        {
            SDL_CondWait(readerQ, mutex);
        }
        ++readers;
        SDL_UnlockMutex(mutex);
        ifstream file("counter.txt");
        if (file.good())
        {
            int count;
            file >> count;
            cout << *((string *)data) << " with value: " << count << endl;
            file.close();
        }
        else
        {
            cout << "Unable to read counter.txt" << endl;
        }
        SDL_LockMutex(mutex);
        //printf("\nThis is %s thread\n", (char *) data);
        if (--readers == 0)
        {
            SDL_CondSignal(writerQ);
        }
        SDL_UnlockMutex(mutex);
    }
}
int RW::writer(void *data)
{
    while (1)
    {
        SDL_Delay(rand() % 3000);
        SDL_LockMutex(mutex);
        while (!(readers == 0) && !(writers == 0))
        {
            SDL_CondWait(writerQ, mutex);
        }
        ++writers;
        SDL_UnlockMutex(mutex);

        int count = -1;
        ifstream read("counter.txt");
        if (read.good())
        {
            read >> count;
            read.close();
        }
        else
        {
            cout << "write file failed" << endl;
        }
        ++count;
        ofstream write("counter.txt", ios::trunc);
        write << count;
        write.close();

        cout << *((string *)data) << " with value: " << count << endl;

        SDL_LockMutex(mutex);
        --writers; //only one writer at one time
        SDL_CondSignal(writerQ);
        SDL_CondSignal(readerQ);
        SDL_UnlockMutex(mutex);
    }
}
int main()
{
    RW result;
    //thread identifiers
    SDL_Thread *idr[20];
    SDL_Thread *idw[3];

    mutex = SDL_CreateMutex();
    readerQ = SDL_CreateCond();
    writerQ = SDL_CreateCond();

    for (int i = 0; i < 3; ++i)
    {
        stringstream cruz;
        cruz << "writer: " << i;
        string *name = new string(cruz.str());
        idw[i] = SDL_CreateThread(result.writer(void *) name);
    }

    for (int i = 0; i < 20; ++i)
    {
        stringstream cruz;
        cruz << "reader: " << i;
        string *name = new string(cruz.str());
        idr[i] = SDL_CreateThread(result.reader(void *) name);
    }
    SDL_WaitThread(idw[0], NULL);
    SDL_DestroyCond(readerQ);
    SDL_DestroyCond(writerQ);
    SDL_DestroyMutex(mutex);
    return 0;
}