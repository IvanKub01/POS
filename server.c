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
#include <limits.h>

#define maxSirka 100

#define maxVyska 100


int nacitajPoziadavkuNaCinnost(int newsockfd){
    int vyberS = -1;
    while(vyberS == -1) {
        read(newsockfd, &vyberS, sizeof(int));
    }
    printf("Vyber cinnosti je %d\n", vyberS);
    return vyberS;
}

void posliVybrasSaveKlientovi(int newsockfd, int vyberS){
    printf("idem poslat klinetovi save\n");
    char buffer[maxSirka * maxVyska + 20];
    FILE *ft;
    char ct;
    int help = -1;


    ft=fopen("glad_receive.txt","r");
    if (! ft )
    {
        printf("oops, file can't be read\n");
        exit(-1);
    }


    while((ct=getc(ft))!=EOF)			//Counting No of words in the file
    {

        fscanf(ft , "%s" , buffer);

        //printf("%s\n" , buffer);	//Ignore


        char ci = buffer[0];
        //Ignore
        char ci2 = buffer[1];
        //printf("%c %c\n",ci,ci2);

        if ((strncmp(&ci,"@\n",1) == 0)&&(isspace(ct)||ct=='\t')){

            help++;
            printf("%d\n", help);
        }
        if (help==vyberS) {
            printf("while cyklus\n");
            printf("matica %s\n", buffer);
            write(newsockfd, buffer, 512);
        }
        //printf("%d",strcmp(buffer[0],"@"));
        //printf("%d",strcmp(buffer[1],"@"));

    }

    fclose(ft);
}
int nacitajPoradoveCislo(int newsockfd){
    int vyberS = -1;
    while(vyberS == -1) {

        read(newsockfd, &vyberS, sizeof(int));

    }
    printf("Vyber je %d\n", vyberS);
    return vyberS;
}
void nacitanieUlozH(int newsockfd){

    char buffer[maxSirka * maxVyska + 20];
    FILE *ff;

    int wordss = 0;
    int numberOfSaves = 4;
    char cc;
    printf("Zaciname pocitat slova na Serveri\n");

    ff=fopen("glad_receive.txt","r");
    if (! ff )
    {
        printf("oops, file can't be read\n");
        exit(-1);
    }
    printf("Otvorili sme txt na serveri\n");

    while((cc=getc(ff))!=EOF)			//Counting No of words in the file
    {
        fscanf(ff , "%s" , buffer);


        printf("%s\n" , buffer);	//Ignore

        if((isspace(cc)||cc=='\t')){
            wordss++;
        }
        char ci = buffer[0];
        //Ignore
        char ci2 = buffer[1];
        //printf("%c %c\n",ci,ci2);

        if ((strncmp(&ci,"@\n",1) == 0)&&(isspace(cc)||cc=='\t')){

            numberOfSaves++;

            //printf("y. %s\n",buffer);

        }
        //printf("%d",strcmp(buffer[0],"@"));
        //printf("%d",strcmp(buffer[1],"@"));

    }

    numberOfSaves -= 3;

    printf("Posielam Words = %d \n"  , wordss);

    printf("Posielam numberOfSaves = %d \n"  , numberOfSaves);

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
}

void ukladanie(int newsockfd){
    printf("ukladanie\n");

    char buffer[maxSirka * maxVyska + 20];

    FILE *fp;
    int ch = 0;

    fp = fopen("glad_receive.txt","a");
    int words;
    read(newsockfd, &words, sizeof(int)); // nacita pocet slov v glad.txt aby vedel kolko nacitat
    printf("Klient poslav správu z poctom slov : %d, ktoré sa nachádzali v glad.txt\n" , words);

    while(ch != words) // zapise to do glad_receive.txt
    {
        read(newsockfd , buffer , 512);
        fprintf(fp , " %s\n" , buffer);
        printf(" server %s \n"  , buffer); //Line for Testing , Ignore
        ch++;
    }
    printf("glad.txt sa nakopíroval do glad_receive.txt\n");
    fclose(fp);
}

int goServer(int argc, char *argv[])
{
    int cinnost = 1;
    printf("Som v serveri\n");
    int sockfd, newsockfd;
    socklen_t cli_len;
    struct sockaddr_in serv_addr, cli_addr;

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
    listen(sockfd, INT64_MAX);
    cli_len = sizeof(cli_addr);
    while(cinnost != 2) {
        newsockfd = accept(sockfd, (struct sockaddr*)&cli_addr, &cli_len);
        if (newsockfd < 0)
        {
            perror("ERROR on accept");
            return 3;
        }
        printf("Som dnu\n");      //Ignore , Line for Testing

        cinnost = nacitajPoziadavkuNaCinnost(newsockfd);
        printf("%d\n", cinnost);

        if (cinnost == 0) {
            ukladanie(newsockfd);//nacita nazvy glad.txt a ulozi ho
            printf("Ideme spocitat kolko slov mame na serveri\n");
        } else if (cinnost == 1) {
            nacitanieUlozH(newsockfd); //nacita nazvy ulozenych hier a posle ich klientovi
            printf("kontrol\n");
            int vyberS = nacitajPoradoveCislo(newsockfd);
            posliVybrasSaveKlientovi(newsockfd,vyberS);
        }
        printf("kontrol\n");

        close(newsockfd);
    }
    printf("kontrol\n");

    close(newsockfd);

    close(sockfd);

    return 0;
}