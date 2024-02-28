#include<io.h>
#include<stdio.h>
#include<conio.h>
#include<string.h>

#include<ws2tcpip.h>
#include<winsock2.h>

#define PORT "8888"

/*
	compile:
		clang main.c -o main -lws2_32

*/

/*
	get address info then quit

*/


void report(struct sockaddr *serverAddress) {
   char hostBuffer[INET6_ADDRSTRLEN];
   char serviceBuffer[NI_MAXSERV];

	int err = getnameinfo(
		serverAddress, (socklen_t)sizeof(*serverAddress),
		hostBuffer,    sizeof(hostBuffer), 
		serviceBuffer, sizeof(serviceBuffer),
		NI_NUMERICHOST
	);

	if (err != 0) {
		printf("It's not working!!\n");
	} 
	else	{
		printf("network interface is open:\n%s:%s\n", hostBuffer, serviceBuffer);
	}
}


int main() {
	printf("hello world\n");


	// using windows
	WSADATA wsa;
	int iResult;

	iResult = WSAStartup(MAKEWORD(2,2), &wsa);

	if (iResult) {
		printf("WSAStartup failed: %d\n", iResult);
		return 1;
	}

	// ya
	struct addrinfo hints;
	struct addrinfo *result = NULL;

	ZeroMemory(&hints, sizeof(hints));

	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	iResult = getaddrinfo(NULL, PORT, &hints, &result);

	if (iResult != 0) {
		// this means the port is occupied probs
		printf("getaddrinfo failed: %d\n", iResult);
		WSACleanup();
		return 1;
	}


	printf("%s\n", result->ai_addr->sa_data);
	report(result->ai_addr);



	return 0;
}