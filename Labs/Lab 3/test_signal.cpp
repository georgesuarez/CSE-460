//test_signal.cpp
#include <signal.h>
#include <unistd.h>
#include <iostream>

using namespace std;

void func(int sig)
{
    cout << "Oops! -- I got a signal " << sig << endl;
}

int main()
{
    struct sigaction act;

    (void)sigaction(SIGINT, &act, NULL); //catch terminal interrupts

    while ( 1 );

    return 0;
}
