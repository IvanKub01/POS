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
#include <pthread.h>

//kniznice potrebne na chod metody kbhit()
#include <termios.h>
#include <sys/time.h>

//definuje, kolko sekund bude simulacia spat pred vygenerovanim dalsieho kroku
#define spanok 2
//tu sa nastavuju maximalne mozne rozmery sveta
#define maxSirka 100
#define maxVyska 100

typedef struct dataVlaken {
    pthread_mutex_t* mutex;
    int chcemSkoncit;

    int* pocetRiadkov;
    int* pocetStlpcov;
    int* svet;
    int argcM;
    char* par0;
    char* par1;
    char* par2;
    char* par3;
} DATA;

void vyprazdniStdin (void){
    int c = getchar();
    while (c != '\n' && c != EOF)
    c = getchar();
}



void nacitajMaticuUlozNaServeri(int sockfd,int** svet, int* x, int* y){
    int help=0;
    char buffer[maxSirka * maxVyska + 20]; //tych 20 sluzi na dodatocnu reziu komunikacie

    int pocet2 = 3;
    int xx=0;
    int yy=0;
    bzero(buffer,512);
    //printf("toto nam server poslal naspat\n");
    int pocitadlo = 0;
    while(help != pocet2)
    {
        read(sockfd , buffer , 512);
        if (help==1){
            xx=atoi(buffer);
            *x = atoi(buffer);

        }else if (help==2){
            yy=atoi(buffer);
            pocet2=yy*xx;
            *y = atoi(buffer);
            pocet2 += 3;

            //realokovanie pamate pre novy svet - a inicializacia na 0

            int*pomocnySmernik = realloc(*svet, (xx * yy * sizeof(int)));
            if(pomocnySmernik != NULL) {
                //printf("\nNove pole:\n");
                *svet=pomocnySmernik;
                //printf("\n\n");
            } else {
                printf("\nNepodarilo sa rellacovat pamat ,,svet sa nenacita\n");
            }
            for (int i = 0; i < (xx * yy); i++) {
                (*svet)[i] = 0;
            }

        }else if (help != 0){
            int cislo = atoi(buffer);
            (*svet)[pocitadlo] = cislo;
            pocitadlo++;
        }

        help++;
    }
}

int nacitajZklavesnice (int sockfd,int pocetUH){
    //nacita a posle int z klavesnice
    int vyber = 0;
    char ukoncovaciZnak;
    int spravnyVstup = 0;

    while (spravnyVstup == 0) {
        printf("Zadaj poradové čislo Uloženej hry: \n");
        int of = scanf("%d%c", &vyber, &ukoncovaciZnak);
        if(of != 2 || ukoncovaciZnak != '\n') {
            printf("Neplatny vstup - nie je to cislo! %d\n",vyber);
            vyprazdniStdin();

        } else {
            if (vyber > -1 && vyber < pocetUH) {
                spravnyVstup = 1;
            }
            else {
                printf("Neplatny vstup - zaznam s takym cislom neexistuje!\n");
            }
        }
    }




    printf("Nacitam svet cislo %d.\n",vyber);
    int m = write(sockfd, &vyber, sizeof(vyber));
    if (m < 0)
    {
        perror("Error reading from socket");
        return 6;
    }
    return 0;
}

int nacitajUlozHfromS(int sockfd){
    char buffer[maxSirka * maxVyska + 20];
    bzero(buffer,512);
    int pocet;
    int n = read(sockfd, &pocet, sizeof(int));
    if (n < 0)
    {
        perror("Error reading from socket");
        return 6;
    }

    //printf("toto je pocet slov na serveri %d\n",pocet);

    int help=0;
    pocet--;
    bzero(buffer,512);
    printf("toto nam server poslal naspat\n");
    while(help != pocet)
    {
        read(sockfd , buffer , 512);
        printf("%d.  %s\n" ,help ,buffer);
        help++;
    }

    return pocet;
}

