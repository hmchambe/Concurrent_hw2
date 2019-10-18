/* ----------------------------------------------------------- */
/* NAME : Hunter Chambers                  User ID: 81276171   */
/* DUE DATE : 09/--/2019                                       */
/* PROGRAM ASSIGNMENT #2                                       */
/* FILE NAME : main.c				               */
/* PROGRAM PURPOSE :                                           */
/* ----------------------------------------------------------- */


#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>

void display(int *a, int size)
{
	int i;
	char buf[100];
	sprintf(buf, "Input array for mergesort has %d elements\n", size);
	write(1, buf, strlen(buf));

	for(i=0; i<size; i++)
	{
		sprintf(buf, "   %d   ", a[i]);
		write(1, buf, strlen(buf));
	}
		sprintf(buf, "\n");
		write(1, buf, strlen(buf));
}

int main(int argc, char *argv[])
{
	key_t 			ShmKEY;
	pid_t 			pid = getpid(), forkPID;
	int				ShmID, i;
	char			*upperBound;
	int			    size;
	char			buf[300];
	int 			*ShmPTR;

	scanf("%d", &size);
	printf("size -> %d\n", size);



	int correctSize = snprintf(NULL, 0, "%d", size);
	char *shortBuf = malloc( correctSize + 1);
	char *upperBoundBuf = malloc( correctSize + 1);
	snprintf(shortBuf, correctSize+1, "%d", size);
	snprintf(upperBoundBuf, correctSize+1, "%d", size-1);
	char *merge[] = {"./merge", "0", upperBoundBuf, shortBuf, NULL};
    ShmKEY = ftok("./", 'a');
	if ((ShmID = shmget(ShmKEY, sizeof(int)*size, IPC_CREAT | 0666)) < 0) {
		int error = errno;
		printf("shmid = %d\nshmget() failed.\nerrno: %d\n", ShmID, error);
	    	return 0;
	}

	sprintf(buf, "Merge Sort with Multiple Processes:\n");
	write(1, buf, strlen(buf));
	ShmPTR = shmat(ShmID, NULL, 0);
	if(ShmPTR == (void*)-1)
	{
		printf("shmat() failed.\n");
		shmctl(ShmID, IPC_RMID, NULL);	/* remove it if shmat() fails */
		return 0;
	}

	sprintf(buf, "*** MAIN: Shared memory key = %d\n*** MAIN: shared memory created\n*** MAIN: Shared memory attached and is ready to use\n", ShmKEY);
	write(1, buf, strlen(buf));

	/* read in array and store in ShmPTR */
	for(i=0; i<size; i++)
	{
		scanf("%d", &ShmPTR[i]);
	}

	display(ShmPTR, size);
	sprintf(buf, "*** MAIN: about to spawn the merge sort process\n");
	write(1, buf, strlen(buf));

	if((forkPID = fork()) == 0)
	{ /*  CHILD execs merge  */
		if(execvp(merge[0], merge) < 0)
		{
			printf("execvp() failed\nErrno: %d", errno);
			exit(1);
		}
	}else if(forkPID < 0)
	{
		printf("FORK FAILED\n");
		exit(1);
	}


	/* PARENT waits for child */

	wait(NULL);
	display(ShmPTR,size);
    shmdt((void *) ShmPTR);
/* TODO Check all mallocs and make sure they are freed */
	shmctl(ShmID, IPC_RMID, NULL); /* removes shared memory when done */
	free(shortBuf);
	free(upperBoundBuf);
	sprintf(buf, "*** MAIN: shared memory successfully detached\n*** MAIN: shared memory successfully removed\n*** MAIN: exits\n");
	write(1, buf, strlen(buf));
	exit(0);

}

