#include <iostream>
#include <unistd.h>

using namespace std;

int main()
{
    pid_t pid = fork();

    for (int i = 0; i < 10; ++i)
    {
        if (pid == 0)
        {
            cout << "This is a child with PID: " << getpid() << ". My Parent PID is: " << getppid() << endl;
            pid = fork();
        } else
        {
            wait(0);
        }
    }
    return 0;
}