// Simple example of use string: strlen(), strcmp(), strcat(), strcpy(), strchr(), strstr()

#include <stdio.h>
#include <string.h>

int main(void) {
	char s1[80], s2[80];

	printf("Enter string 1 (S1): ");
	scanf("%s", s1);
	printf("Enter string 2 (S2): ");
	scanf("%s", s2);

	printf("Lenght S1: %d\n", (int)strlen(s1));
	printf("Lenght S2: %d\n", (int)strlen(s2));

	if (!strcmp(s1, s2))
		printf("S1 and S2 are iguals.\n");

	strcat(s1, s2);
	printf("Concatenation: %s\n", s1);

	strcpy(s1, "New S1 value");
	printf("S1: %s\n", s1);
	strcpy(s2, "New S2 value");
	printf("S2: %s\n", s2);

	if (strchr(s1, 'v'))
		printf("Char [v] found in [%s]\n", s1);

	if (strstr(s2, "S2"))
		printf("String [S2] found in [%s]\n", s2);

	return 0;
}
