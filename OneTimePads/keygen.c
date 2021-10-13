#include <stdio.h>
#include <stdlib.h>
#include <time.h>


int main(int argc, char* argv[]) {

	srand(time(NULL));
	int length = atoi(argv[1]);
	char alpha[27] = { 'A', 'B', 'C', 'D', 'E', 'F', 'G',
						  'H', 'I', 'J', 'K', 'L', 'M', 'N',
						  'O', 'P', 'Q', 'R', 'S', 'T', 'U',
						  'V', 'W', 'X', 'Y', 'Z', ' ' };
	char key[length];
	for (int i = 0; i < length; i++)
	{

		key[i] = alpha[rand() % 27];

	}
	key[length] = '\0';

	printf("%s\n", key);

	return 0;

}

