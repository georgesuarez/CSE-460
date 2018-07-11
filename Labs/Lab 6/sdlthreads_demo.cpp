#include <SDL2/SDL.h>
#include <SDL2/SDL_thread.h>
#include <stdio.h>

using namespace std;

//The thread
int runner ( void *data )
{
  char *tname = ( char * )data;

  printf("I am %s\n", tname );
  return 0;
}

int runner2 ( void *data )
{
  char *tname = ( char * )data;
  printf("Hello CSE 460! This is %s\n", tname );
  return 0;
}

int runner3 ( void *data )
{
  char *tname = ( char * )data;
  int result = 2 + 2;
  printf("%s is executing 2 + 2 which is %d\n", tname, result);
  return result;
}

int main ()
{
  SDL_Thread *id1, *id2, *id3;                //thread identifiers
  //names of threads
  char *tnames[3] = { (char *) "Thread 1", (char *) "Thread 2", (char *) "Thread 3" };


  //create the threads
  id1 = SDL_CreateThread ( runner, "TestThread1", tnames[0] );
  id2 = SDL_CreateThread ( runner2, "TestThread2", tnames[1] );
  id3 = SDL_CreateThread ( runner3, "TestThread3" , tnames[2] ) ;

  //wait for the threads to exit
  SDL_WaitThread ( id1, NULL );
  SDL_WaitThread ( id2, NULL );
  SDL_WaitThread ( id3, NULL );

  return 0;
}