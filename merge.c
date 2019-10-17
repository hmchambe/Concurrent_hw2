/* ----------------------------------------------------------- */
/* NAME : Hunter Chambers                  User ID: 81276171   */
/* DUE DATE : 09/--/2019                                       */
/* PROGRAM ASSIGNMENT #2                                       */
/* FILE NAME : merge.c				   			               */
/* PROGRAM PURPOSE :                                           */
/* ----------------------------------------------------------- */


#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include  <sys/types.h>
#include  <sys/ipc.h>
#include  <sys/shm.h>
#include  <unistd.h>


struct  Memory {
	int a[8];
};


int  main(int argc, char *argv[])
{
    key_t         ShmKEY;
    int           ShmID, i;
    struct Memory *ShmPTR;
    int           edges[2] = {atoi(argv[1]), atoi(argv[2])};

    if (argc != 3) {                   /* check # of arguments      */
        printf("need 2 integer command line argument.\nThere are %d elements\n", argc);
        return 0;
    }
    ShmKEY = ftok("./", 'a');          /* construct a key           */
    if ((ShmID = shmget(ShmKEY, sizeof(struct Memory), IPC_CREAT|0666)) < 0) {
        printf("shmget() failed.\n");
        return 0;
    }
    printf("Merge: Shared memory obtained\n");
    ShmPTR = (struct Memory *) shmat(ShmID, NULL, 0);  /* attachit  */
    if ((long) ShmPTR == -1) {
        printf("shmat() failed.\n");   
        shmctl(ShmID, IPC_RMID, NULL); /* remove it if shmaT fails  */
        return 0;
    }

    for(i=edges[0]; i<edges[1]; i++)
    {
	printf("ShmPTR[%d] = %d\n", i, ShmPTR->a[i]);
    }
    printf("Merge: Shared memory attached\n");  /* everything ready*/
    printf("Merge: Shared memory key = %d\n", ShmKEY);
    printf("Merge: Shared memory ID  = %d\n", ShmID);
    printf("Merge: My PID = %d\n", getpid());

    printf("Edges: %d, %d\nShmPTR[0]: %d\n", edges[0], edges[1], ShmPTR->a[0]);


    shmctl(ShmID, IPC_RMID, NULL);     /* remove it                 */
    printf("Server: Shared memory removed\n");
    return 0;
}
