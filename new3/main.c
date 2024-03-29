/*
	compile:
		clang main.c -o main -lws2_32 -liphlpapi

*/

/*
	get available ipv4 addresses

*/

#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <stdio.h>

// #pragma comment(lib, "iphlpapi.lib")
// #pragma comment(lib, "ws2_32.lib")

#define MALLOC(x) HeapAlloc(GetProcessHeap(), 0, (x))
#define FREE(x) HeapFree(GetProcessHeap(), 0, (x))

/* Note: could also use malloc() and free() */

int main()
{

	int i;
	
	/* Variables used by GetIpAddrTable */
	PMIB_IPADDRTABLE pIPAddrTable;
	DWORD dwSize = 0;
	DWORD dwRetVal = 0;
	IN_ADDR IPAddr;

	/* Variables used to return error message */
	LPVOID lpMsgBuf;

	// Before calling AddIPAddress we use GetIpAddrTable to get
	// an adapter to which we can add the IP.
	pIPAddrTable = (MIB_IPADDRTABLE *) MALLOC(sizeof (MIB_IPADDRTABLE));

	if (pIPAddrTable) {
		// Make an initial call to GetIpAddrTable to get the
		// necessary size into the dwSize variable
		if (GetIpAddrTable(pIPAddrTable, &dwSize, 0) == ERROR_INSUFFICIENT_BUFFER) {
			FREE(pIPAddrTable);
			pIPAddrTable = (MIB_IPADDRTABLE *) MALLOC(dwSize);
		}
		if (pIPAddrTable == NULL) {
			printf("Memory allocation failed for GetIpAddrTable\n");
			exit(1);
		}
	}
	// Make a second call to GetIpAddrTable to get the
	// actual data we want
	if ((dwRetVal = GetIpAddrTable(pIPAddrTable, &dwSize, 0)) != NO_ERROR) { 
		printf("GetIpAddrTable failed with error %lu\n", dwRetVal);
		if (FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, dwRetVal, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),       // Default language
			(LPTSTR) & lpMsgBuf, 0, NULL)) 
			{
				printf("\tError: %s", lpMsgBuf);
				LocalFree(lpMsgBuf);
			}
		exit(1);
	}

	printf("\tNum Entries: %ld\n", pIPAddrTable->dwNumEntries);

	for (i = 0; i < (int)pIPAddrTable->dwNumEntries; i++) {
		printf("\n\tInterface Index[%d]:\t%ld\n", i, pIPAddrTable->table[i].dwIndex);
		IPAddr.S_un.S_addr = (u_long) pIPAddrTable->table[i].dwAddr;
		printf("\tIP Address[%d]:     \t%s\n", i, inet_ntoa(IPAddr));
		IPAddr.S_un.S_addr = (u_long) pIPAddrTable->table[i].dwMask;
		printf("\tSubnet Mask[%d]:    \t%s\n", i, inet_ntoa(IPAddr));
		IPAddr.S_un.S_addr = (u_long) pIPAddrTable->table[i].dwBCastAddr;
		printf("\tBroadCast[%d]:      \t%s (%ld%)\n", i, inet_ntoa(IPAddr), pIPAddrTable->table[i].dwBCastAddr);
		printf("\tReassembly size[%d]:\t%ld\n", i, pIPAddrTable->table[i].dwReasmSize);
		printf("\tType and State[%d]:", i);

		if (pIPAddrTable->table[i].wType & MIB_IPADDR_PRIMARY)
			printf("\tPrimary IP Address");
		if (pIPAddrTable->table[i].wType & MIB_IPADDR_DYNAMIC)
			printf("\tDynamic IP Address");
		if (pIPAddrTable->table[i].wType & MIB_IPADDR_DISCONNECTED)
			printf("\tAddress is on disconnected interface");
		if (pIPAddrTable->table[i].wType & MIB_IPADDR_DELETED)
			printf("\tAddress is being deleted");
		if (pIPAddrTable->table[i].wType & MIB_IPADDR_TRANSIENT)
			printf("\tTransient address");
		printf("\n");
	}

	if (pIPAddrTable) {
		FREE(pIPAddrTable);
		pIPAddrTable = NULL;
	}

	exit(0);
}