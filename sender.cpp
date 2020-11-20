#include <sys/shm.h>
#include <sys/msg.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "msg.h"    /* For the message struct */

/* The size of the shared memory segment */
#define SHARED_MEMORY_CHUNK_SIZE 1000

/* The ids for the shared memory segment and the message queue */
int shmid;
int msqid;

/* The pointer to the shared memory */
void* sharedMemPtr;

/**
 * Sets up the shared memory segment and message queue
 * @param shmid - the id of the allocated shared memory
 * @param msqid - the id of the allocated message queue
 */
void init(int& shmid, int& msqid, void*& sharedMemPtr)
{
	/* TODO:
    1. Create a file called keyfile.txt containing string "Hello world" (you may do
 	    so manually or from the code).
	2. Use ftok("keyfile.txt", 'a') in order to generate the key.
	3. Use will use this key in the TODO's below. Use the same key for the queue
	   and the shared memory segment. This also serves to illustrate the difference
 	   between the key and the id used in message queues and shared memory. The key is
	   like the file name and the id is like the file object.  Every System V object
	   on the system has a unique id, but different objects may have the same key.
	*/

	key_t key = ftok("keyfile.txt",'a');

	/* TODO: Get the id of the shared memory segment. The size of the segment must be SHARED_MEMORY_CHUNK_SIZE */
	shmid = shmget(IPC_PRIVATE, SHARED_MEMORY_CHUNK_SIZE, IPC_CREAT);
	/* TODO: Attach to the shared memory */
	shmat(shmid, sharedMemPtr, IPC_CREAT);


	/* TODO: Attach to the message queue */
	/* Store the IDs and the pointer to the shared memory region in the corresponding function parameters */
	msqid = msgget(key,0644);

}

/**
 * Performs the cleanup functions
 * @param sharedMemPtr - the pointer to the shared memory
 * @param shmid - the id of the shared memory segment
 * @param msqid - the id of the message queue
 */
void cleanUp(const int& shmid, const int& msqid, void* sharedMemPtr)
{
	/* TODO: Detach from shared memory */
	shmdt(sharedMemPtr);
}

/**
 * The main send function
 * @param fileName - the name of the file
 * @return - the number of bytes sent
 */
unsigned long sendFile(const char* fileName)
{

	/* A buffer to store message we will send to the receiver. */
	message sndMsg;

	/* A buffer to store message received from the receiver. */
	ackMessage rcvMsg;

	/* The number of bytes sent */
  unsigned long numBytesSent = 0;

	/* Open the file */
	FILE * fp =  fopen(fileName, "rb");

	/* Was the file open? */
	if(!fp)
	{
		perror("fopen");
		exit(-1);
	}

	/* Read the whole file */
	while(!feof(fp))
	{
		/* Read at most SHARED_MEMORY_CHUNK_SIZE from the file and
 		 * store them in shared memory.  fread() will return how many bytes it has
		 * actually read. This is important; the last chunk read may be less than
		 * SHARED_MEMORY_CHUNK_SIZE. Save the number of bytes that were actually
		 * read in numBytesSent to record how many bytes were actually send.
 		 */

		/* TODO: count the number of bytes sent. */
		numBytesSent = fread(&fileName,1,SHARED_MEMORY_CHUNK_SIZE,fp);

		/* TODO: Send a message to the receiver telling him that the data is ready
 		 * to be read (message of type SENDER_DATA_TYPE).
 		 */
		 if(msgsnd(msqid, &sndMsg, sndMsg.size, SENDER_DATA_TYPE) == -1)
		 {
			 perror("msgsnd");
			 exit(-1);
		 }

		/* TODO: Wait until the receiver sends us a message of type RECV_DONE_TYPE telling us
 		 * that he finished saving a chunk of memory.
 		 */
		 if(msgrcv(msqid, &rcvMsg, sizeof(rcvMsg) - sizeof(long), SENDER_DATA_TYPE, 0) == -1)
		 {
			 perror("msgrcv");
			 exit(1);
		 }
	}

	/** TODO: once we are out of the above loop, we have finished sending the file.
 	  * Lets tell the receiver that we have nothing more to send. We will do this by
 	  * sending a message of type SENDER_DATA_TYPE with size field set to 0.
	  */
		if(msgsnd(msqid, &sndMsg, 0, SENDER_DATA_TYPE) == -1)
		{
			perror("msgsnd");
			exit(-1);
		}


	/* Close the file */
	fclose(fp);

	return numBytesSent;
}

/**
 * Used to send the name of the file to the receiver
 * @param fileName - the name of the file to send
 */
void sendFileName(const char* fileName)
{
	/* Get the length of the file name */
	int fileNameSize = strlen(fileName);

	/* TODO: Make sure the file name does not exceed
	 * the maximum buffer size in the fileNameMsg
	 * struct. If exceeds, then terminate with an error.
	 */
	if(fileNameSize > MAX_FILE_NAME_SIZE)
	{
		perror("File name exceeds maximum size.");
		exit(-1);
	}

	/* TODO: Create an instance of the struct representing the message
	 * containing the name of the file.
	 */
	 fileNameMsg nameMsg;
	/* TODO: Set the message type FILE_NAME_TRANSFER_TYPE */
	strncpy(nameMsg.fileName,fileName,fileNameSize+1);

	nameMsg.mtype = FILE_NAME_TRANSFER_TYPE;
	/* TODO: Set the file name in the message */

	/* TODO: Send the message using msgsnd */
	if(msgsnd(msqid, &nameMsg,sizeof(fileNameSize) - sizeof(long), nameMsg.mtype) == -1)
	{
		perror("msgsnd");
		exit(-1);
	}

}

int main(int argc, char** argv)
{
	/* Check the command line arguments */
	if(argc < 2)
	{
		fprintf(stderr, "USAGE: %s <FILE NAME>\n", argv[0]);
		exit(-1);
	}

	/* Connect to shared memory and the message queue */
	init(shmid, msqid, sharedMemPtr);

	/* Send the name of the file */
        sendFileName(argv[1]);

	/* Send the file */
	fprintf(stderr, "The number of bytes sent is %lu\n", sendFile(argv[1]));

	/* Cleanup */
	cleanUp(shmid, msqid, sharedMemPtr);

	return 0;
}
