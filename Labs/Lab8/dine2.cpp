/*
  dine2.cpp :  mutexes lock chopsticks
  Compile:  g++ -o  dine2 dine2.cpp -lSDL 
  Execute:  ./dine2
*/

#include <SDL2/SDL.h>
#include <SDL2/SDL_thread.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <signal.h>
#include <unistd.h>

#define LEFT (i - 1) % 5
#define RIGHT (i + 1) % 5
#define HUNGRY 0
#define EATING 1
#define THINKING 2

SDL_sem *s[5]; // one semaphore per philosopher
bool quit = false;
int nEating = 0; // number of philosopers eating
SDL_mutex *mutex;
int state[5]; //array keeping track of everyone's state
void test(int i)
{
    int left = LEFT;
    if (left < 0)
        left += 5;

    if (state[i] == HUNGRY && state[left] != EATING &&
        state[RIGHT] != EATING)
    {
        state[i] = EATING;
        SDL_SemPost(s[i]);
    }
}

void putdown(int i)
{
    SDL_LockMutex(mutex);
    state[i] = THINKING;
    test(LEFT);
    test(RIGHT);
    SDL_UnlockMutex(mutex);
}

SDL_sem *chopLock[5]; //locks for chopsticks
bool quit = false;
int nEating = 0; // number of philosopers eating

void think(int i)
{
    SDL_Delay(rand() % 2000);
}

void eat(int i)
{
    printf("\nPhilosopher %d eating!\n", i);
    SDL_Delay(rand() % 2000);
}

void take_chops(int i)
{
    printf("\nTaking chopstick %d", i);
}

void put_chops(int i)
{
}

int philosopher(void *data)
{
    int i, l, r;
    i = atoi((char *)data);
    l = i; //left
    r = (i + 1) % 5;
    while (!quit)
    {
        think(i);
        printf("\nPhilosoper %d ", i);
        SDL_SemWait(chopLock[l]);
        take_chops(l);
        //    SDL_Delay ( rand() % 2000 );  //could lead to deadlock
        SDL_SemWait(chopLock[r]);
        take_chops(r);
        nEating++;
        eat(i);
        nEating--;
        put_chops(r);
        SDL_SemPost(chopLock[r]);
        put_chops(l);
        SDL_SemPost(chopLock[l]);
    }
}

void checkCount(int sig)
{
    if (sig == SIGINT)
        printf("\n%d philospers eating\n", nEating);
    else if (sig == SIGQUIT)
    {
        quit = true;
        printf("\nQuitting, please wait....\n");
        for (int i = 0; i < 5; i++)
        { // break any deadlock
            printf("\nUnlocking %d ", i);
            SDL_SemPost(chopLock[i]);
            printf("\nUnlocking %d done", i);
        }
    }
}

int main()
{
    struct sigaction act, actq;

    act.sa_handler = checkCount;
    sigemptyset(&act.sa_mask);
    sigaction(SIGINT, &act, 0);
    actq.sa_handler = checkCount;
    sigaction(SIGQUIT, &actq, 0);

    SDL_Thread *p[5]; //thread identifiers
    const char *names[] = {"0", "1", "2", "3", "4"};

    for (int i = 0; i < 5; i++)
        chopLock[i] = SDL_CreateSemaphore(1);
    for (int i = 0; i < 5; i++)
        p[i] = SDL_CreateThread(philosopher, "Philosopher", (char *)names[i]);

    for (int i = 0; i < 5; i++)
        SDL_WaitThread(p[i], NULL);
    for (int i = 0; i < 5; i++)
        SDL_DestroySemaphore(chopLock[i]);

    return 0;
}