void nacitajMaticu(int matica[],int x ,int y,char *text){
    FILE *f = fopen("glad.txt", "w");
    if (f == NULL)
    {
        printf("Error opening file nacitajMaticu!\n");
        exit(1);
    }


    //printf("zapisujem do glad.txt\n");

    fprintf(f, "%s\n", text);
    fprintf(f, "%d\n", x);
    fprintf(f, "%d\n", y);


    for (int j = 0; j < (x*y); ++j) {
        fprintf(f, "%d\n", matica[j]);

    }

    //printf("Zapisané do glad.txt a pripravené na poslanie na server\n");
    fclose(f);
}
void posliServer(int sockfd){
    char buffer[512];
    bzero(buffer,512);

    FILE *f;
    int words = 0;
    char c;
    //printf("Zaciname posielat z clienta n server\n");

    f=fopen("glad.txt","r");
    if (! f ) // overi ci sa da otvorit subor
    {
        printf("oops, file can't be read\n");
        exit(-1);
    }

    while((c=getc(f))!=EOF)			//Spocita pocet slov v glad.txt
    {
        fscanf(f , "%s" , buffer);

        if(isspace(c)||c=='\t')
            words++;
    }
    //printf(" Pocet slov v glad.txt = %d . Posielam tento pocet na server\n"  , words);	//Ignore

    write(sockfd, &words, sizeof(int));
    rewind(f);


    char ch ;
    while(ch != EOF) // posle obsah suboru glad.txt na server
    {
        fscanf(f , "%s" , buffer);

        //printf("klient posiela %s\n" , buffer);

        write(sockfd,buffer,512);
        ch = fgetc(f);
    }
    //printf("Súbor sa poslal uspešne.\n");
    fclose(f);


}

int posliCinnostServeru(int cinnost, int sockfd) {
    printf("Posielam poziadavku na cinnost %d serveru\n",cinnost);
    int m = write(sockfd, &cinnost, sizeof(cinnost));
    if (m < 0)
    {
        perror("Error reading from socket");
        return 6;
    }
    return 0;
}

void vykresliPole(int* pole, int pocetRiadkov, int pocetStlpcov) {
    int pocitadlo = 0;
    for (int i = 0; i < (pocetRiadkov * pocetStlpcov); ++i) {
        if (pocitadlo % pocetStlpcov == (pocetStlpcov - 1)) {
            printf("%d\n", pole[i]);
            pocitadlo++;
        } else {
            printf("%d ", pole[i]);
            pocitadlo++;
        }
    }
    printf("\n");
}

int* KomunikaciaZoServerom(int cinnost, int argc, char* par0, char* par1, char* par2, char* par3, char nazov[512], int *svet, int* x, int* y) {
    int sockfd;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    if (argc < 3) {
        fprintf(stderr, "usage %s hostname port\n", par0);
        return 1;
    }

    server = gethostbyname(par2);
    if (server == NULL) {
        fprintf(stderr, "Error, no such host\n");
        return 2;
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy(
            (char *) server->h_addr,
            (char *) &serv_addr.sin_addr.s_addr,
            server->h_length
    );
    serv_addr.sin_port = htons(atoi(par3));

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Error creating socket");
        return 3;
    }

    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("Error connecting to socket");
        return 4;
    }


    if (cinnost == 0) {
        posliCinnostServeru(cinnost, sockfd);
        nacitajMaticu(svet, *x, *y,nazov);// nacita maticu do glad.txt
        posliServer(sockfd); // posle glad.txt na server
    } else if (cinnost == 1) {
        posliCinnostServeru(cinnost, sockfd);
        int pocetUH = nacitajUlozHfromS(sockfd); // nacita nazvy ulozenych hier
        nacitajZklavesnice(sockfd,pocetUH);
        nacitajMaticuUlozNaServeri(sockfd,&svet,x,y);

    } else if (cinnost == 2) {
        posliCinnostServeru(cinnost, sockfd);
    }
    close(sockfd);

    return svet;
}


void generujNahodne(int* pole, int pocetRiadkov, int pocetStlpcov) {
    for (int i = 0; i < (pocetRiadkov * pocetStlpcov); ++i) {
        int policko = rand() % 2;
        pole[i] = policko;
    }
}

