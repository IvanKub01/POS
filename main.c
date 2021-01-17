#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include<ctype.h>



int main(int argc, char *argv[])
{
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
    serv_addr.sin_port = htons(atoi(argv[1]));

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




    FILE *fp;
    int ch = -1;

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
    rewind(fp);
    int wordss = 0;
    char cc;
    printf("Zaciname pocitat slova na Serveri\n");

    fp=fopen("glad_receive.txt","r");
    if (! fp ) // equivalent to saying if ( in_file == NULL )
    {
        printf("oops, file can't be read\n");
        exit(-1);
    }
    printf("Otvorili sme txt na serveri\n");

    while((cc=getc(fp))!=EOF)			//Counting No of words in the file
    {
        fscanf(fp , "%s" , buffer);

        //printf("%s\n" , buffer);	//Ignore

        if(isspace(cc)||cc=='\t')
            wordss++;
    }
    printf("Posielam Words = %d \n"  , wordss);	//Ignore
    write(newsockfd,&wordss,sizeof(int));

    printf("Idem posielat slova zo txt Servera\n");

    rewind(fp);
    fp=fopen("glad_receive.txt","r");
    if (! fp ) // equivalent to saying if ( in_file == NULL )
    {
        printf("oops, file can't be read\n");
        exit(-1);
    }
    //char chh ;
    // bzero(buffer,512);
    int ii = 0;
    //while((cc=getc(fp))!=EOF)
    while (ii != 10)
    {
        fscanf(fp , "%s" , buffer);
        printf("%s\n" , buffer);	//Ignore
        write(sockfd,buffer,512);
        //chh = fgetc(fp);
        ii++;
    }
    printf("poslalo sa zo servera\n");
    fclose(fp);





    close(newsockfd);
    close(sockfd);

    return 0;
}