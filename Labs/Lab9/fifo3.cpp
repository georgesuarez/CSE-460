#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_thread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <unistd.h>
#include <deque>
using namespace std;
class Cframe
{
  public:
    int frameNo;
    // frame number
    int pageNo;
    // page number
    int r;
    // reference bit
    Cframe(int n, int p)
    {
        frameNo = n;
        pageNo = p;
        // no page loaded at beginning
        r = 0;
    }
};
deque<Cframe> Q;
int nFaults = 0;
int page, frame;
SDL_mutex *mutex1;
SDL_cond *updateQueue; // condition varaible
bool update = false;
bool quit = false;
#define MAX_TEXT 512
struct my_msg_st
{
    long int my_msg_type;
    char some_text[MAX_TEXT];
};
int displayMsg(void *data)
{
    struct my_msg_st some_data;
    int msgid;
    char buffer[BUFSIZ];
    msgid = msgget((key_t)1234, 0666 | IPC_CREAT);
    if (msgid == -1)
    {
        fprintf(stderr, "msgget failed with error: %d\n", errno);
        exit(EXIT_FAILURE);
    }
    while (true)
    {
        SDL_LockMutex(mutex1);
        while (!update && !quit)
        {
            SDL_CondWait(updateQueue, mutex1);
        }
        update = false;
        SDL_LockMutex(mutex1);
        sprintf(buffer, "%d,%d,%d\n", page, frame, nFaults);
        some_data.my_msg_type = 1;
        strcpy(some_data.some_text, buffer);
        if (msgsnd(msgid, (void *)&some_data, MAX_TEXT, 0) == -1)
        {
            fprintf(stderr, "msgsnd failed\n");
            exit(EXIT_FAILURE);
        }
        if (page == -99)
        {
            break;
        }
    }
    exit(EXIT_SUCCESS);
}
void fault()
{
    nFaults++;
}
int search(deque<Cframe> &q, int p)
{
    int n = q.size();
    for (int i = 0; i < n; i++)
    {
        if (q[i].pageNo == p)
        {
            q[i].r = 1;
            // recently referenced
            return q[i].frameNo;
        }
    }
    return -1;
}
int main()
{
    SDL_Thread *tid = SDL_CreateThread(displayMsg, "Send Message" ,(char *)"Send-thread");
    int maxFrames;
    cout << "\nEnter max. number of frames allowed in main memory: ";
    cin >> maxFrames;
    int n;
    cout << "Enter sequence of page requests (-99 to terminate).\n";
    while (true)
    {
        cout << "New page: ";
        cin >> page;
        if (page == -99)
        {
            quit = true;
            SDL_CondSignal(updateQueue);
            break;
        }
        if ((frame = search(Q, page)) != -1)
        {
            ;
        }
        else
        {
            n = Q.size();
            if (n < maxFrames)
            {
                Cframe aFrame(n, page);
                Q.push_back(aFrame);
                frame = aFrame.frameNo;
            }
            else
            {
                Cframe aFrame = Q.front();
                while (aFrame.r == 1)
                {
                    // find oldest page that
                    Q.pop_front();
                    // has r == 0; set all r
                    aFrame.r = 0;
                    // flags to 0 until one is
                    Q.push_back(aFrame);
                    // found
                    aFrame = Q.front();
                }
                Q.pop_front();
                aFrame.pageNo = page;
                Q.push_back(aFrame);
                frame = aFrame.frameNo;
            }
            fault();
        }
        SDL_LockMutex(mutex1);
        update = true;
        SDL_CondSignal(updateQueue);
        SDL_UnlockMutex(mutex1);
    }
    SDL_WaitThread(tid, NULL);
    return 0;
}