void generujUzivatel(int* pole, int pocetRiadkov, int pocetStlpcov) {
    //suradnice policka, ktore nastavi uzivatel
    int x = 0;
    int y = 0;

    int uzDost = 1;
    int pocetZadanych = 0;

    //nacitania suradnic od pouzivatela
    printf("UPOZORNENIE: Suradnice policok zacinaju od [0, 0] a koncia [pocetRiadkov - 1, pocetStlpcov - 1]\n");
    while (uzDost == 1 && pocetZadanych <= (pocetRiadkov * pocetStlpcov)) {

        //nastavenie x-ovej suradnice ziveho policka
        int vstupUzivatela = 0;
        char ukoncovaciZnak;
        int spravnyVstup = 0;

        while (spravnyVstup == 0) {
            printf("Zadaj x-ovu suradnicu policka: ");
            if(scanf("%d%c", &vstupUzivatela, &ukoncovaciZnak) != 2 || ukoncovaciZnak != '\n') {
                printf("Neplatny vstup - nie je to cislo!\n");
                vyprazdniStdin();
            } else {
                x = vstupUzivatela;
                spravnyVstup = 1;
            }
        }

        //nastavenie y-ovej suradnice policka
        vstupUzivatela = 0;
        ukoncovaciZnak = 's';
        spravnyVstup = 0;

        while (spravnyVstup == 0) {
            printf("Zadaj y-ovu suradnicu policka: ");
            if(scanf("%d%c", &vstupUzivatela, &ukoncovaciZnak) != 2 || ukoncovaciZnak != '\n') {
                printf("Neplatny vstup - nie je to cislo!\n");
                vyprazdniStdin();
            } else {
                y = vstupUzivatela;
                spravnyVstup = 1;
            }
        }

        //kontrola platnosti zadanych suradnic
        if (x < pocetStlpcov && y < pocetRiadkov) {
            pole[(y * pocetStlpcov) + x] = 1;
            printf("Zadanie noveho ziveho policka prebehlo uspesne.\n");
        } else {
            printf("Zadanie noveho ziveho policka neprebehlo uspesne. Zadane suradnice prekracuju velkost sveta.\n");
        }

        printf("Chces zadat nove policko? (1 = Ano / 0 = Nie) ");
        scanf("%d", &uzDost);
    }
}

void krokSimulacie(int* pole, int pocetRiadkov, int pocetStlpcov) {
    int pomocnePole[pocetRiadkov * pocetStlpcov];
    for (int i = 0; i < pocetRiadkov * pocetStlpcov; ++i) {
        pomocnePole[i] = pole[i];
    }

    //priprava dvojrozmernych poli
    int pole2D[pocetRiadkov + 2][pocetStlpcov + 2];
    int pomocnePole2D[pocetRiadkov + 2][pocetStlpcov + 2];

    //nastavenie vsetkych prvkov dvojrozmernych poli na 0
    for(int i = 0; i <= pocetRiadkov + 1; i++) {
        for (int j = 0; j <= pocetStlpcov + 1; j++) {
            pole2D[i][j] = 0;
            pomocnePole2D[i][j] = 0;
        }
    }

    //konverzia jednorozmerneho pola na dvojrozmerne
    for (int i = 0; i < pocetRiadkov * pocetStlpcov; ++i) {
        int riadok = i / pocetStlpcov;
        int stlpec = i % pocetStlpcov;
        pole2D[riadok + 1][stlpec + 1] = pole[i];
        pomocnePole2D[riadok + 1][stlpec + 1] = pole[i];
    }

    //priebeh jedneho kroku simulacie
    for(int i = 1; i <= pocetRiadkov; i++) {
        for(int j = 1; j <= pocetStlpcov; j++) {
            int pocetSusedov = 0;
            for(int k = -1; k < 2; k++) {
                for(int l = -1; l < 2; l++) {
                    if(pole2D[i + k][j + l] == 1) {
                        pocetSusedov++;
                    }
                }
            }
            if(pole2D[i][j] == 1) {
                pocetSusedov--;
            }
            if(pole2D[i][j] == 0 && pocetSusedov == 3) {
                pomocnePole2D[i][j] = 1;
            } else if(pole2D[i][j] == 0 && pocetSusedov < 3) {
                pomocnePole2D[i][j] = 0;
            } else if(pole2D[i][j] == 0 && pocetSusedov > 3) {
                pomocnePole2D[i][j] = 0;
            } else if(pole2D[i][j] == 1 && pocetSusedov < 2) {
                pomocnePole2D[i][j] = 0;
            } else if(pole2D[i][j] == 1 && pocetSusedov == 2) {
                pomocnePole2D[i][j] = 1;
            } else if(pole2D[i][j] == 1 && pocetSusedov == 3) {
                pomocnePole2D[i][j] = 1;
            } else if(pole2D[i][j] == 1 && pocetSusedov > 3) {
                pomocnePole2D[i][j] = 0;
            }
        }
    }

    //konverzia dvojrozmerneho pola naspat na jednorozmerne
    int pocitadlo = 0;
    for(int i = 1; i <= pocetRiadkov; i++) {
        for (int j = 1; j <= pocetStlpcov; j++) {
            pole[pocitadlo] = pomocnePole2D[i][j];
            pocitadlo++;
        }
    }
}

