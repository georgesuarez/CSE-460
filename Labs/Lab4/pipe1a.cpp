//pipe1.cpp
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <iostream>

using namespace std;

int main(int argc, char *argv[])
{
    FILE *fpi; //for reading a pipe

    char buffer[BUFSIZ + 1]; //BUFSIZ defined in <stdio.h>

    for (int i = 1; i < argc; ++i)
    {
        strcat(buffer, argv[i]);
        strcat(buffer, " ");
    }

    int chars_read;
    fpi = popen(buffer, "r");
    if (fpi != NULL)
    {
        //read data from pipe into buffer
        chars_read = fread(buffer, sizeof(char), BUFSIZ, fpi);
        if (chars_read > 0)
            cout << "Output from pipe: " << buffer << endl;
        pclose(fpi); //close the pipe
        return 0;
    }
    memset(buffer, 0, sizeof(buffer)); //clear buffer

    return 1;
}