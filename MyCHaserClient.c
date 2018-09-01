/*
 * Almajiro's CHaserOnline Client
 *
 * Author: Kuroki Almajiro
 * Date:   2018/6/28
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/param.h>
#include <sys/uio.h>
#include <unistd.h>

#define BUF_LEN 512
#define MAX_KEYWORD 30
#define VERSION "C204"
#define AUTHOR "Kuroki Almajiro"
#define LINK "http://www.almajiro.tokyo"

struct sockaddr_in server;
char host[BUF_LEN] = "localhost";
char path[BUF_LEN] = "/";
unsigned short port = 0;

int soloTargets[] = {5, 6, 7, 8, 9, 1, 3, 20, 21, 22, 23, 30, 31, 32, 33, 60, 61, 62, 63, 0, 2};
int multiTargets[] = {5, 6, 7, 8, 9, 20, 21, 22, 23, 30, 31, 32, 33, 60, 61, 62, 63, 1, 3, 2, 0};
int targets[22] = {5, 6, 7, 8, 9, 1, 20, 21, 22, 23, 30, 31, 32, 33, 60, 61, 62, 63, 3, 0, 2};

int returnNumberX[10];
int self = 1000;
int maxItemSum = 5;
int previousMode = 0;
int previousCount = 0;
int previousItem = 0;
int thisActionFlag = 0;
int readyFlag = 0;
int breaktime = 0;
int breaktimeLimit = 1;
int lastOutside = 0;

int totalAttack = 0;
int totalBreak = 0;

int way[9] = {1, 3, 5, 7, 0, 2, 6, 8};
char characters[] = {'C', 'H', 'a', 's', 'e', 'r', 'O', 'n'};

void parseReturnNumber(int count, int* returnNumber);

int calcItem(int item);

int send_cmd(char *command, char *param, char *returnCode);

int returnCode2int(char *returnCode, int *returnNumberX);

int Init(int argc, char **argv, char *ProxyAddress, int ProxyPort);

int main(int argc, char *argv[]) {
    int i, j, k;
    int RoomNumber = -1;
    char command[20];
    char param[BUF_LEN];
    char buff[10];
    char ProxyAddress[256];
    int ProxyPort;
    char UserName[20];
    char PassWord[20];
    char ReturnCode[BUF_LEN];
    char *pivo;
    char type[100];

    int turn = 1;
    int count = 9;
    int ActionCount = 9;
    int CountBuff = 9;
    int mode = 0;
    int nextMode = 0;
    int GetReadyMode = 0;
    int maxTarget;
    int itemsSum = 0;

    int returnNumber[10];
    int ActionReturnNumber[10];

    int selfFlag = 0;
    int alert = 0;
    int alertMode = 0;

    strcpy(ProxyAddress, "");
    ActionReturnNumber[0] = -10000;

    system("reset");
    printf("\x1b[32m");
    printf("Almajiro's CHaserOnline Client V.%s coded by %s\n", VERSION, AUTHOR);
    printf("LINK: %s\n", LINK);
    printf("Compiled at %s (%s)\n" , __DATE__, __TIME__);
    system("uname -v");
    usleep(500000);
    printf("\x1b[31m");
    puts("");
    puts("                                     ,--.");
    puts("                                    {    }");
    puts("                                    K,   }");
    puts("                                   /  `Y`");
    puts("                              _   /   /");
    puts("                             {_'-K.__/");
    puts("                               `/-.__L._");
    puts("                               /  ' /`\\_}");
    puts("                              /  ' /     ");
    puts("                      ____   /  ' /");
    puts("               ,-'~~~~    ~~/  ' /_");
    puts("             ,'             ``~~~%%',");
    puts("            (                     %  Y");
    puts("           {                      %% I");
    puts("          {      -                 %  `.");
    puts("          |       ',                %  )");
    puts("          |        |   ,..__      __. Y");
    puts("          |    .,_./  Y ' / ^Y   J   )|");
    puts("          \\           |' /   |   |   ||");
    puts("           \\          L_/    . _ (_,.'(");
    puts("            \\,   ,      ^^\"\"' / |      )");
    puts("              \\_  \\          /,L]     /");
    puts("                '-_`-,       ` `   ./`");
    puts("                   `-(_            )");
    puts("                       ^^\\..___,.--`");
    puts("");
    sleep(2);

    printf("\x1b[33m");

    i = 2;
    while (argv[i] != NULL) {
        if (argv[i][0] == '-') {
            switch (argv[i][1]) {
                case 'x':
                    i++;
                    pivo = strchr(argv[i], ':');
                    *pivo = '\0';
                    strcpy(ProxyAddress, argv[i]);
                    ProxyPort = atoi((char *) (++pivo));
                    break;

                case 'u':
                    i++;
                    strcpy(UserName, argv[i]);
                    break;

                case 'p':
                    i++;
                    strcpy(PassWord, argv[i]);
                    break;

                case 'r':
                    i++;
                    RoomNumber = atoi(argv[i]);
                    break;

                case 't':
                    i++;
                    strcpy(type, argv[i]);
                    break;

                default:
                    break;
            }
        }

        i++;
    }

    if (strcmp(type, "solo") == 0) {
        puts("Solo Mode");
        for (i=0; i<22; i++) {
            targets[i] = soloTargets[i];
        }
        breaktimeLimit = 2;
        maxItemSum = 20;
        puts("Set Parameters.");
    } else if (strcmp(type, "multi") == 0) {
        puts("Multiplayer Mode");
        for (i=0; i<22; i++) {
            targets[i] = multiTargets[i];
        }
        breaktimeLimit = 3;
        maxItemSum = 80;
        puts("Set Parameters.");
    } else {
        puts("Default Mode");
    }

    if (Init(argc, argv, ProxyAddress, ProxyPort) != 0) {
        return -1;
    }

    do {
        if (strcmp(UserName, "") == 0) {
            printf("Username: ");
            scanf("%s", UserName);
        }
        strcpy(param, "user=");
        strcat(param, UserName);
        if (strcmp(PassWord, "") == 0) {
            printf("Password: ");
            scanf("%s", PassWord);
        }
        strcat(param, "&pass=");
        strcat(param, PassWord);
        send_cmd("UserCheck", param, ReturnCode);

        printf("UserCheck L127\n");

        if (strcmp(ReturnCode, "roomNumber=") == 0) {
        } else {
            printf("\x1b[31m");
            printf("User Authorization Failed.\n");
            printf("\x1b[39m");
            return -1;
        }
    } while (strcmp(ReturnCode, "roomNumber=") != 0);

    do {
        if (RoomNumber < 0) {
            printf("RoomNumber=");
            scanf("%d", &RoomNumber);
        }
        printf("RoomNumber=%d\n", RoomNumber);
        strcpy(param, "roomNumber=");
        sprintf(buff, "%d", RoomNumber);
        strcat(param, buff);
        send_cmd("RoomNumberCheck", param, ReturnCode);

        if (strcmp(ReturnCode, "command1=") == 0) {
        } else {
            printf("\x1b[31m");
            printf("RoomCheckNG\n");
            printf("\x1b[39m");
            return -1;
        }
    } while (strcmp(ReturnCode, "command1=") != 0);

    printf("\x1b[32m");
    puts(" _____                             _   _               _____                              __       _ ");
    puts("/  __ \\                           | | (_)             /  ___|                            / _|     | |");
    puts("| /  \\/ ___  _ __  _ __   ___  ___| |_ _  ___  _ __   \\ `--. _   _  ___ ___ ___  ___ ___| |_ _   _| |");
    puts("| |    / _ \\| '_ \\| '_ \\ / _ \\/ __| __| |/ _ \\| '_ \\   `--. \\ | | |/ __/ __/ _ \\/ __/ __|  _| | | | |");
    puts("| \\__/\\ (_) | | | | | | |  __/ (__| |_| | (_) | | | | /\\__/ / |_| | (_| (_|  __/\\__ \\__ \\ | | |_| | |");
    puts(" \\____/\\___/|_| |_|_| |_|\\___|\\___|\\__|_|\\___/|_| |_| \\____/ \\__,_|\\___\\___\\___||___/___/_|  \\__,_|_|");

    printf("\x1b[33m");
    puts("Initializing ReturnNumbers");

    // Initialize Arrays
    for(i=0; i<9; i++) {
        returnNumber[i] = 0;
        returnNumberX[i] = 0;
        ActionReturnNumber[i] = 0;
    }
    ActionReturnNumber[0] = -10000;

    puts("Array Initialized.");

    while (1) {
        puts("----------------------------");
        printf("Turn: %d\n", turn);
        puts("----------------------------");

        /*
         * Generate GetReady
         */
        do {
            puts("Waiting for next turn\n");

            /*
            if (selfFlag) {
                parseReturnNumber(ActionCount, ActionReturnNumber);
            }
             */

            if (readyFlag == 0) {
                strcpy(param, "command1=");
                if (nextMode == 1 || nextMode == 3 || nextMode == 5 || nextMode == 7) {
                    puts("Next Mode Set");
                    GetReadyMode = nextMode;
                    nextMode = 0;
                } else {
                    GetReadyMode = 0;
                }

                if (ActionCount != CountBuff) {
                    if (ActionCount < CountBuff) {
                        GetReadyMode = 0;
                    }
                    CountBuff = ActionCount;
                }

                if (ActionCount == 1 || ActionCount == 2 || ActionCount == 3) {
                    GetReadyMode = 0;
                }

                switch (GetReadyMode) {
                    case 0:
                        strcat(param, "gr");
                        break;

                    case 1:
                        strcat(param, "gru"); // Up
                        break;

                    case 3:
                        strcat(param, "grl"); // Left
                        break;

                    case 5:
                        strcat(param, "grr"); // Right
                        break;

                    case 7:
                        strcat(param, "grd"); // Down
                        break;

                    default:
                        strcat(param, "gr");
                }

                puts("GetReadyMode");
                printf("Command: %s\n", param);
                printf("Waiting for next turn");

                readyFlag = 1;
            }

            send_cmd("GetReadyCheck", param, ReturnCode);
            if (strchr(ReturnCode, ',') != NULL) {
            } else {
                if (strcmp(ReturnCode, "user=") == 0) {
                    break;
                }
            }
        } while (strchr(ReturnCode, ',') == NULL);

        puts("");

        count = returnCode2int(ReturnCode, returnNumber);
        readyFlag = 0;

        if (selfFlag == 0) {
            self = returnNumber[4];

            printf("Your character is ");

            switch (self / 1000) {
                case 1:
                    printf("C\n");
                    break;
                case 2:
                    printf("H\n");
                    break;
                case 3:
                    printf("a\n");
                    break;
                case 4:
                    printf("s\n");
                    break;
                case 5:
                    printf("e\n");
                    break;
                case 6:
                    printf("r\n");
                    break;
                case 7:
                    printf("O\n");
                    break;
                case 8:
                    printf("n\n");
                    break;
            }

            puts("");

            selfFlag = 1;
        }

        /*
         * Prepare Action
         */
        do {
            strcpy(param, "command2=");

            parseReturnNumber(count, returnNumber);

            previousMode = mode;
            previousCount = count;

            for (i = 0; i < 9; i++) {
                if (!(i % 3)) {
                    puts("");
                }
                printf("%5d", returnNumberX[i]);
            }
            puts("\n");
            printf("Count: %d\n", count);
            printf("Self: %d(%c)\n", self, characters[self/1000 - 1]);

            maxTarget = 1000;
            itemsSum = 0;

            for (i = 0; i < 9; i++) {
                if (i == 4) continue;
                itemsSum += calcItem(returnNumberX[i]);
                for (j = 0; j < 22; j++) {
                    if (returnNumberX[i] == targets[j]) {
                        if (maxTarget > j) {
                            maxTarget = j;
                        }
                    }
                }
            }

            if (nextMode && thisActionFlag == 0) {
                printf("Set NextMode: %d\n", nextMode);
                mode = nextMode;
                thisActionFlag = 1;
            }

            k = 0;
            if (nextMode == 0) {
                puts("ROUTE---------------------------");
                printf("MaxTarget: %d\n", maxTarget);
                printf("Item sum: %d\n", itemsSum);
                for (i = 0; i < 8; i++) {

                    if (returnNumberX[way[i]] == targets[maxTarget]) {
                        puts("MATCH TARGET");
                        //printf("way=%d\n", way[i]);
                        /*
                        if (way[i] == 0 || way[i] == 2) {
                            if (returnNumberX[1] == 41 ||
                                returnNumberX[1] == 42 ||
                                returnNumberX[1] == 43 ||
                                returnNumberX[1] == 50 ||
                                returnNumberX[1] == 51 ||
                                returnNumberX[1] == 52 ||
                                returnNumberX[1] == 53) {
                                continue;
                            }
                        }

                        if (way[i] == 6 || way[i] == 8) {
                            if (returnNumberX[7] == 41 ||
                                returnNumberX[7] == 42 ||
                                returnNumberX[7] == 43 ||
                                returnNumberX[7] == 50 ||
                                returnNumberX[7] == 51 ||
                                returnNumberX[7] == 52 ||
                                returnNumberX[7] == 53) {
                                continue;
                            }
                        }
                         */

                        k = 1;

                        printf("Item: %d\n", targets[maxTarget]);
                        previousItem = targets[maxTarget];
                        if (way[i] == 0) {
                            if (returnNumberX[1] == 12 ||
                                returnNumberX[1] == 20 ||
                                returnNumberX[1] == 21 ||
                                returnNumberX[1] == 22 ||
                                returnNumberX[1] == 23 ||
                                returnNumberX[1] == 30 ||
                                returnNumberX[1] == 31 ||
                                returnNumberX[1] == 32 ||
                                returnNumberX[1] == 33 ||
                                returnNumberX[1] == 60 ||
                                returnNumberX[1] == 61 ||
                                returnNumberX[1] == 62 ||
                                returnNumberX[1] == 63 ||
                                returnNumberX[1] == 40 ||
                                returnNumberX[1] == 41 ||
                                returnNumberX[1] == 42 ||
                                returnNumberX[1] == 43 ||
                                returnNumberX[1] == 50 ||
                                returnNumberX[1] == 51 ||
                                returnNumberX[1] == 52 ||
                                returnNumberX[1] == 53) {
                                //printf("Left -> Top\n");
                                mode = 3;
                                nextMode = 1;
                            } else {
                                //printf("Top -> Left\n");
                                mode = 1;
                                nextMode = 3;
                            }
                        } else if (way[i] == 1) {
                            //printf("Top\n");
                            mode = 1;
                        } else if (way[i] == 2) {
                            if (returnNumberX[1] == 12 ||
                                returnNumberX[1] == 20 ||
                                returnNumberX[1] == 21 ||
                                returnNumberX[1] == 22 ||
                                returnNumberX[1] == 23 ||
                                returnNumberX[1] == 30 ||
                                returnNumberX[1] == 31 ||
                                returnNumberX[1] == 32 ||
                                returnNumberX[1] == 33 ||
                                returnNumberX[1] == 60 ||
                                returnNumberX[1] == 61 ||
                                returnNumberX[1] == 62 ||
                                returnNumberX[1] == 63 ||
                                returnNumberX[1] == 40 ||
                                returnNumberX[1] == 41 ||
                                returnNumberX[1] == 42 ||
                                returnNumberX[1] == 43 ||
                                returnNumberX[1] == 50 ||
                                returnNumberX[1] == 51 ||
                                returnNumberX[1] == 52 ||
                                returnNumberX[1] == 53) {
                                //printf("Right -> Top\n");
                                mode = 5;
                                nextMode = 1;
                            } else {
                                //printf("Top -> Right\n");
                                mode = 1;
                                nextMode = 5;
                            }
                        } else if (way[i] == 3) {
                            //printf("Left\n");
                            mode = 3;
                        } else if (way[i] == 5) {
                            //printf("Right\n");
                            mode = 5;
                        } else if (way[i] == 6) {
                            if (returnNumberX[7] == 12 ||
                                returnNumberX[7] == 20 ||
                                returnNumberX[7] == 21 ||
                                returnNumberX[7] == 22 ||
                                returnNumberX[7] == 23 ||
                                returnNumberX[7] == 30 ||
                                returnNumberX[7] == 31 ||
                                returnNumberX[7] == 32 ||
                                returnNumberX[7] == 33 ||
                                returnNumberX[7] == 60 ||
                                returnNumberX[7] == 61 ||
                                returnNumberX[7] == 62 ||
                                returnNumberX[7] == 63 ||
                                returnNumberX[7] == 40 ||
                                returnNumberX[7] == 41 ||
                                returnNumberX[7] == 42 ||
                                returnNumberX[7] == 43 ||
                                returnNumberX[7] == 50 ||
                                returnNumberX[7] == 51 ||
                                returnNumberX[7] == 52 ||
                                returnNumberX[7] == 53) {
                                //printf("Left -> Down\n");
                                mode = 3;
                                nextMode = 7;
                            } else {
                                //printf("Down -> Left\n");
                                mode = 7;
                                nextMode = 3;
                            }

                        } else if (way[i] == 7) {
                            //printf("Down\n");
                            mode = 7;
                        } else if (way[i] == 8) {
                            if (returnNumberX[7] == 12 ||
                                returnNumberX[7] == 20 ||
                                returnNumberX[7] == 21 ||
                                returnNumberX[7] == 22 ||
                                returnNumberX[7] == 23 ||
                                returnNumberX[7] == 30 ||
                                returnNumberX[7] == 31 ||
                                returnNumberX[7] == 32 ||
                                returnNumberX[7] == 33 ||
                                returnNumberX[7] == 60 ||
                                returnNumberX[7] == 61 ||
                                returnNumberX[7] == 62 ||
                                returnNumberX[7] == 63 ||
                                returnNumberX[7] == 40 ||
                                returnNumberX[7] == 41 ||
                                returnNumberX[7] == 42 ||
                                returnNumberX[7] == 43 ||
                                returnNumberX[7] == 50 ||
                                returnNumberX[7] == 51 ||
                                returnNumberX[7] == 52 ||
                                returnNumberX[7] == 53) {
                                //printf("Right -> Down\n");
                                mode = 5;
                                nextMode = 7;
                            } else {
                                //printf("Down -> Right\n");
                                mode = 7;
                                nextMode = 5;
                            }
                        }
                        printf("ROUTE MODE=%d\n", mode);
                        printf("ROUTE NEXTMODE=%d\n", nextMode);
                        puts("ROUTE COMPLETE------------------");
                        break;
                    }
                    if (k) {
                        break;
                    }
                }
                alert = 0;
            }

            if ((itemsSum < maxItemSum) && count == 6 && nextMode == 0 && thisActionFlag == 0) {
                printf("The item sums under %d.\n", maxItemSum);
                printf("Item sum=%d\n", itemsSum);
                nextMode = 0;

                if (returnNumberX[0] == -1 &&
                    returnNumberX[3] == -1 &&
                    returnNumberX[6] == -1) {
                    if (rand() % 2) {
                        mode = 21;
                    } else {
                        mode = 22;
                    }
                }

                if (returnNumberX[0] == -1 &&
                    returnNumberX[1] == -1 &&
                    returnNumberX[2] == -1) {
                    mode = nextMode = 7;
                }

                if (returnNumberX[6] == -1 &&
                    returnNumberX[7] == -1 &&
                    returnNumberX[8] == -1) {
                    mode = nextMode = 1;
                }

                if (returnNumberX[2] == -1 &&
                    returnNumberX[5] == -1 &&
                    returnNumberX[8] == -1) {
                    if (rand() % 2) {
                        mode = 23;
                    } else {
                        mode = 24;
                    }
                }
                alert = 0;
            }

            if (itemsSum < maxItemSum && count == 9 &&
                (
                        nextMode == 0 ||
                        previousItem == 20 ||
                        previousItem == 21 ||
                        previousItem == 22 ||
                        previousItem == 23 ||
                        previousItem == 30 ||
                        previousItem == 31 ||
                        previousItem == 32 ||
                        previousItem == 33 ||
                        previousItem == 60 ||
                        previousItem == 61 ||
                        previousItem == 62 ||
                        previousItem == 63
                ) && thisActionFlag == 0 &&
                (
                        targets[maxTarget] != 20 ||
                        targets[maxTarget] != 21 ||
                        targets[maxTarget] != 22 ||
                        targets[maxTarget] != 23 ||
                        targets[maxTarget] != 30 ||
                        targets[maxTarget] != 31 ||
                        targets[maxTarget] != 32 ||
                        targets[maxTarget] != 33 ||
                        targets[maxTarget] != 60 ||
                        targets[maxTarget] != 61 ||
                        targets[maxTarget] != 62 ||
                        targets[maxTarget] != 63
                )) {
                printf("The item sums under %d.\n", maxItemSum);
                printf("Item sum=%d\n", itemsSum);
                nextMode = 0;

                // Set previous mode
                /*
                for (i=21; i<25; i++) {
                    if (mode == i) {
                        previousMode = mode;
                    }
                }
                 */

                // Generate next command using random value.
                do {
                    mode = rand() % (25 - 21) + 21;
                }while (mode == previousMode);

                for (i=0; i<9; i++) {
                    for(j=20; j<=23; j++) {
                        if (returnNumberX[i] == j) {
                            if (i == 0) {
                                if (returnNumberX[1] == 12 ||
                                    returnNumberX[1] == 20 ||
                                    returnNumberX[1] == 21 ||
                                    returnNumberX[1] == 22 ||
                                    returnNumberX[1] == 23 ||
                                    returnNumberX[1] == 30 ||
                                    returnNumberX[1] == 31 ||
                                    returnNumberX[1] == 32 ||
                                    returnNumberX[1] == 33 ||
                                    returnNumberX[1] == 60 ||
                                    returnNumberX[1] == 61 ||
                                    returnNumberX[1] == 62 ||
                                    returnNumberX[1] == 63 ||
                                    returnNumberX[1] == 40 ||
                                    returnNumberX[1] == 41 ||
                                    returnNumberX[1] == 42 ||
                                    returnNumberX[1] == 43 ||
                                    returnNumberX[1] == 50 ||
                                    returnNumberX[1] == 51 ||
                                    returnNumberX[1] == 52 ||
                                    returnNumberX[1] == 53) {
                                    //printf("Left -> Top\n");
                                    mode = 3;
                                    nextMode = 1;
                                } else {
                                    //printf("Top -> Left\n");
                                    mode = 1;
                                    nextMode = 3;
                                }
                            } else if (i == 1) {
                                //printf("Top\n");
                                mode = 1;
                                nextMode = 0;
                            } else if (i == 2) {
                                if (returnNumberX[1] == 12 ||
                                    returnNumberX[1] == 20 ||
                                    returnNumberX[1] == 21 ||
                                    returnNumberX[1] == 22 ||
                                    returnNumberX[1] == 23 ||
                                    returnNumberX[1] == 30 ||
                                    returnNumberX[1] == 31 ||
                                    returnNumberX[1] == 32 ||
                                    returnNumberX[1] == 33 ||
                                    returnNumberX[1] == 60 ||
                                    returnNumberX[1] == 61 ||
                                    returnNumberX[1] == 62 ||
                                    returnNumberX[1] == 63 ||
                                    returnNumberX[1] == 40 ||
                                    returnNumberX[1] == 41 ||
                                    returnNumberX[1] == 42 ||
                                    returnNumberX[1] == 43 ||
                                    returnNumberX[1] == 50 ||
                                    returnNumberX[1] == 51 ||
                                    returnNumberX[1] == 52 ||
                                    returnNumberX[1] == 53) {
                                    //printf("Right -> Top\n");
                                    mode = 5;
                                    nextMode = 1;
                                } else {
                                    //printf("Top -> Right\n");
                                    mode = 1;
                                    nextMode = 5;
                                }
                            } else if (i == 3) {
                                //printf("Left\n");
                                mode = 3;
                                nextMode = 0;
                            } else if (i == 5) {
                                //printf("Right\n");
                                mode = 5;
                                nextMode = 0;
                            } else if (i == 6) {
                                if (returnNumberX[7] == 12 ||
                                    returnNumberX[7] == 20 ||
                                    returnNumberX[7] == 21 ||
                                    returnNumberX[7] == 22 ||
                                    returnNumberX[7] == 23 ||
                                    returnNumberX[7] == 30 ||
                                    returnNumberX[7] == 31 ||
                                    returnNumberX[7] == 32 ||
                                    returnNumberX[7] == 33 ||
                                    returnNumberX[7] == 60 ||
                                    returnNumberX[7] == 61 ||
                                    returnNumberX[7] == 62 ||
                                    returnNumberX[7] == 63 ||
                                    returnNumberX[7] == 40 ||
                                    returnNumberX[7] == 41 ||
                                    returnNumberX[7] == 42 ||
                                    returnNumberX[7] == 43 ||
                                    returnNumberX[7] == 50 ||
                                    returnNumberX[7] == 51 ||
                                    returnNumberX[7] == 52 ||
                                    returnNumberX[7] == 53) {
                                    //printf("Left -> Down\n");
                                    mode = 3;
                                    nextMode = 7;
                                } else {
                                    //printf("Down -> Left\n");
                                    mode = 7;
                                    nextMode = 3;
                                }

                            } else if (i == 7) {
                                //printf("Down\n");
                                mode = 7;
                                nextMode = 0;
                            } else if (i == 8) {
                                if (returnNumberX[7] == 12 ||
                                    returnNumberX[7] == 20 ||
                                    returnNumberX[7] == 21 ||
                                    returnNumberX[7] == 22 ||
                                    returnNumberX[7] == 23 ||
                                    returnNumberX[7] == 30 ||
                                    returnNumberX[7] == 31 ||
                                    returnNumberX[7] == 32 ||
                                    returnNumberX[7] == 33 ||
                                    returnNumberX[7] == 60 ||
                                    returnNumberX[7] == 61 ||
                                    returnNumberX[7] == 62 ||
                                    returnNumberX[7] == 63 ||
                                    returnNumberX[7] == 40 ||
                                    returnNumberX[7] == 41 ||
                                    returnNumberX[7] == 42 ||
                                    returnNumberX[7] == 43 ||
                                    returnNumberX[7] == 50 ||
                                    returnNumberX[7] == 51 ||
                                    returnNumberX[7] == 52 ||
                                    returnNumberX[7] == 53) {
                                    //printf("Right -> Down\n");
                                    mode = 5;
                                    nextMode = 7;
                                } else {
                                    //printf("Down -> Right\n");
                                    mode = 7;
                                    nextMode = 5;
                                }
                            }
                        }
                    }

                    for(j=30; j<=33; j++) {
                        if (returnNumberX[i] == j) {
                            if (i == 0) {
                                if (returnNumberX[1] == 12 ||
                                    returnNumberX[1] == 20 ||
                                    returnNumberX[1] == 21 ||
                                    returnNumberX[1] == 22 ||
                                    returnNumberX[1] == 23 ||
                                    returnNumberX[1] == 30 ||
                                    returnNumberX[1] == 31 ||
                                    returnNumberX[1] == 32 ||
                                    returnNumberX[1] == 33 ||
                                    returnNumberX[1] == 60 ||
                                    returnNumberX[1] == 61 ||
                                    returnNumberX[1] == 62 ||
                                    returnNumberX[1] == 63 ||
                                    returnNumberX[1] == 40 ||
                                    returnNumberX[1] == 41 ||
                                    returnNumberX[1] == 42 ||
                                    returnNumberX[1] == 43 ||
                                    returnNumberX[1] == 50 ||
                                    returnNumberX[1] == 51 ||
                                    returnNumberX[1] == 52 ||
                                    returnNumberX[1] == 53) {
                                    //printf("Left -> Top\n");
                                    mode = 3;
                                    nextMode = 1;
                                } else {
                                    //printf("Top -> Left\n");
                                    mode = 1;
                                    nextMode = 3;
                                }
                            } else if (i == 1) {
                                //printf("Top\n");
                                mode = 1;
                                nextMode = 0;
                            } else if (i == 2) {
                                if (returnNumberX[1] == 12 ||
                                    returnNumberX[1] == 20 ||
                                    returnNumberX[1] == 21 ||
                                    returnNumberX[1] == 22 ||
                                    returnNumberX[1] == 23 ||
                                    returnNumberX[1] == 30 ||
                                    returnNumberX[1] == 31 ||
                                    returnNumberX[1] == 32 ||
                                    returnNumberX[1] == 33 ||
                                    returnNumberX[1] == 60 ||
                                    returnNumberX[1] == 61 ||
                                    returnNumberX[1] == 62 ||
                                    returnNumberX[1] == 63 ||
                                    returnNumberX[1] == 40 ||
                                    returnNumberX[1] == 41 ||
                                    returnNumberX[1] == 42 ||
                                    returnNumberX[1] == 43 ||
                                    returnNumberX[1] == 50 ||
                                    returnNumberX[1] == 51 ||
                                    returnNumberX[1] == 52 ||
                                    returnNumberX[1] == 53) {
                                    //printf("Right -> Top\n");
                                    mode = 5;
                                    nextMode = 1;
                                } else {
                                    //printf("Top -> Right\n");
                                    mode = 1;
                                    nextMode = 5;
                                }
                            } else if (i == 3) {
                                //printf("Left\n");
                                mode = 3;
                                nextMode = 0;
                            } else if (i == 5) {
                                //printf("Right\n");
                                mode = 5;
                                nextMode = 0;
                            } else if (i == 6) {
                                if (returnNumberX[7] == 12 ||
                                    returnNumberX[7] == 20 ||
                                    returnNumberX[7] == 21 ||
                                    returnNumberX[7] == 22 ||
                                    returnNumberX[7] == 23 ||
                                    returnNumberX[7] == 30 ||
                                    returnNumberX[7] == 31 ||
                                    returnNumberX[7] == 32 ||
                                    returnNumberX[7] == 33 ||
                                    returnNumberX[7] == 60 ||
                                    returnNumberX[7] == 61 ||
                                    returnNumberX[7] == 62 ||
                                    returnNumberX[7] == 63 ||
                                    returnNumberX[7] == 40 ||
                                    returnNumberX[7] == 41 ||
                                    returnNumberX[7] == 42 ||
                                    returnNumberX[7] == 43 ||
                                    returnNumberX[7] == 50 ||
                                    returnNumberX[7] == 51 ||
                                    returnNumberX[7] == 52 ||
                                    returnNumberX[7] == 53) {
                                    //printf("Left -> Down\n");
                                    mode = 3;
                                    nextMode = 7;
                                } else {
                                    //printf("Down -> Left\n");
                                    mode = 7;
                                    nextMode = 3;
                                }

                            } else if (i == 7) {
                                //printf("Down\n");
                                mode = 7;
                                nextMode = 0;
                            } else if (i == 8) {
                                if (returnNumberX[7] == 12 ||
                                    returnNumberX[7] == 20 ||
                                    returnNumberX[7] == 21 ||
                                    returnNumberX[7] == 22 ||
                                    returnNumberX[7] == 23 ||
                                    returnNumberX[7] == 30 ||
                                    returnNumberX[7] == 31 ||
                                    returnNumberX[7] == 32 ||
                                    returnNumberX[7] == 33 ||
                                    returnNumberX[7] == 60 ||
                                    returnNumberX[7] == 61 ||
                                    returnNumberX[7] == 62 ||
                                    returnNumberX[7] == 63 ||
                                    returnNumberX[7] == 40 ||
                                    returnNumberX[7] == 41 ||
                                    returnNumberX[7] == 42 ||
                                    returnNumberX[7] == 43 ||
                                    returnNumberX[7] == 50 ||
                                    returnNumberX[7] == 51 ||
                                    returnNumberX[7] == 52 ||
                                    returnNumberX[7] == 53) {
                                    //printf("Right -> Down\n");
                                    mode = 5;
                                    nextMode = 7;
                                } else {
                                    //printf("Down -> Right\n");
                                    mode = 7;
                                    nextMode = 5;
                                }
                            }
                        }
                    }

                    for(j=60; j<=63; j++) {
                        if (returnNumberX[i] == j) {
                            if (i == 0) {
                                if (returnNumberX[1] == 12 ||
                                    returnNumberX[1] == 20 ||
                                    returnNumberX[1] == 21 ||
                                    returnNumberX[1] == 22 ||
                                    returnNumberX[1] == 23 ||
                                    returnNumberX[1] == 30 ||
                                    returnNumberX[1] == 31 ||
                                    returnNumberX[1] == 32 ||
                                    returnNumberX[1] == 33 ||
                                    returnNumberX[1] == 60 ||
                                    returnNumberX[1] == 61 ||
                                    returnNumberX[1] == 62 ||
                                    returnNumberX[1] == 63 ||
                                    returnNumberX[1] == 40 ||
                                    returnNumberX[1] == 41 ||
                                    returnNumberX[1] == 42 ||
                                    returnNumberX[1] == 43 ||
                                    returnNumberX[1] == 50 ||
                                    returnNumberX[1] == 51 ||
                                    returnNumberX[1] == 52 ||
                                    returnNumberX[1] == 53) {
                                    //printf("Left -> Top\n");
                                    mode = 3;
                                    nextMode = 1;
                                } else {
                                    //printf("Top -> Left\n");
                                    mode = 1;
                                    nextMode = 3;
                                }
                            } else if (i == 1) {
                                //printf("Top\n");
                                mode = 1;
                                nextMode = 0;
                            } else if (i == 2) {
                                if (returnNumberX[1] == 12 ||
                                    returnNumberX[1] == 20 ||
                                    returnNumberX[1] == 21 ||
                                    returnNumberX[1] == 22 ||
                                    returnNumberX[1] == 23 ||
                                    returnNumberX[1] == 30 ||
                                    returnNumberX[1] == 31 ||
                                    returnNumberX[1] == 32 ||
                                    returnNumberX[1] == 33 ||
                                    returnNumberX[1] == 60 ||
                                    returnNumberX[1] == 61 ||
                                    returnNumberX[1] == 62 ||
                                    returnNumberX[1] == 63 ||
                                    returnNumberX[1] == 40 ||
                                    returnNumberX[1] == 41 ||
                                    returnNumberX[1] == 42 ||
                                    returnNumberX[1] == 43 ||
                                    returnNumberX[1] == 50 ||
                                    returnNumberX[1] == 51 ||
                                    returnNumberX[1] == 52 ||
                                    returnNumberX[1] == 53) {
                                    //printf("Right -> Top\n");
                                    mode = 5;
                                    nextMode = 1;
                                } else {
                                    //printf("Top -> Right\n");
                                    mode = 1;
                                    nextMode = 5;
                                }
                            } else if (i == 3) {
                                //printf("Left\n");
                                mode = 3;
                                nextMode = 0;
                            } else if (i == 5) {
                                //printf("Right\n");
                                mode = 5;
                                nextMode = 0;
                            } else if (i == 6) {
                                if (returnNumberX[7] == 12 ||
                                    returnNumberX[7] == 20 ||
                                    returnNumberX[7] == 21 ||
                                    returnNumberX[7] == 22 ||
                                    returnNumberX[7] == 23 ||
                                    returnNumberX[7] == 30 ||
                                    returnNumberX[7] == 31 ||
                                    returnNumberX[7] == 32 ||
                                    returnNumberX[7] == 33 ||
                                    returnNumberX[7] == 60 ||
                                    returnNumberX[7] == 61 ||
                                    returnNumberX[7] == 62 ||
                                    returnNumberX[7] == 63 ||
                                    returnNumberX[7] == 40 ||
                                    returnNumberX[7] == 41 ||
                                    returnNumberX[7] == 42 ||
                                    returnNumberX[7] == 43 ||
                                    returnNumberX[7] == 50 ||
                                    returnNumberX[7] == 51 ||
                                    returnNumberX[7] == 52 ||
                                    returnNumberX[7] == 53) {
                                    //printf("Left -> Down\n");
                                    mode = 3;
                                    nextMode = 7;
                                } else {
                                    //printf("Down -> Left\n");
                                    mode = 7;
                                    nextMode = 3;
                                }

                            } else if (i == 7) {
                                //printf("Down\n");
                                mode = 7;
                                nextMode = 0;
                            } else if (i == 8) {
                                if (returnNumberX[7] == 12 ||
                                    returnNumberX[7] == 20 ||
                                    returnNumberX[7] == 21 ||
                                    returnNumberX[7] == 22 ||
                                    returnNumberX[7] == 23 ||
                                    returnNumberX[7] == 30 ||
                                    returnNumberX[7] == 31 ||
                                    returnNumberX[7] == 32 ||
                                    returnNumberX[7] == 33 ||
                                    returnNumberX[7] == 60 ||
                                    returnNumberX[7] == 61 ||
                                    returnNumberX[7] == 62 ||
                                    returnNumberX[7] == 63 ||
                                    returnNumberX[7] == 40 ||
                                    returnNumberX[7] == 41 ||
                                    returnNumberX[7] == 42 ||
                                    returnNumberX[7] == 43 ||
                                    returnNumberX[7] == 50 ||
                                    returnNumberX[7] == 51 ||
                                    returnNumberX[7] == 52 ||
                                    returnNumberX[7] == 53) {
                                    //printf("Right -> Down\n");
                                    mode = 5;
                                    nextMode = 7;
                                } else {
                                    //printf("Down -> Right\n");
                                    mode = 7;
                                    nextMode = 5;
                                }
                            }
                        }
                    }
                }

                k = 0;

                for (i=40; i<44; i++) {
                    for (j=0; j<9; j++) {
                        if ((returnNumberX[j] == i || returnNumberX[j] == i + 10) && breaktime < breaktimeLimit) {
                            k = 1;

                            printf("\x1b[34m");
                            puts("    ____                  __   __  _              ");
                            puts("   / __ )________  ____ _/ /__/ /_(_)___ ___  ___ ");
                            puts("  / __  / ___/ _ \\/ __ `/ //_/ __/ / __ `__ \\/ _ \\");
                            puts(" / /_/ / /  /  __/ /_/ / ,< / /_/ / / / / / /  __/");
                            puts("/_____/_/   \\___/\\__,_/_/|_|\\__/_/_/ /_/ /_/\\___/ ");
                            puts("                                                  ");
                            puts("\x1b[33m");

                            if (j == 1) {
                                mode = 43;
                                nextMode = 1;
                            }

                            if (j == 3) {
                                mode = 42;
                                nextMode = 3;
                            }

                            if (j == 5) {
                                mode = 41;
                                nextMode = 5;
                            }

                            if (j == 7) {
                                mode = 44;
                                nextMode = 7;
                            }

                            breaktime++;
                            totalBreak++;
                        }
                    }

                    if (k) {
                        break;
                    }
                }
                alert = 0;
            }

            if (count == -1 || alert) {
                printf("\x1b[31m");
                puts("ALERT: OUTSIDE.");
                printf("\x1b[39m");

                if (previousMode == 1 || previousMode == 11 || previousMode == 21 || previousMode == 23) {
                    mode = 7;

                    if (previousMode == 21) {
                        mode = 24;
                    } else if(previousMode == 23) {
                        mode = 22;
                    }
                }

                if (previousMode == 7 || previousMode == 10 || previousMode == 22 || previousMode == 24) {
                    mode = 1;

                    if (previousMode == 22) {
                        mode = 23;
                    } else if(previousMode == 24) {
                        mode = 21;
                    }
                }

                if (previousMode == 3 || previousMode == 13 || previousMode == 23 || previousMode == 24) {
                    mode = 5;

                    if (previousMode == 23) {
                        mode = 22;
                    } else if(previousMode == 24) {
                        mode = 21;
                    }
                }

                if (previousMode == 5 || previousMode == 12 || previousMode == 21 || previousMode == 22) {
                    mode = 5;

                    if (previousMode == 21) {
                        mode = 24;
                    } else if(previousMode == 22) {
                        mode = 23;
                    }
                }

                if (previousMode == 14)  {
                    mode = 24;
                }

                if (previousMode == 15)  {
                    mode = 22;
                }

                if (previousMode == 16)  {
                    mode = 23;
                }

                if (previousMode == 17)  {
                    mode = 21;
                }

                alert = 1;
            } else {
                alert = 0;
            }

            /*
            if (previousCount == -1 || count == -1) {
                mode = nextMode = previousMode;
            }
             */


            if (alert) {
                printf("\x1b[31m");
                puts("     _    _     _____ ____ _____ ");
                puts("    / \\  | |   | ____|  _ \\_   _|");
                puts("   / _ \\ | |   |  _| | |_) || |  ");
                puts("  / ___ \\| |___| |___|  _ < | |  ");
                puts(" /_/   \\_\\_____|_____|_| \\_\\|_|  ");
                puts("                                 ");
                printf("Set Mode to %d\n", mode);
                printf("\x1b[33m");
            }

            for (i = 0; i < 9; i++) {
                if (returnNumberX[i] >= 1000 && returnNumberX[i] != self) {
                    printf("\x1b[35m");
                    puts("");
                    puts("_    ____ _  _ _  _ ____ _  _    ____ ___ ___ ____ ____ _  _ ");
                    puts("|    |__| |  | |\\ | |    |__|    |__|  |   |  |__| |    |_/  ");
                    puts("|___ |  | |__| | \\| |___ |  |    |  |  |   |  |  | |___ | \\_ ");
                    puts("                                                             ");
                    puts("");
                    printf("\x1b[33m");

                    if (i == 0) {
                        mode = 15;
                    }
                    if (i == 1) {
                        mode = 10;
                    }
                    if (i == 2) {
                        mode = 14;
                    }
                    if (i == 3) {
                        mode = 12;
                    }
                    if (i == 5) {
                        mode = 13;
                    }
                    if (i == 6) {
                        mode = 17;
                    }
                    if (i == 7) {
                        mode = 11;
                    }
                    if (i == 8) {
                        mode = 16;
                    }

                    nextMode = thisActionFlag = 0;
                    totalAttack++;
                    break;
                }
            }

            if (thisActionFlag) {
                thisActionFlag = 0;
                nextMode = 0;
            }

            if (count == 3) {
                puts("WARNING: ROUTE PREVIOUS MODE");
                nextMode = 0;
                switch (previousMode) {
                    case 1:
                        mode = 7;
                        break;
                    case 3:
                        mode = 5;
                        break;
                    case 5:
                        mode = 3;
                        break;
                    case 7:
                        mode = 1;
                        break;
                    case 10:
                        mode = 1;
                        break;
                    case 11:
                        mode = 7;
                        break;
                    case 12:
                        mode = 3;
                        break;
                    case 13:
                        mode = 5;
                        break;
                    case 14:
                        mode = 21;
                        break;
                    case 15:
                        mode = 23;
                        break;
                    case 16:
                        mode = 22;
                        break;
                    case 17:
                        mode = 24;
                        break;
                    case 21:
                        mode = 24;
                        break;
                    case 22:
                        mode = 23;
                        break;
                    case 23:
                        mode = 22;
                        break;
                    case 24:
                        mode = 21;
                        break;
                    case 31:
                        mode = 21;
                        break;
                    case 32:
                        mode = 23;
                        break;
                    case 33:
                        mode = 22;
                        break;
                    case 34:
                        mode = 24;
                        break;
                }
            }

            /**
             *  course out routine
             */

            if (returnNumberX[0] == -1 &&
                returnNumberX[1] == -1 &&
                returnNumberX[2] == -1 &&
                returnNumberX[3] == -1 &&
                returnNumberX[4] == self &&
                returnNumberX[5] == 0 &&
                returnNumberX[6] == -1 &&
                returnNumberX[7] == 0 &&
                returnNumberX[8] == 0) {
                printf("\x1b[31m");
                puts("ALERT: FORCE COMMAND MODE");
                printf("\x1b[39m");
                mode = nextMode = 22;
            }

            if (returnNumberX[0] == -1 &&
                returnNumberX[1] == -1 &&
                returnNumberX[2] == -1 &&
                returnNumberX[3] == 0 &&
                returnNumberX[4] == self &&
                returnNumberX[5] == -1 &&
                returnNumberX[6] == 0 &&
                returnNumberX[7] == 0 &&
                returnNumberX[8] == -1) {
                printf("\x1b[31m");
                puts("ALERT: FORCE COMMAND MODE");
                printf("\x1b[39m");
                mode = nextMode = 24;
            }

            if (returnNumberX[0] == 0 &&
                returnNumberX[1] == 0 &&
                returnNumberX[2] == -1 &&
                returnNumberX[3] == 0 &&
                returnNumberX[4] == self &&
                returnNumberX[5] == -1 &&
                returnNumberX[6] == -1 &&
                returnNumberX[7] == -1 &&
                returnNumberX[8] == -1) {
                printf("\x1b[31m");
                puts("ALERT: FORCE COMMAND MODE");
                printf("\x1b[39m");
                mode = nextMode = 23;
            }

            if (returnNumberX[0] == -1 &&
                returnNumberX[1] == 0 &&
                returnNumberX[2] == 0 &&
                returnNumberX[3] == -1 &&
                returnNumberX[4] == self &&
                returnNumberX[5] == 0 &&
                returnNumberX[6] == -1 &&
                returnNumberX[7] == -1 &&
                returnNumberX[8] == -1) {
                printf("\x1b[31m");
                puts("ALERT: FORCE COMMAND MODE");
                printf("\x1b[39m");
                mode = nextMode = 21;
            }

            /*
            printf("mode=%d\n", mode);
            printf("nextMode=%d\n", nextMode);
            */

            switch (mode) {
                case 1:
                    strcat(param, "wu"); // Up
                    break;

                case 3:
                    strcat(param, "wl"); // Left
                    break;

                case 5:
                    strcat(param, "wr"); // Right
                    break;

                case 7:
                    strcat(param, "wd"); // Down
                    break;

                case 10:
                    strcat(param, "pu2wd"); // 上にブロックを置いて下に逃げる
                    break;

                case 11:
                    strcat(param, "pd2wu"); // 下にブロックを置いて上に逃げる
                    break;

                case 12:
                    strcat(param, "pl2wr"); // 左にブロックを置いて右に逃げる
                    break;

                case 13:
                    strcat(param, "pr2wl"); // 右にブロックを置いて左に逃げる
                    break;

                case 14:
                    strcat(param, "pru2wld"); // 右上にブロックを置いて左下に逃げる
                    break;

                case 15:
                    strcat(param, "plu2wrd"); // 左上にブロックを置いて右下に逃げる
                    break;

                case 16:
                    strcat(param, "prd2wlu"); // 右下にブロックを置いて左上に逃げる
                    break;

                case 17:
                    strcat(param, "pld2wru"); // 左下にブロックを置き右上に逃げる
                    break;

                case 21:
                    strcat(param, "keiru"); // 右上へ桂馬のような動きをする
                    break;

                case 22:
                    strcat(param, "keird"); // 右下へ桂馬のような動きをする
                    break;

                case 23:
                    strcat(param, "keilu"); // 左上へ桂馬のような動きをする
                    break;

                case 24:
                    strcat(param, "keild"); // 左下へ桂馬のような動きをする
                    break;

                case 31:
                    strcat(param, "pru0wld"); // 右上の土を砕いて左下に移動する
                    break;

                case 32:
                    strcat(param, "plu0wrd"); // 左上の土を砕いて右下に移動する
                    break;

                case 33:
                    strcat(param, "prd0wlu"); // 右下の土を砕いて左上に移動する
                    break;

                case 34:
                    strcat(param, "pld0wru"); // 左下の土を砕いて右上に移動する
                    break;

                case 41:
                    strcat(param, "pr0"); // 右にある土を砕きます
                    break;

                case 42:
                    strcat(param, "pl0"); // 左にある土を砕きます
                    break;

                case 43:
                    strcat(param, "pu0"); // 上にある土を砕きます
                    break;

                case 44:
                    strcat(param, "pd0"); // 下にある土を砕きます
                    break;

                default:
                    strcat(param, "wr");
            }

            printf("Action Command: %s\n", param);

            send_cmd("CommandCheck", param, ReturnCode);
        } while (strchr(ReturnCode, ',') == NULL && strcmp(ReturnCode, "user=") != 0);
        ActionCount = returnCode2int(ReturnCode, ActionReturnNumber);

        puts("");

        i = 0;
        do {
            send_cmd("EndCommandCheck", "command3=%23", ReturnCode);
            if (strcmp(ReturnCode, "command1=") == 0) {
            } else if (strcmp(ReturnCode, "user=") == 0 || i++ > 5) {
                printf("\x1b[32m");
                puts("");
                puts("  ________                        _________       __   ");
                puts(" /  _____/_____    _____   ____  /   _____/ _____/  |_ ");
                puts("/   \\  ___\\__  \\  /     \\_/ __ \\ \\_____  \\_/ __ \\   __\\");
                puts("\\    \\_\\  \\/ __ \\|  Y Y  \\  ___/ /        \\  ___/|  |  ");
                puts(" \\______  (____  /__|_|  /\\___  >_______  /\\___  >__|  ");
                puts("        \\/     \\/      \\/     \\/        \\/     \\/      ");
                puts("");
                puts("---------------------------------------------------------------");
                printf("Total Attack: %d\n", totalAttack);
                printf("Total Break: %d\n", totalBreak);
                printf("\x1b[39m");
                return 0;
            } else {
            }
        } while (strcmp(ReturnCode, "command1=") != 0 && strcmp(ReturnCode, "user=") != 0);

        puts("----------------------------");
        puts("Turn End.");
        puts("----------------------------");
        turn++;
    }
}

