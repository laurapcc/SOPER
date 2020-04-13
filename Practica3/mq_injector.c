#include <fcntl.h>
#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#define DOSKB 2048

int main(int argc, char *argv[]) {
    FILE* f=NULL;
    char aux[DOSKB];
    char* file;
    char* mq_name;

    if (argc!=3){
        fprintf(stdout, "Error en el numero de argumentos\n");
        return EXIT_FAILURE;
    }
    file = argv[1];
    mq_name = argv[2];

    struct mq_attr attributes = {
        .mq_flags = 0,
        .mq_maxmsg = 10,
        .mq_curmsgs = 0,
        .mq_msgsize = DOSKB*sizeof(char)
    };

    mqd_t queue = mq_open(mq_name,
        O_CREAT | O_WRONLY, /* This process is only going to send messages */
        S_IRUSR | S_IWUSR, /* The user can read and write */
        &attributes);

    if (queue == (mqd_t)-1) {
        fprintf(stderr, "Error opening the queue\n");
        return EXIT_FAILURE;
    }

    f = fopen(file, "r");
    if (f==NULL){
        fprintf(stdout, "Error abriendo el archivo\n");
        mq_close(queue);
        return EXIT_FAILURE;
    }

    while (fgets(aux, DOSKB, f)!=NULL){
        if (mq_send(queue, aux, sizeof(aux), 1) == -1) {
            fprintf(stderr, "Error sending message\n");
            mq_close(queue);
            return EXIT_FAILURE;
        }
    }

    char final[10]="Final\0";
    if (mq_send(queue, final, sizeof(final), 1) == -1) { //estaba sizeof(aux)
        fprintf(stderr, "Error sending message\n");
        return EXIT_FAILURE;
    }
       
    /* Wait for input to end the program */
    fprintf(stdout, "Press any key to finish\n");
    getchar();

    mq_close(queue);
    mq_unlink(mq_name);

    return EXIT_SUCCESS;
}
