#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include<ctype.h>
#include "klient.h"
#include "server.h"



int main(int argc, char *argv[])
{
    printf("Som v main\n");
    printf("%s m\n",argv[1]);
    printf("%s m\n",argv[2]);

    if (strcmp(argv[1],"server") == 0) {
        printf("%s s\n",argv[1]);

        goServer(argc,argv);
    } else {
        printf("%s k\n",argv[1]);
        goKlient(argc,argv);

    }

    return 0;
}