//metoda pre vlakno simlacie
static void* simulacia(void* args) {
    DATA* data = (DATA*)args;
    int koniec = 0;
    while (koniec == 0) {
        pthread_mutex_lock(data->mutex);

        printf("Generujem dalsi krok simulacie...\n");
        krokSimulacie(data->svet, *data->pocetRiadkov, *data->pocetStlpcov);
        vykresliPole(data->svet, *data->pocetRiadkov, *data->pocetStlpcov);
        koniec = data->chcemSkoncit;

        pthread_mutex_unlock(data->mutex);

        //po vygenerovani sa vlakno na urcity cas uspi - hodnota v podstate udava rychlost simulacie - hodnotu "spanok" nastavuj hore v define
        usleep((spanok * 1000000));

    }
    free(data->svet);

    return NULL;
}

//metody na nahradenie metody khbit() jazyka C, ktora vo Windowse sluzi na zistovanie toho, ci bola stlacena nejaka klavesa - na Linuxe nefunguje
//zdroj: https://cboard.cprogramming.com/c-programming/63166-kbhit-linux.html
int kbhit (void)
{
    struct timeval tv;
    fd_set rdfs;

    tv.tv_sec = 0;
    tv.tv_usec = 0;

    FD_ZERO(&rdfs);
    FD_SET (STDIN_FILENO, &rdfs);

    select(STDIN_FILENO+1, &rdfs, NULL, NULL, &tv);
    return FD_ISSET(STDIN_FILENO, &rdfs);

}
void changemode(int dir)
{
    static struct termios oldt, newt;

    if ( dir == 1 )
    {
        tcgetattr( STDIN_FILENO, &oldt);
        newt = oldt;
        newt.c_lflag &= ~( ICANON | ECHO );
        tcsetattr( STDIN_FILENO, TCSANOW, &newt);
    }
    else
        tcsetattr( STDIN_FILENO, TCSANOW, &oldt);
}
//tu konci prevzaty kod




