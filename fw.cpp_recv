#include <stdio.h>
#include <stdlib.h>

int main()
{
	// Where to store the contentens read from the file
	char buffer[] = "Hello fwrite()!";
	
	// Open the file
	FILE* fp = fopen("file.txt", "w");

	// Sanity checks
	if(!fp)
	{
		perror("fopen");
		exit(1);
	}

	// Write 16 bytes stored in the buffer into
	// the file represented by fp
	int numWritten = fwrite(buffer, 1, 16, fp);
	
	fprintf(stderr, "Buffer = %s\n", buffer);
	fprintf(stderr, "Written %d bytes\n", numWritten);

	// Close the file
	fclose(fp);
	
	return 0;
}
