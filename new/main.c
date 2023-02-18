/*
	Live Server on port 8888
*/
#include<io.h>
#include<stdio.h>
#include<conio.h>

#include<ws2tcpip.h>
#include<winsock2.h>

#define PORT "8888"

#define DEFAULT_BUFLEN 512


int err();
void setHttpHeader(char httpHeader[]);

// void report(struct sockaddr_in *serverAddress);

void report(struct sockaddr *serverAddress)
{
   char hostBuffer[INET6_ADDRSTRLEN];
   char serviceBuffer[NI_MAXSERV];

	int err = getnameinfo(
		serverAddress, (socklen_t)sizeof(*serverAddress),
		hostBuffer,    sizeof(hostBuffer), 
		serviceBuffer, sizeof(serviceBuffer),
		NI_NUMERICHOST
	);

   if (err != 0) 
	{
        printf("It's not working!!\n");
   } 
	else 
	{
	    printf("\n\nServer listening on http://%s:%s\n", hostBuffer, serviceBuffer);
	}
}



int main(int argc , char *argv[])
{
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2,2), &wsa) != 0)
	{
		exit(err());
	}
	
	// get server/host address data
	struct addrinfo hints;
	struct addrinfo *result = NULL;

	ZeroMemory(&hints, sizeof(hints));

	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	if (getaddrinfo(NULL, PORT, &hints, &result) != 0)
	{
		exit(err());
	}
		
	// Create a SOCKET for the server to listen for client connections
	SOCKET ListenSocket = INVALID_SOCKET;
	if ((ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol)) == INVALID_SOCKET)
	{
		printf("Error at socket(): %d\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}
	
	// Setup the TCP listening socket
   if (bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen) == SOCKET_ERROR)
	{
        printf("bind failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
   }

	freeaddrinfo(result);

	// listen for incoming connections
	if (listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR) 
	{
		printf("Listen failed with error: %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}	

	// Accept a client socket
	SOCKET ClientSocket = INVALID_SOCKET;
	if ((ClientSocket = accept(ListenSocket, NULL, NULL)) == INVALID_SOCKET) 
	{
		printf("accept failed: %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}
	
	char httpHeader[8000] = "HTTP/1.1 200 OK\r\n\n";
	setHttpHeader(httpHeader);
	report(result->ai_addr);

	// input char
	unsigned char a;

	char recvbuf[DEFAULT_BUFLEN];
	int sendResult;
	int recieveResult;
	// int recvbuflen = DEFAULT_BUFLEN;

	do {
		sendResult = recv(ClientSocket, recvbuf, DEFAULT_BUFLEN, 0);
		if (sendResult > 0) 
		{
			printf("Bytes received: %d\n\n", sendResult);
			// printf("recvbuf = %s", recvbuf);
			// printf(httpHeader);

			recieveResult = send(ClientSocket, httpHeader, sizeof(httpHeader), 0);
			if (recieveResult == SOCKET_ERROR) 
			{
				printf("send failed: %d\n", WSAGetLastError());
				closesocket(ClientSocket);
				WSACleanup();
				return 1;
			}
			// printf("Bytes sent: %d\n", recieveResult);


		}
		else if (sendResult == 0)
		{
			printf("Connection closing...\n");
		}
		else
		{
			printf("recv failed: %d\n", WSAGetLastError());
			closesocket(ClientSocket);
			WSACleanup();
			return 1;
		}

		printf("enter e to exit\n");
		a = getchar();
		if (a == 101) break;

	} while (sendResult > 0);



	// shutdown the send half of the connection since no more data will be sent
	int iResult = shutdown(ClientSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) 
	{
		printf("shutdown failed: %d\n", WSAGetLastError());
		closesocket(ClientSocket);
		WSACleanup();
		return 1;
	}

	closesocket(ClientSocket);
	WSACleanup();
	return 0;
}

int err()
{
	fprintf(stderr, "error = %d", WSAGetLastError());
	WSACleanup();
	return 1;
}

void setHttpHeader(char httpHeader[])
{
	printf("here");
	FILE *htmlData = fopen("index.html", "r");

	char line[100];
	char responseData[8000];
	while (fgets(line, 100, htmlData) != 0) 
	{
		printf(line);
		strcat(responseData, line);
	}

	// strcat(httpHeader, " ");
	strcat(httpHeader, responseData);
}


// void report(struct sockaddr_in *serverAddress)
// {
//     char hostBuffer[INET6_ADDRSTRLEN];
//     char serviceBuffer[NI_MAXSERV];
	
//     socklen_t addr_len = (socklen_t) sizeof(*serverAddress);

//     int err = getnameinfo(
//         (struct sockaddr *) serverAddress, addr_len,
//         hostBuffer, sizeof(hostBuffer),
//         serviceBuffer, sizeof(serviceBuffer),
//         NI_NUMERICHOST
//     );

//     if (err != 0) {
//         printf("It's not working!!\n");
//     }

//     printf("\n\n\tServer listening on http://%s:%s\n", hostBuffer, serviceBuffer);
// }