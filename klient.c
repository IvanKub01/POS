//
// Created by Ivan on 1/6/2021.
//

#include "klient.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include<ctype.h>
int nacitajUlozHfromS(int sockfd){
    char buffer[512];
    bzero(buffer,512);
    int pocet;
    int n = read(sockfd, &pocet, sizeof(int));
    if (n < 0)
    {
        perror("Error reading from socket");
        return 6;
    }

    printf("toto je pocet slov na serveri %d\n",pocet);

    int help=0;
    bzero(buffer,512);
    printf("toto nam server poslal naspat\n");
    while(help != pocet)
    {
        read(sockfd , buffer , 512);
        printf("%d.  %s\n" ,help ,buffer);
        //printf(" %s %d \n"  , buffer , ch); //Line for Testing , Ignore
        help++;
    }
    return 0;
}
void nacitajMaticu(int matica[],int x ,int y){
    FILE *f = fopen("glad.txt", "w");
    if (f == NULL)
    {
        printf("Error opening file nacitajMaticu!\n");
        exit(1);
    }

/*    printf("nacitaj meno\n");
    char text[10000];
    scanf("%s",&text);
    printf("%s\n",text);*/
    const char *text = "@Nazov7";
    fprintf(f, "%s\n", text);
    fprintf(f, "%d\n", x);
    fprintf(f, "%d\n", y);


    for (int j = 0; j < (x*y); ++j) {
        fprintf(f, "%d\n", matica[j]);

    }


    fclose(f);
}
void posliServer(int sockfd){
    char buffer[512];
    bzero(buffer,512);

    FILE *f;
    int words = 0;
    char c;
    printf("Zaciname posielat z clienta n server\n");

    f=fopen("glad.txt","r");
    if (! f ) // equivalent to saying if ( in_file == NULL )
    {
        printf("oops, file can't be read\n");
        exit(-1);
    }
    //printf("Hello, World2\n");

    while((c=getc(f))!=EOF)			//Counting No of words in the file
    {
        fscanf(f , "%s" , buffer);

        if(isspace(c)||c=='\t')
            words++;
    }
    printf(" posielame Words = %d \n"  , words);	//Ignore

    write(sockfd, &words, sizeof(int));
    rewind(f);


    char ch ;
    while(ch != EOF)
    {
        fscanf(f , "%s" , buffer);
        printf("%s\n" , buffer);	//Ignore
        write(sockfd,buffer,512);
        ch = fgetc(f);
    }
    printf("The file was sent successfully\n");
    fclose(f);


}
int goKlient(int argc, char *argv[])
{

    printf("Som v klientovi\n");

    int sockfd, n;
    struct sockaddr_in serv_addr;
    struct hostent* server;

    //char buffer[256];

    if (argc < 3)
    {
        fprintf(stderr,"usage %s hostname port\n", argv[0]);
        return 1;
    }

    server = gethostbyname(argv[2]);
    if (server == NULL)
    {
        fprintf(stderr, "Error, no such host\n");
        return 2;
    }

    bzero((char*)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy(
            (char*)server->h_addr,
            (char*)&serv_addr.sin_addr.s_addr,
            server->h_length
    );
    serv_addr.sin_port = htons(atoi(argv[3]));

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        perror("Error creating socket");
        return 3;
    }

    if(connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("Error connecting to socket");
        return 4;
    }
    int x = 3;
    int y = 2;
    int matica[x*y];
    for (int i = 0; i < (x*y); ++i) {
        matica[i] = rand() %2;
    }
    nacitajMaticu(matica,x,y);
    posliServer(sockfd);
    nacitajUlozHfromS(sockfd);

    //pocet slov na serveri

    close(sockfd);
    printf("zadaj cislo\n");
    int cislo=0;
    scanf("%d",&cislo);
    printf("%d\n",cislo);
    return 0;
}