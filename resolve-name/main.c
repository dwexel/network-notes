#include <stdio.h>
#include <string.h>
#include <winsock2.h>

#pragma comment(lib,"ws2_32.lib") //Winsock Library

int err();

    // get list of host names

    // questions:
    //  how does a cast from a char pointer to a struct pointer work?
    //  how does it know that the last address will be NULL

    // printf("size of in_addr = %d\n", sizeof(struct in_addr));
    // size of in_addr = 4


int main()
{    
    WSADATA wsa;
    
    char *hostname = "www.google.com";
    struct hostent *host;

    int wsaerr = WSAStartup(MAKEWORD(2, 0), &wsa);
    if (wsaerr)
        exit(err());

    host = gethostbyname(hostname);
    if (!host)
        exit(err());

    struct in_addr** addresses;
    addresses = (struct in_addr**) host->h_addr_list;

    for (int i = 0; addresses[i] != NULL; i++)
        printf("%s\n", inet_ntoa(*addresses[i]));

    WSACleanup();
    return 0;
}

int err()
{
    fprintf(stderr, "%s\n", WSAGetLastError());
    return 1;
}
