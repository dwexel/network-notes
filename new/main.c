/*
	Live Server on port 8888
*/

#include<io.h>
#include<stdio.h>
#include<conio.h>
#include<string.h>

#include<ws2tcpip.h>
#include<winsock2.h>

#define PORT "8888"
#define DEFAULT_BUFLEN 8000

// http response length
#define RESPONSE_LEN 8000


// enum response_types {
// 	html, js
// };


int filesize(FILE *f) {
	fseek(f, 0, SEEK_END);
	int size = ftell(f);
	rewind(f);
	return size;
}

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
		printf("\nServer listening on http://%s:%s\n", hostBuffer, serviceBuffer);
	}
}

int main(int argc , char *argv[]) {
	WSADATA wsa;
	int iResult;

	iResult = WSAStartup(MAKEWORD(2,2), &wsa);
	if (iResult) {
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

	// get address info for 0.0.0.0:8888
	iResult = getaddrinfo(NULL, PORT, &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo failed: %d\n", iResult);
		WSACleanup();
		return 1;
	}
	
	// Create a SOCKET for the server to listen for client connections
	SOCKET ListenSocket = INVALID_SOCKET;
	if ((ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol)) == INVALID_SOCKET) {
		printf("Error at socket(): %d\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}
	
	// Setup the TCP listening socket
	if (bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen) == SOCKET_ERROR) {
		printf("bind failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(result);
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}

	// im not sure why but it was in the tutorial
	freeaddrinfo(result);

	// listen for incoming connections
	if (listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR) {
		printf("Listen failed with error: %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}
	
	report(result->ai_addr);
	SOCKET ClientSocket = INVALID_SOCKET;

	// setup responses
	char httpResponse[RESPONSE_LEN];
	char recvbuf[DEFAULT_BUFLEN];
	int sendResult;
	int recvResult;

	while (1){
		if ((ClientSocket = accept(ListenSocket, NULL, NULL)) != INVALID_SOCKET) {

			recvResult = recv(ClientSocket, recvbuf, DEFAULT_BUFLEN, 0);

			if (recvResult > 0) {

				// recieved request
				printf("------------request:\n%s\n", recvbuf);
				
				char *uri;
				char path[50];
				char *ext;

				uri = strtok(recvbuf, "\n");
				uri = strtok(uri, " ");
				uri = strtok(NULL, " ");

				// if the request is asking for the root, get ready to return "index.html"
				if (!strcmp(uri, "/")) {
					strcpy(path, "index.html");
				}

				// if the request is for a different file, get ready to return whatever it is
				else if(uri[0] == '/') {
					path[0] = '.';
					path[1] = '\0';
					strcat(path, uri);
				}

				// get the file extension of the requested file
				// this isn't really used atm
				ext = strrchr(path, '.');


				FILE *f = fopen(path, "r");
				memset(httpResponse, (char)0, RESPONSE_LEN);

				if (f) {
					// server found the file
					char *target;
					
					target = httpResponse;
					target += sprintf(target, "%s", "HTTP/1.1 200 OK\r\n");

					// well...
					if (!strcmp(ext, ".html")) {
						target += sprintf(target, "Content-Type: text/html\r\n");
					}

					target += sprintf(target, "Content-Security-Policy: %s\r\n\r\n", "script-src 'self'");

					char line[100];
					while (fgets(line, 100, f)) {
						strcat(httpResponse, line);
					}
					fclose(f);
	
				}
				else {
					// file not found
					strcat(httpResponse, "HTTP/1.1 404 Not Found\r\n");
					strcat(httpResponse, "Content-Type: text/plain\r\n\r\n");
					strcat(httpResponse, "404 not found\r\nfile:");
					strcat(httpResponse, path);	
				}
		
				printf("------------response:\n");
				printf("%s\n", httpResponse);

				sendResult = send(ClientSocket, httpResponse, strlen(httpResponse), 0);

				if (sendResult != SOCKET_ERROR) {
					// sent response successfully
					printf("------bytes sent: %d\n", sendResult);
					closesocket(ClientSocket);
				}
				else {
					// error: failed response
					printf("error: send failed: %d\n", WSAGetLastError());
					closesocket(ClientSocket);
					WSACleanup();
					return 1;				
				}
			}
			else if (recvResult == 0) {
				// socket closed by client
				printf("connection closing...\n");
				closesocket(ClientSocket);
			}
			else {
				// error: failed receive
				printf("error: receive failed: %d\n", WSAGetLastError());
				closesocket(ClientSocket);
				WSACleanup();
				return 1;
			}
		}
		else
		{
			// error: rejected socket
			printf("error: accept failed: %d\n", WSAGetLastError());
			closesocket(ListenSocket);
			WSACleanup();
			return 1;
		}
	}

	// wait, this will never be reached.
	// shutdown the send half of the connection since no more data will be sent
	// if ((shutdown(ClientSocket, SD_SEND)) == SOCKET_ERROR) 
	// {
	// 	printf("shutdown failed: %d\n", WSAGetLastError());
	// 	closesocket(ClientSocket);
	// 	WSACleanup();
	// 	return 1;
	// }
	// closesocket(ClientSocket);
	// WSACleanup();
	// return 0;
}

