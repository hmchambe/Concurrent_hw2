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
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>

#define N 8

struct Memory {
	int a[8];
};

int main(int argc, char *argv[])
{
	key_t 			ShmKEY;
	pid_t 			pid = getpid(), forkPID;
	int			ShmID, i;
	char *merge[] = {"./merge", "0", "7", NULL};
	struct Memory *ShmPTR;
	int a[N] = {9, 8, 7, 6, 5, 4, 3, 2};

    ShmKEY = ftok("./", 'a');

	if ((ShmID = shmget(ShmKEY, sizeof(struct Memory), IPC_CREAT | 0666)) < 0) {
		int error = errno;
		printf("shmid = %d\nshmget() failed.\nerrno: %d\n", ShmID, error);
	    	return 0;
	}

	printf("Shared memory obtained\n");
	ShmPTR = (struct Memory *) shmat(ShmID, NULL, 0);
	if((int) ShmPTR == -1)
	{
		printf("shmat() failed.\n");
		shmctl(ShmID, IPC_RMID, NULL);	/* remove it if shmat() fails */
		return 0;
	}

	for(i=0; i<N; i++)
	{
		ShmPTR->a[i] = a[i];
		/* printf("a[%d]: %d\nShmPTR a[%d]: %d\n", i, a[i], i, (ShmPTR->a[i])); */
	}

	printf("Shared memory attached\n");
	printf("Shared memory key = %d\n", ShmKEY);
	printf("Shared memory ID = %d\n", ShmID);
	printf("My PID = %d\n", pid);



	if((forkPID = fork()) == 0)
	{ /*  CHILD execs merge  */
		if(execvp(merge[0], merge) < 0)
		{
			printf("execvp() failed\nErrno: %d", errno);
			exit(1);
		}
	}else if(forkPID > 0)
	{ /* PARENT waits for child */
		wait(NULL);
		exit(0);
	}else
	{
		printf("FORK FAILED\n");
		exit(1);
	}
}

