#include <stdio.h>
#include <stdlib.h>

int main()
{
	// Where to store the contentens read from the file
	char buffer[100];
	
	// Open the file
	FILE* fp = fopen("file.txt", "r");

	// Sanity checks
	if(!fp)
	{
		perror("fopen");
		exit(1);
	}

	// Read at most 100 bytes from the file represented by fp.
	// Store them in the buffer;
	int numRead = fread(buffer, 1, 100, fp);
	
	fprintf(stderr, "Buffer = %s\n", buffer);
	fprintf(stderr, "Read %d bytes\n", numRead);

	fclose(fp);	
	return 0;
}