void parseReturnNumber(int count, int* returnNumber) {
    int i, j;

    for (i=0; i<9; i++) {

    }

    if (count == -1) {
        for (i = 0; i < 9; i++) {
            returnNumberX[i] = -1;
        }
    }

    if (count == 4) {
        if (returnNumber[0] == self) {
            returnNumberX[0] = -1;
            returnNumberX[1] = -1;
            returnNumberX[2] = -1;
            returnNumberX[3] = -1;
            returnNumberX[4] = returnNumber[0];
            returnNumberX[5] = returnNumber[1];
            returnNumberX[6] = -1;
            returnNumberX[7] = returnNumber[2];
            returnNumberX[8] = returnNumber[3];
        }

        if (returnNumber[1] == self) {
            returnNumberX[0] = -1;
            returnNumberX[1] = -1;
            returnNumberX[2] = -1;
            returnNumberX[3] = returnNumber[0];
            returnNumberX[4] = returnNumber[1];
            returnNumberX[5] = -1;
            returnNumberX[6] = returnNumber[2];
            returnNumberX[7] = returnNumber[3];
            returnNumberX[8] = -1;
        }

        if (returnNumber[3] == self) {
            returnNumberX[0] = returnNumber[0];
            returnNumberX[1] = returnNumber[1];
            returnNumberX[2] = -1;
            returnNumberX[3] = returnNumber[2];
            returnNumberX[4] = returnNumber[3];
            returnNumberX[5] = -1;
            returnNumberX[6] = -1;
            returnNumberX[7] = -1;
            returnNumberX[8] = -1;
        }

        if (returnNumber[2] == self) {
            returnNumberX[0] = -1;
            returnNumberX[1] = returnNumber[0];
            returnNumberX[2] = returnNumber[1];
            returnNumberX[3] = -1;
            returnNumberX[4] = returnNumber[2];
            returnNumberX[5] = returnNumber[3];
            returnNumberX[6] = -1;
            returnNumberX[7] = -1;
            returnNumberX[8] = -1;
        }
    }

    if (count == 6) {
        if (returnNumber[4] == self) {
            returnNumberX[0] = returnNumber[0];
            returnNumberX[1] = returnNumber[1];
            returnNumberX[2] = returnNumber[2];
            returnNumberX[3] = returnNumber[3];
            returnNumberX[4] = returnNumber[4];
            returnNumberX[5] = returnNumber[5];
            returnNumberX[6] = -1;
            returnNumberX[7] = -1;
            returnNumberX[8] = -1;
        }

        if (returnNumber[2] == self) {
            returnNumberX[0] = -1;
            returnNumberX[1] = returnNumber[0];
            returnNumberX[2] = returnNumber[1];
            returnNumberX[3] = -1;
            returnNumberX[4] = returnNumber[2];
            returnNumberX[5] = returnNumber[3];
            returnNumberX[6] = -1;
            returnNumberX[7] = returnNumber[4];
            returnNumberX[8] = returnNumber[5];
        }

        if (returnNumber[1] == self) {
            returnNumberX[0] = -1;
            returnNumberX[1] = -1;
            returnNumberX[2] = -1;
            returnNumberX[3] = returnNumber[0];
            returnNumberX[4] = returnNumber[1];
            returnNumberX[5] = returnNumber[2];
            returnNumberX[6] = returnNumber[3];
            returnNumberX[7] = returnNumber[4];
            returnNumberX[8] = returnNumber[5];
        }

        if (returnNumber[3] == self) {
            returnNumberX[0] = returnNumber[0];
            returnNumberX[1] = returnNumber[1];
            returnNumberX[2] = -1;
            returnNumberX[3] = returnNumber[2];
            returnNumberX[4] = returnNumber[3];
            returnNumberX[5] = -1;
            returnNumberX[6] = returnNumber[4];
            returnNumberX[7] = returnNumber[5];
            returnNumberX[8] = -1;
        }
    }

    if (count == 9) {
        for (i = 0; i < 9; i++) {
            returnNumberX[i] = returnNumber[i];
        }
    }
}

