//
// Created by Ivan on 1/6/2021.
//

#include "server.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include<ctype.h>
#include <string.h>


void ukladanie(int newsockfd){
    char buffer[512];

    FILE *fp;
    int ch = 0;

    fp = fopen("glad_receive.txt","a");
    int words;
    read(newsockfd, &words, sizeof(int));
    printf("Passed integer is : %d\n" , words);      //Ignore , Line for Testing

    while(ch != words)
    {
        read(newsockfd , buffer , 512);
        fprintf(fp , " %s\n" , buffer);
        //printf(" %s %d \n"  , buffer , ch); //Line for Testing , Ignore
        ch++;
    }
    fclose(fp);
}
int goServer(int argc, char *argv[])
{
    printf("Som v serveri\n");
    int sockfd, newsockfd;
    socklen_t cli_len;
    struct sockaddr_in serv_addr, cli_addr;
    int n;
    char buffer[512];

    if (argc < 2)
    {
        fprintf(stderr,"usage %s port\n", argv[0]);
        return 1;
    }

    bzero((char*)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(atoi(argv[2]));

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        perror("Error creating socket");
        return 1;
    }

    if (bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("Error binding socket address");
        return 2;
    }

    listen(sockfd, 5);
    cli_len = sizeof(cli_addr);

    newsockfd = accept(sockfd, (struct sockaddr*)&cli_addr, &cli_len);
    if (newsockfd < 0)
    {
        perror("ERROR on accept");
        return 3;
    }



    printf("Som dnu\n");      //Ignore , Line for Testing


    ukladanie(newsockfd);
    printf("Ideme spocitat kolko slov mame na serveri\n");



    /*printf("Here is the message: %s\n", buffer);

    const char* msg = "I got your message";
    n = write(newsockfd, msg, strlen(msg)+1);


    if (n < 0)
    {
        perror("Error writing to socket");
        return 5;
    }*/
    // ulpoad from server to client///////////////////////////////////////
    FILE *ff;

    int wordss = 0;
    int numberOfSaves = 4;
    char cc;
    printf("Zaciname pocitat slova na Serveri\n");

    ff=fopen("glad_receive.txt","r");
    if (! ff ) // equivalent to saying if ( in_file == NULL )
    {
        printf("oops, file can't be read\n");
        exit(-1);
    }
    printf("Otvorili sme txt na serveri\n");

    while((cc=getc(ff))!=EOF)			//Counting No of words in the file
    {
        fscanf(ff , "%s" , buffer);

        //printf("%s\n" , buffer);	//Ignore

        if((isspace(cc)||cc=='\t'))
            wordss++;
        char ci = buffer[0];
        	//Ignore
        char ci2 = buffer[1];
        //printf("%c %c\n",ci,ci2);
        if (strncmp(&ci,"@",1) == 0){
            numberOfSaves++;
            //printf("y. %s\n",buffer);

        }
        //printf("%d",strcmp(buffer[0],"@"));
        //printf("%d",strcmp(buffer[1],"@"));

    }
    printf("Posielam Words = %d \n"  , wordss);	//Ignore
    write(newsockfd,&numberOfSaves,sizeof(int));

    printf("Idem posielat zoznam ulozenych hier zo txt Servera num \n");

    rewind(ff);
    ff=fopen("glad_receive.txt","r");
    if (! ff ) // equivalent to saying if ( in_file == NULL )
    {
        printf("oops, file can't be read\n");
        exit(-1);
    }
    //char chh ;
    // bzero(buffer,512);
    int ii = 0;
    //while((cc=getc(fp))!=EOF)
    while (ii != wordss)
    {
        fscanf(ff , "%s" , buffer);
        char cj = buffer[0];

        if (strncmp(&cj,"@",1) == 0) {
            write(newsockfd, buffer, 512);
        }


        //printf(".. %c\n" , buffer[0]);	//Ignore

        //bzero(buffer,512);
        //chh = fgetc(fp);
        ii++;
    }
    printf("poslane zo servera\n");
    fclose(ff);





    close(newsockfd);
    close(sockfd);

    return 0;
}