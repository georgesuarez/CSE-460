#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <unistd.h>
#include <iostream>

#define MAX_TEXT 512

struct my_msg_st
{
    long int my_msg_type;
    char some_text[MAX_TEXT];
};

int main()
{
    int running = 1;
    struct my_msg_st some_data;
    int msgid, page, frame, faults;
    msgid = msgget((key_t)1234, 0666 | IPC_CREAT);
    if (msgid == -1)
    {
        fprintf(stderr, "msgget failed with error: %d\n", errno);
        exit(EXIT_FAILURE);
    }
    printf("Page\tFrame\tTotal Faults\n");
    while (1)
    {
        if (msgrcv(msgid, (void *)&some_data, MAX_TEXT, 0, 0) == -1)
        {
            fprintf(stderr, "msgrcv failed with error: %d\n", errno);
            exit(EXIT_FAILURE);
        }
        sscanf(some_data.some_text, "%d,%d,%d\n", &page, &frame, &faults);
        if (strncmp(some_data.some_text, "-99", 3) == 0)
        {
            printf("\nTerminal ending...\n");
            running = 0;
            break;
        }
        printf("%d\t%d\t%d\n", page, frame, faults);
    }
    if (msgctl(msgid, IPC_RMID, 0) == -1)
    {
        fprintf(stderr, "msgctl failed with error: %d\n", errno);
        exit(EXIT_FAILURE);
    }
    exit(EXIT_SUCCESS);
}