int calcItem(int item) {
    if (item == 1) {
        return 50;
    }
    if (item == 3) {
        return 10;
    }
    if (item == 5 || item == 6 || item == 7 || item == 8) {
        return 150;
    }
    if (item == 9) {
        return 100;
    }

    return 0;
}

int send_cmd(char *command, char *param, char *returnCode) {
    char buf[BUF_LEN];
    char WebBuf[BUF_LEN * 40];
    int s;
    char send_buf[BUF_LEN];

    static char SessionID[100];
    char *SessionIDstart;
    char *SessionIDend;
    int SessionIDlength;

    char ReturnBuf[BUF_LEN];
    char *ReturnBufStart;
    char *ReturnBufEnd;
    int ReturnBufLength;

    char keyword[MAX_KEYWORD][30] =
            {
                    "user=", "<input",
                    "command1=", "<input",
                    "GetReady ReturnCode=", "\n",
                    "command2=", "<input",
                    "Action ReturnCode=", "\n",
                    "command3=", "<input",
                    "roomNumber=", "<input"
            };


    int i;

    returnCode[0] = '\0';

    if ((s = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        fprintf(stderr, "Failed to create socket.\n");
        return 1;
    }

    if (connect(s, (struct sockaddr *) &server, sizeof(server)) == -1) {
        fprintf(stderr, "Connection Failed\n");
        return 1;
    }

    if (strcmp(SessionID, "") == 0) {
        sprintf(send_buf,
                "GET http://%s/CHaserOnline003/user/%s?%s HTTP/1.1\r\n",
                host, command, param);
    } else {
        sprintf(send_buf,
                "GET http://%s/CHaserOnline003/user/%s;jsessionid=%s?%s HTTP/1.1\r\n",
                host, command, SessionID, param);
    }

    //printf("send_buf=%s\n", send_buf);

    write(s, send_buf, strlen(send_buf));
    sprintf(send_buf, "Host: %s:%d\r\n", host, port);
    write(s, send_buf, strlen(send_buf));
    sprintf(send_buf, "\r\n");
    write(s, send_buf, strlen(send_buf));

    int read_size;
    read_size = read(s, buf, BUF_LEN);
    buf[read_size] = '\0';

    if (read_size > 0) {
        //printf("\nbuf=%s", buf);

        strcpy(WebBuf, buf);

        do {
            read_size = read(s, buf, BUF_LEN);
            //write(1, buf, read_size);
            buf[read_size] = '\0';

            //printf("\nbuf=%s", buf);
            strcat(WebBuf, buf);
        } while (read_size >= BUF_LEN);
        //printf("\nlen(WebBuf)=%lu\n", strlen(WebBuf));

        if (SessionID[0] == '\0') {
            SessionIDstart = strstr(WebBuf, "JSESSIONID=");
            if (SessionIDstart != NULL) {
                SessionIDend = strchr(SessionIDstart, ';');
                if (SessionIDend != NULL) {
                    SessionIDlength = SessionIDend - SessionIDstart - 11;
                    strncpy(SessionID, SessionIDstart + 11, SessionIDlength);
                    SessionID[SessionIDlength] = '\0';
                } else {
                }
            } else {
            }
        }

        for (i = 0; i <= MAX_KEYWORD; i = i + 2) {
            if (keyword[i][0] == '\0') {
                break;
            }

            ReturnBufStart = strstr(WebBuf, keyword[i]);
            if (ReturnBufStart != NULL) {
                ReturnBufEnd = strstr(ReturnBufStart, keyword[i + 1]);
                if (ReturnBufEnd != NULL) {
                    ReturnBufLength = ReturnBufEnd - ReturnBufStart - strlen(keyword[i]);
                    if (ReturnBufLength == 0) {
                        strcpy(ReturnBuf, keyword[i]);
                    } else {
                        strncpy(ReturnBuf, ReturnBufStart + strlen(keyword[i]), ReturnBufLength);
                        ReturnBuf[ReturnBufLength] = '\0';
                    }

                    if (strlen(ReturnBuf) == 1) {
                        strcat(ReturnBuf, ",");
                    }

                    strcpy(returnCode, ReturnBuf);
                    i = MAX_KEYWORD + 1;


                    //printf("command=%s\n", command);
                    printf("\nsend_cmd - execute log-------------\n");
                    printf("ReturnCode=%s\n", returnCode);

                    close(s);
                    return 0;
                }
            }
        }
    }
    /* 後始末 */
    close(s);
    return 0;
}

int returnCode2int(char *returnCode, int *returnNumber) {
    int i = 0;
    char *buf;
    int count = 0;

    buf = strtok(returnCode, ",");
    if (buf != NULL) {
        count++;
        returnNumber[i] = atoi(buf);
        for (i = 1; i < 9; i++) {
            buf = strtok(NULL, ",");
            if (buf != NULL) {
                count++;
                returnNumber[i] = atoi(buf);
            } else {
                break;
            }
        }
        return count;
    } else {
        return -1;
    }
}

int Init(int argc, char **argv, char *ProxyAddress, int ProxyPort) {
    struct hostent *servhost;
    struct servent *service;

    if (argc > 1) {
        char host_path[BUF_LEN];

        if (strlen(argv[1]) > BUF_LEN - 1) {
            fprintf(stderr, "URL is too long.\n");
            return 1;
        }

        if (strstr(argv[1], "http://") &&
            sscanf(argv[1], "http://%s", host_path) &&
            strcmp(argv[1], "http://")) {

            char *p;
            p = strchr(host_path, '/');
            if (p != NULL) {
                strcpy(path, p);
                *p = '\0';
                strcpy(host, host_path);
            } else {
                strcpy(host, host_path);
            }

            p = strchr(host, ':');
            if (p != NULL) {
                port = atoi(p + 1);
                if (port <= 0) {
                    port = 80;
                }
                *p = '\0';
            }
        } else {
            fprintf(stderr, "URL は http://host/path の形式で指定してください。\n");
            return 1;
        }
    }

    printf("Game Server: http://%s%s\n", host, path);

    if (strcmp(ProxyAddress, "") == 0) {
        servhost = gethostbyname(host);
        if (servhost == NULL) {
            fprintf(stderr, "[%s] から IP アドレスへの変換に失敗しました。\n", host);
            return 0;
        }
    } else {
        servhost = gethostbyname(ProxyAddress);
        if (servhost == NULL) {
            fprintf(stderr, "[%s] から IP アドレスへの変換に失敗しました。\n", ProxyAddress);
            return 0;
        }
    }

    bzero(&server, sizeof(server));
    //printf("[debug]bzero()\n");

    server.sin_family = AF_INET;

    bcopy(servhost->h_addr, &server.sin_addr, servhost->h_length);

    if (strcmp(ProxyAddress, "") == 0) {
        if (port != 0) {
            server.sin_port = htons(port);
        } else {
            service = getservbyname("http", "tcp");
            if (service != NULL) {
                server.sin_port = service->s_port;
            } else {
                server.sin_port = htons(80);
            }
        }
    } else {
        server.sin_port = htons(ProxyPort);
    }

    return 0;
}
