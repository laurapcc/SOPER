#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(void){
  int time;

  while (time<10000000){
    time=clock();
  }

	return EXIT_SUCCESS;
}
