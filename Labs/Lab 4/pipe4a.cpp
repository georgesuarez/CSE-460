//pipe4.cpp  (data producer)
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <iostream>

using namespace std;

int main(int argc, char *argv[])
{
    int data_processed;
    int file_pipes[2];
    char buffer[BUFSIZ + 1];
    pid_t fork_result;

    memset(buffer, '\0', sizeof(buffer));

    int index = 0;
    cout << "Input a message to send to pipe5: ";
    while (cin >> buffer[index])
    {
        if (cin.peek() == '\n')
        {
            break;
        }
        else
        {
            buffer[index++];
        }
    }

    if (pipe(file_pipes) == 0)
    { //creates pipe
        fork_result = fork();
        if (fork_result == (pid_t)-1)
        { //fork fails
            fprintf(stderr, "Fork failure");
            exit(EXIT_FAILURE);
        }

        if (fork_result == 0)
        { //child
            sprintf(buffer, "%d", file_pipes[0]);
            (void)execl("pipe5", "pipe5", buffer, (char *)0);
            exit(EXIT_FAILURE);
        }
        else
        { //parent
            data_processed = write(file_pipes[1], buffer,
                                   strlen(buffer));
            printf("%d - wrote %d bytes\n", getpid(), data_processed);
        }
    }
    exit(EXIT_SUCCESS);
}