//metoda pre pouzivatelske vlakno
static void* obsluha(void* args) {
    DATA* data = (DATA*)args;
    int koniec = 0;
    while (koniec == 0) {
        changemode(1);
        int pocitadlo = 0;
        while(pocitadlo < 200) {
            if (kbhit() == 1) {
                vyprazdniStdin();
                pthread_mutex_lock(data->mutex);
                pocitadlo = 300;
                printf("Stlacena klavesa, simulacia pozastavena.\n");
                printf("Pre ulozenie aktualneho sveta - stlac 0\n");
                printf("Pre nacitanie sveta zo servera - stlac 1\n");
                printf("Pre ukoncenie simulacie - stlac 2\n");
                printf("Pre pokracovanie simulacie - stlac 3\n");

                //kontrola vstupu - ci uzivatel zadal cislo a ci je platne
                int vstupUzivatela = 0;
                char ukoncovaciZnak;
                int spravnyVstup = 0;

                //maximalna dlzka nazvu suboru je 512
                char nazovUlozenejHry[512];
                //pomocne pole pre nacitanie nazvu suboru
                char pomocnePole[512];

                while (spravnyVstup == 0) {
                    printf("Zadaj cislo: \n");
                    if(scanf("%d%c", &vstupUzivatela, &ukoncovaciZnak) != 2 || ukoncovaciZnak != '\n') {
                        printf("Neplatny vstup - nie je to cislo!\n");
                        vyprazdniStdin();
                    } else if (vstupUzivatela == 0 || vstupUzivatela == 1 || vstupUzivatela == 2 || vstupUzivatela == 3) {
                        spravnyVstup = 1;
                    } else {
                        printf("Neplatny vstup - taka volba neexistuje!\n");
                    }
                }

                if (vstupUzivatela == 0) {
                    //metoda na ulozenie na server
                    
                    //nacitanie nazvu suboru
                    printf("Zadaj nazov sveta: ");
                    scanf("%s", pomocnePole);
                    //pripojenie znaku zavinaca pred nazov sveta - sluzi ako delimitujuci znak
                    snprintf(nazovUlozenejHry, sizeof nazovUlozenejHry, "@%s", pomocnePole);

                    KomunikaciaZoServerom(vstupUzivatela,data->argcM,data->par0,data->par1,data->par2,data->par3,nazovUlozenejHry,data->svet,data->pocetStlpcov ,data->pocetRiadkov);
                    printf("Ulozenie aktualneho sveta na server prebehlo uspesne\n\n");
                } else if (vstupUzivatela == 1) {
                    //metoda na nacitanie zo servera
                    data->svet = KomunikaciaZoServerom(vstupUzivatela,data->argcM,data->par0,data->par1,data->par2,data->par3,nazovUlozenejHry,data->svet,data->pocetStlpcov ,data->pocetRiadkov);
                    printf("Nacitanie sveta zo servera prebehlo uspesne.\n");
                    printf("Simulacia pokracuje s nacitanym svetom, ktory vypada nasledovne:\n\n");
                    vykresliPole(data->svet, *data->pocetRiadkov, *data->pocetStlpcov);
                } else if (vstupUzivatela == 2) {
                    printf("Ukoncenie simulacie prebehlo uspesne\n\n");
                    KomunikaciaZoServerom(vstupUzivatela,data->argcM,data->par0,data->par1,data->par2,data->par3,nazovUlozenejHry,data->svet,data->pocetStlpcov ,data->pocetRiadkov);
                    koniec = 1;


                    data->chcemSkoncit = 1;

                } else if (vstupUzivatela == 3) {
                    //neurob nic - tzn. simulacia sa odpauzuje
                }
            } else {
                pocitadlo++;
            }
        }
        pthread_mutex_unlock(data->mutex);
        changemode(0);
    }
    return NULL;
}


