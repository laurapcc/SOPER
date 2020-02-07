#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

extern int errno;

int main(void){
	FILE* f=NULL;

  f=fopen("/etc/shadow", "r");
  if (f==NULL){
    fprintf(stderr, "%d: %s\n", errno, strerror(errno));
    return EXIT_FAILURE;
  }

  fclose(f);
	return EXIT_SUCCESS;
}
