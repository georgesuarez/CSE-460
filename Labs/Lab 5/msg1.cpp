//msg1.cpp
/* Here's the receiver program. */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define MAX_TEXT 512

struct my_msg_st
{
    long int my_msg_type;
    char some_text[BUFSIZ];
};

int main()
{
    int running = 1;
    struct my_msg_st some_data, send_data;
    long int msg_to_receive = 1;
    char buffer[BUFSIZ];


    /* First, we set up the message queue. */

    int msgid1 = msgget((key_t)1234, 0666 | IPC_CREAT);
    int msgid2 = msgget((key_t)2345, 0666 | IPC_CREAT);

    if (msgid1 == -1 || msgid2 == -1)
    {
        fprintf(stderr, "msgget failed with error: %d\n", errno);
        exit(EXIT_FAILURE);
    }
    /* Then the messages are retrieved from the queue, until an end message is encountered.
 Lastly, the message queue is deleted. */

    while (running)
    {
        printf("\nWaiting...\n");
        if (msgrcv(msgid1, (void *)&some_data, BUFSIZ, msg_to_receive, 0) == -1)
        {
            fprintf(stderr, "msgrcv failed with error: %d\n", errno);
            exit(EXIT_FAILURE);
        } 

        printf("You wrote: %s", some_data.some_text);

        if (strncmp(some_data.some_text, "end", 3) == 0)
        {
            running = 0;
        }
        else
        {
            printf("Enter some text: ");
            fgets(buffer, BUFSIZ, stdin);
            send_data.my_msg_type = 1;
            strcpy(send_data.some_text, buffer);

            if (msgsnd(msgid2, (void*) &send_data, MAX_TEXT, 0) == -1)
            {
                fprintf(stderr, "msgsnd failed\n");
                exit(EXIT_FAILURE);
            }
            if (strncmp(buffer, "end", 3) == 0)
            {
                running = 0;
            }
        }
    }

    if (msgctl(msgid1, IPC_RMID, 0) == -1)
    {
        fprintf(stderr, "msgctl(IPC_RMID) failed\n");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}