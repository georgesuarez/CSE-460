#include <iostream>
#include <unistd.h>

using namespace std;

int main()
{
    pid_t parent_pid = getpid();

    cout << "Parent PID: " << parent_pid << endl;

    pid_t pid = fork();

    for (int i = 0; i < 10; i++)
    {
        if (pid > 0) // Parent process
        {
            pid = fork();
            if (pid == 0)
            {
                cout << "I am child process and my PID is: " << getpid() << " and my parent PID is: " << getppid() << endl;
            }
            else
            {
                wait(0);
            }
        }
    }
    return 0;
}