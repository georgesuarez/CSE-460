#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <iostream>

#include <SDL2/SDL_thread.h>
#include <vector>

int matrixA[3][2] = {{2, 3}, {4, 8}, {6, 11}};
int matrixB[2][3] = {{3, 12, 0}, {4, 10, 12}};
int matrixC[3][3] = {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}};
using namespace std;
class matrix
{
  public:
    void printA(int m[][2]);
    void printB(int m[][3]);
    void printC(int m[][3]);

  private:
    int row;
    int col;
    int product;
};

int dotProduct(void *data)
{
    int row;
    int col;
    int product;
    char *threadname;
    threadname = (char *)data;

    cout << "This is " << threadname << endl;
    ;

    for (row = 0; row < 3; row++)
    {
        for (col = 0; col < 3; col++)
        {
            for (product = 0; product < 2; product++)
            {
                matrixC[row][col] += matrixA[row][product] *
                                     matrixB[product][col];
            }
        }
    }

    return 0;
}

void matrix::printA(int m[][2])
{
    cout << "Matrix A: " << endl;
    for (row = 0; row < 3; row++)
    {
        for (col = 0; col < 2; col++)
            cout << matrixA[row][col] << " ";
        cout << endl;
    }
    cout << endl;
}
void matrix::printB(int m[][3])
{
    cout << "Matrix B: " << endl;
    for (row = 0; row < 2; row++)
    {
        for (col = 0; col < 3; col++)
            cout << matrixB[row][col] << " ";
        cout << endl;
    }

    cout << endl;
}
void matrix::printC(int m[][3])
{
    cout << "Matrix C: " << endl;
    for (row = 0; row < 3; row++)
    {
        for (col = 0; col < 3; col++)
        {
            cout << matrixC[row][col] << " ";
        }
        cout << endl;
    }
}

int main()
{

    matrix matrices;

    SDL_Thread *sumThread;

    sumThread = SDL_CreateThread(dotProduct, "Sum Thread", (void *)"Dot Product Thread");

    if (sumThread == NULL)
    {
        cout << "\nSDL_CreateThread failed: " << SDL_GetError() << endl;
    }
    else
    {
        // Wait for the thread to complete.
        int returnValue;
        SDL_WaitThread(sumThread, &returnValue);
        cout << "Dot product of matrices A and B: " << endl;
        matrices.printA(matrixA);
        matrices.printB(matrixB);
        cout << "Equal matrix C: " << endl;
        matrices.printC(matrixC);
        cout << endl;
    }
    return 0;
}