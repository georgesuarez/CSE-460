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

SDL_sem *s[5];
bool quit = false;
int nEating = 0;
SDL_mutex *mutex;
int state[5];

void test(int i)
{
    //one semaphore per philosopher to lock choptstics
    // number of philosopers eating
    if (state[i] == HUNGRY && state[LEFT] != EATING && state[RIGHT] != EATING)
    {
        state[i] = EATING;
        SDL_SemPost(s[i]);
    }
}
void think(int i)
{
    SDL_Delay(rand() % 2000);
}
void take_chops(int i)
{
    SDL_LockMutex(mutex);
    state[i] = HUNGRY;
    printf("\nTaking choptstick %d", i);
    test(i);
    SDL_UnlockMutex(mutex);
}
void eat(int i)
{
    printf("\nPhilosopher %d eating!\n", i);
    SDL_Delay(rand() % 2000);
}
void put_chops(int i)
{
    SDL_LockMutex(mutex);
    state[i] = THINKING;
    test(LEFT);
    test(RIGHT);
    SDL_UnlockMutex(mutex);
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
            SDL_SemPost(s[i]);
            printf("\nUnlocking %d done", i);
        }
    }
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
        SDL_SemWait(s[l]);
        take_chops(l);
        SDL_Delay(rand() % 2000);
        SDL_SemWait(s[r]);
        take_chops(r);
        nEating++;
        eat(i);
        nEating--;
        put_chops(r);
        SDL_SemPost(s[r]);
        put_chops(l);
        SDL_SemPost(s[l]);
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

    SDL_Thread *p[5];

    const char *names[] = {"0", "1", "2", "3", "4"};
    for (int i = 0; i < 5; i++)
        s[i] = SDL_CreateSemaphore(1);
    for (int i = 0; i < 5; i++)
        p[i] = SDL_CreateThread(philosopher, "Philosopher", (char *)names[i]);
    for (int i = 0; i < 5; i++)
        SDL_WaitThread(p[i], NULL);
    for (int i = 0; i < 5; i++)
        SDL_DestroySemaphore(s[i]);
    return 0;
}