_Noreturn int goKlient(int argc, char *argv[])
{

    //priradenie parametrov programu do premennych
    char* par0 = argv[0];
    char* par1 = argv[1];
    char* par2 = argv[2];
    char* par3 = argv[3];


    //rozmery sveta
    int pocetRiadkov = 0;
    int pocetStlpcov = 0;

    printf("-----------------------------------\n");
    printf("Nastavenie velkosti sveta:\n");
    //premenne sluziace na kontrolu vstupu
    int vstupUzivatela = 0;
    char ukoncovaciZnak;
    int spravnyVstup = 0;

    //nastavenie sirky sveta
    while (spravnyVstup == 0) {
        printf("Zadaj sirku sveta: ");
        if(scanf("%d%c", &vstupUzivatela, &ukoncovaciZnak) != 2 || ukoncovaciZnak != '\n') {
            printf("Neplatny vstup - nie je to cislo!\n");
            vyprazdniStdin();
        } else if (vstupUzivatela <= maxSirka) {
            pocetStlpcov = vstupUzivatela;
            spravnyVstup = 1;
        } else {
            printf("Neplatny vstup - rozmer prekracuje maximalnu povolenu sirku!\n");
        }
    }

    vstupUzivatela = 0;
    ukoncovaciZnak = 's';
    spravnyVstup = 0;

    //nastavenie vysky sveta
    while (spravnyVstup == 0) {
        printf("Zadaj vysku sveta: ");
        if(scanf("%d%c", &vstupUzivatela, &ukoncovaciZnak) != 2 || ukoncovaciZnak != '\n') {
            printf("Neplatny vstup - nie je to cislo!\n");
            vyprazdniStdin();
        } else if (vstupUzivatela <= maxVyska) {
            pocetRiadkov = vstupUzivatela;
            spravnyVstup = 1;
        } else {
            printf("Neplatny vstup - rozmer prekracuje maximalnu povolenu vysku!\n");
        }
    }

    printf("\n");

    //vytvorenie zaciatocneho sveta a nastavenie vsetkych prvkov na 0
    int* svet = calloc((pocetRiadkov * pocetStlpcov), sizeof(int));

    //rucne alebo nahodne vygenerovany svet
    printf("Generovanie sveta:\n");
    printf("Pre nahodne vygenerovany svet - stlac 0\n");
    printf("Pre manualne nastavenie zivych policok - stlac 1\n");
    vstupUzivatela = 0;
    ukoncovaciZnak = 's';
    spravnyVstup = 0;
    while (spravnyVstup == 0) {
        printf("Zadaj cislo: ");
        if(scanf("%d%c", &vstupUzivatela, &ukoncovaciZnak) != 2 || ukoncovaciZnak != '\n') {
            printf("Neplatny vstup - nie je to cislo!\n");
            vyprazdniStdin();
        } else if (vstupUzivatela == 0 || vstupUzivatela == 1) {
            spravnyVstup = 1;
        } else {
            printf("Neplatny vstup - taka volba neexistuje!\n");
        }
    }
    if (vstupUzivatela == 1) {
        printf("Spustam manualne zadavanie policok:\n\n");
        generujUzivatel(svet, pocetRiadkov, pocetStlpcov);
    } else if (vstupUzivatela == 0) {
        generujNahodne(svet, pocetRiadkov, pocetStlpcov);
        printf("Nahodne generovanie prebehlo uspesne.\n\n");
    }

    //vykreslenie zaciatocneho sveta
    printf("\n");
    printf("Pociatocny stav sveta:\n");
    vykresliPole(svet, pocetRiadkov, pocetStlpcov);

    //volba simulacie od n-teho kroku
    printf("\n");
    printf("Start simulacie od zadaneho kroku:\n");
    //kontrola vstupu
    vstupUzivatela = 0;
    ukoncovaciZnak = 's';
    spravnyVstup = 0;
    while (spravnyVstup == 0) {
        printf("Zadaj cislo kroku, od ktoreho chces zacat simulaciu\n");
        printf("UPOZORNENIE - Pociatocny stav = 0.krok : \n");
        if(scanf("%d%c", &vstupUzivatela, &ukoncovaciZnak) != 2 || ukoncovaciZnak != '\n') {
            printf("Neplatny vstup - nie je to cislo!\n");
            vyprazdniStdin();
        } else {
            spravnyVstup = 1;
        }
    }
    for (int i = 1; i <= vstupUzivatela; ++i) {
        krokSimulacie(svet, pocetRiadkov, pocetStlpcov);
    }
    printf("Zacinam simulaciu od kroku %d:\n", vstupUzivatela);
    vykresliPole(svet, pocetRiadkov, pocetStlpcov);

    //mutex a data
    pthread_mutex_t mutex;
    pthread_mutex_init(&mutex, NULL);

    DATA data = {&mutex, 0, &pocetRiadkov, &pocetStlpcov, svet,argc,par0,par1,par2,par3};

    pthread_t simulator;
    pthread_create(&simulator, NULL, simulacia, &data);

    pthread_t uzivatel;
    pthread_create(&uzivatel, NULL, obsluha, &data);

    pthread_join(simulator, NULL);
    pthread_join(uzivatel, NULL);

    pthread_mutex_destroy(&mutex);

    return 1;

}