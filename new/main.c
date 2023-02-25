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

// http response length
#define RESPONSE_LEN 8000



void clrstr(char string[], int size)
{
	int i;
	for (i = 0; i < size; i++) { string[i] = (char)0; }
}

void catfile(char* destination, char *filename)
{
	FILE *f = fopen(filename, "r");
	char line[100];

	// should check to make sure it doesn't go over length 8000
	// char responseData[8000];
	while (fgets(line, 100, f))
	{
		// strcat(responseData, line);
		strcat(destination, line);
	}
	// strcat(destination, responseData);
}

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
		printf("\n\nServer listening on http://%s:%s\n\n", hostBuffer, serviceBuffer);
	}
}

void parse(char* header)
{
	char* h = header;

	do {
		printf("%c\n", *h);
		h++;
	} while (*h != '/');
	
	printf("here\n%s\n", header);

}


int main(int argc , char *argv[])
{
	WSADATA wsa;
	int iResult;

	iResult = WSAStartup(MAKEWORD(2,2), &wsa);
	if (iResult) 
	{
		printf("WSAStartup failed: %d\n", iResult);
		return 1;
	}

	// get server/host address data
	struct addrinfo hints;
	struct addrinfo *result = NULL;

	ZeroMemory(&hints, sizeof(hints));

	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	iResult = getaddrinfo(NULL, PORT, &hints, &result);
	if (iResult != 0)
	{
		printf("getaddrinfo failed: %d\n", iResult);
		WSACleanup();
		return 1;
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
	
	report(result->ai_addr);
	SOCKET ClientSocket = INVALID_SOCKET;

	char httpResponse[RESPONSE_LEN];
	char recvbuf[DEFAULT_BUFLEN];
	int sendResult;
	int recvResult;

	char *pch;
	char *header;
	
	while (1)
	{
		if ((ClientSocket = accept(ListenSocket, NULL, NULL)) != INVALID_SOCKET) 
		{
			// accepted socket
			recvResult = recv(ClientSocket, recvbuf, DEFAULT_BUFLEN, 0);

			if (recvResult > 0)
			{
				// recieved request
				// printf("Bytes received = %d\n", recvResult);

				pch = strtok(recvbuf, "\n");

				parse(pch);

				printf("first line = \n%s\n", pch);

				header = "HTTP/1.1 200 OK\r\n\n";
				clrstr(httpResponse, RESPONSE_LEN);
				strcat(httpResponse, header);
				catfile(httpResponse, "index.html");

				sendResult = send(ClientSocket, httpResponse, strlen(httpResponse), 0);

				if (sendResult != SOCKET_ERROR)
				{
					// send response
					printf("Bytes sent = %d\n", sendResult);
					closesocket(ClientSocket);
				}
				else
				{
					// error: failed response
					printf("send failed: %d\n", WSAGetLastError());
					closesocket(ClientSocket);
					WSACleanup();
					return 1;				
				}
			}
			else if (recvResult == 0)
			{
				printf("Connection closing...\n");
			}
			else
			{
				// error: failed receive
				printf("recv failed: %d\n", WSAGetLastError());
				closesocket(ClientSocket);
				WSACleanup();
				return 1;
			}
		}
		else
		{
			// error: rejected socket
			printf("accept failed: %d\n", WSAGetLastError());
			closesocket(ListenSocket);
			WSACleanup();
			return 1;
		}
	}

	// free(httpResponse);

	// shutdown the send half of the connection since no more data will be sent
	if ((shutdown(ClientSocket, SD_SEND)) == SOCKET_ERROR) 
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

