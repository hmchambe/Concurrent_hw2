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
	pid_t 			pid = getpid();
	int			ShmID, i;
	int			edges[2] = {0, N - 1};
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
	printf("Share2d memory key = %ld\n", ShmKEY);
	printf("Shared memory ID = %ld\n", ShmID);
	printf("My PID = %ld\n", pid);

	
	
	if(fork() > 0)
		wait(NULL);
	else
		execvp("./merge", edges);


}

