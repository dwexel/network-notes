#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MAX_IDs 100
#define ID_LENGTH 8
#define BUFLEN 9

int main()
{
	// id max length = 8
	char char1[MAX_IDs];
	char char2[MAX_IDs];
	char char3[MAX_IDs];
	char char4[MAX_IDs];
	char char5[MAX_IDs];
	char char6[MAX_IDs];
	char char7[MAX_IDs];
	char char8[MAX_IDs];

	char* storage[] = {char1, char2, char3, char4, char5, char6, char7, char8};
	int stored = 0;
	char line[BUFLEN];

	int c;
	int len;
	int w;
	char ch;

	// enter words
	puts("enter word:");
	while (1) {
		printf("> ");
		gets(line);
		len = strlen(line);
		if (!len) break;

		for (c = 0; c < ID_LENGTH; c++) {
			
			if (c >= len) {
				ch = '\0';
			} else {
				ch = line[c];
			}

			storage[c][stored] = ch;

		}
		stored += 1;
	}
	if (stored == 0) return 0;

	// print stored
	printf("number of ids = %d\n", stored);
	for (w = 0; w < stored; w++) {
		for (c = 0; c < ID_LENGTH; c++) {
			ch = storage[c][w];
			if (ch != '\0') {
				printf("%c", ch);
			}
		}
		printf("\n");
	}

	// compare words
	puts("enter word to compare:");
	printf("> ");
	gets(line);
	len = strlen(line);
	if (!len) return 0;

	char e;
	char s;
	// int b;

	for (w = 0; w < stored; w++) {
		for (c = 0; c < ID_LENGTH; c++) {
			e = line[c];
			s = storage[c][w];

			if (c >= len) e  = '-';
			if (s == '\0') s = '-';
			printf("entered = %c, stored = %c, same = %d\n", e, s, e == s);
		}
		printf("\n");
	}

	return 0;
}