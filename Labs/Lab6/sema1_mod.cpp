//sema1.cpp
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <iostream>
#include <stdio.h>

using namespace std;

static int sem_id; //semaphore id

// #if defined(__GNU_LIBRARY__) && !defined(_SEM_SEMUN_UNDEFINED)
/* union semun is defined by including <sys/sem.h> */
// #else
// /* according to X/OPEN we have to define it ourselves */
// union semun {
//     int val;               /* value for SETVAL */
//     struct semid_ds *buf;  /* buffer for IPC_STAT, IPC_SET */
//     unsigned short *array; /* array for GETALL, SETALL */
//                            /* Linux specific part: */
//     struct seminfo *__buf; /* buffer for IPC_INFO */
// };
// #endif

//initializes semaphore using SETVAL
static int set_semvalue(int val)
{
    union semun sem_union; // sem_union;

    sem_union.val = val;
    if (semctl(sem_id, 0, SETVAL, sem_union) == -1)
        return (0);
    return 1;
}

//delete semaphore
static int del_semvalue()
{
    union semun sem_union; // sem_union;

    sem_union.val = 1;
    if (semctl(sem_id, 0, IPC_RMID, sem_union) == -1)
        return (0);
    return 1;
}

static int SEM_DOWN()
{
    struct sembuf b;

    b.sem_num = 0;
    b.sem_op = -1; //P(), i.e. down()
    b.sem_flg = SEM_UNDO;
    if (semop(sem_id, &b, 1) == -1)
    {
        cout << "Semaphore DOWN() failed!" << endl;
        return 0;
    }

    return 1;
}

static int SEM_UP()
{
    struct sembuf b;

    b.sem_num = 0;
    b.sem_op = 1; //V(), i.e. UP()
    b.sem_flg = SEM_UNDO;
    if (semop(sem_id, &b, 1) == -1)
    {
        cout << "Semaphore UP() failed!" << endl;
        return 0;
    }
    return 1;
}
int main(int argc, char *argv[])
{
    int i, pause_time;
    char ce = 'e', cl = 'l';

    srand((unsigned int)getpid()); //seed RNG with process id

    sem_id = semget((key_t)1234, 1, 0666 | IPC_CREAT);

    if (argc > 0)
    {
        if (!set_semvalue(1))
        { //process can enter CS
            cout << "Semaphore initialized failed!" << endl;
            exit(EXIT_FAILURE);
        }
        if (argc > 1)
        {
            ce = 'E';
            cl = 'L';
        }
        sleep(1);
    }
    else
    {
        if (!set_semvalue(0))
        { //process will be blocked initially
            cout << "Semaphore initialized failed!" << endl;
            exit(EXIT_FAILURE);
        }
        sleep(1);
    }

    //enter and leave critical section 10 times
    for (i = 0; i < 10; i++)
    {
        if (!SEM_DOWN())
            exit(EXIT_FAILURE);
        cout << ce;
        fflush(stdout);          //entering critical section
        pause_time = rand() % 3; //simulate critical section
        sleep(pause_time);
        cout << cl;
        fflush(stdout); //leaving critical section
        if (!SEM_UP())
            exit(EXIT_FAILURE); //signal other waiting process
        pause_time = rand() % 2;
        sleep(pause_time);
    }
    cout << endl
         << getpid() << " finished!" << endl;
    if (argc > 0)
    {
        sleep(2);
        del_semvalue();
    }
    exit(EXIT_SUCCESS);
}