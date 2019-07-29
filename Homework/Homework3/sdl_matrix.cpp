#include <SDL2/SDL.h>
#include <SDL2/SDL_thread.h>
#include <vector>
#include <ctime>
#include <iostream>

using namespace std;

int matrixA[3][2] = {{5, 4}, {2,6}, {9, 2}};
int matrixB[2][3] = {{5,6,2}, {4, 2, 8}};
int matrixC[3][3] = {{0, 0, 0},{0, 0, 0},{0, 0, 0}};

int dotProduct(void *data)
{
    char *threadname = (char *)data;
    

    for (int row = 0; row < 3; row++)
    {
        for (int col = 0; col < 3; col++)
        {
            for (int product = 0; product < 2; product++)
            {
                matrixC[row][col] += matrixA[row][product] * matrixB[product][col];
            }
        }
    }
        return 0;
}

void printMatrixA(int matrix[][2])
{
    cout << "Matrix A: " << endl;
    for (int row = 0; row < 3; row++)
    {
        for (int col = 0; col < 2; col++)
        {
            cout << matrixA[row][col] << " ";
        }
        cout << endl;
    }

    cout << endl;
}

void printMatrixB(int m[][3])
{
    cout << "Matrix B: " << endl;
    for (int row = 0; row < 2; row++)
    {
        for (int col = 0; col < 3; col++)
        {
            cout << matrixB[row][col] << " ";
        }
        cout << endl;
    }

    cout << endl;
}

void printMatrixC(int m[][3])
{
    cout << "Matrix C: " << endl;
    for (int row = 0; row < 3; row++)
    {
        for (int col = 0; col < 3; col++)
        {
            cout << matrixC[row][col] << " ";
        }
        cout << endl;
    }
    cout << endl;
}

int main()
{
    SDL_Thread *sumThread = SDL_CreateThread(dotProduct, "Sum Thread", (void *)"Dot Product Thread");

    if (sumThread == NULL)
    {
        cout << "SDL_CreateThread failed: \n" << SDL_GetError() << endl;
    }
    else
    {
        int returnValue;
        SDL_WaitThread(sumThread, &returnValue);
        printMatrixA(matrixA);
        printMatrixB(matrixB);

        cout << "Equals to ";
        printMatrixC(matrixC);
        cout << endl;
    }
    return 0;
}