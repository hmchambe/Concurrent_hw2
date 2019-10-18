/* ----------------------------------------------------------- */
/* NAME : Hunter Chambers                  User ID: 81276171   */
/* DUE DATE : 09/--/2019                                       */
/* PROGRAM ASSIGNMENT #2                                       */
/* FILE NAME : merge.c				   			               */
/* PROGRAM PURPOSE :                                           */
/*  Perform mergesort on an array concurrently using a         */
/*  modified binary search method for inserting values         */ 
/* ----------------------------------------------------------- */


#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>

struct  Memory {
	int a[8];
};

void swap(int *a, int x, int y)
{
	int tempNum;
	tempNum = a[x];
	a[x] = a[y];
	a[y] = tempNum;
}


int  main(int argc, char *argv[])
{
	char          buf[300];
    key_t         ShmKEY;
    int           ShmID, i, m;
    int           *ShmPTR;
    int           upperBound = atoi(argv[2]);
	int			  lowerBound = atoi(argv[1]);
    int           size = atoi(argv[3]);
    char          upperBoundChar[5], lowerBoundChar[5];

    pid_t         firstForkVal, secondForkVal;
    printf("Hello%d\n", upperBound);

	/* 		check # of arguments      */
    if (argc != 4) {                   
        printf("need 3 integer command line argument.\nThere are %d elements\n", argc);
        return 0;
    }

	/* 		construct a key     */
    ShmKEY = ftok("./", 'a');          
    if ((ShmID = shmget(ShmKEY, sizeof(int), 0666)) < 0) {
        printf("shmget() failed.\n");
        return 0;
    }
	
	printf("Merge: Shared memory obtained\n");
    ShmPTR = shmat(ShmID, NULL, 0);  /*  attach it  */
    
	/*		check shared memory attach 		*/
	if (ShmPTR == (void*)-1) {
        printf("shmat() failed.\n");   
        shmctl(ShmID, IPC_RMID, NULL); /* remove it if shmat fails  */
        return 0;
    }
/* NOW WE HAVE SHARED MEMORY ARRAY */

	if(upperBound - lowerBound == 1)
	{
		if(ShmPTR[lowerBound] > ShmPTR[upperBound])
		{
            printf("MADE IT\n");
			swap(ShmPTR, upperBound, lowerBound);
            return;
		}
	}
	/*		printing initialization info	*/
	sprintf(buf, "   ### M-PROC(%d): entering with a[%d..%d]\n", getpid(), lowerBound, upperBound);
	write(1, buf, strlen(buf));
    /* array has more than two entries */
    m = (lowerBound + upperBound)/2;

    if((firstForkVal = fork()) > 0) /* parent process */
    {  
        if((secondForkVal = fork()) > 0) /* parent process */
        {   /* wait for child processes */
            wait(NULL);
            wait(NULL);
        }else if(secondForkVal == 0) /* right child process */
        {   /* gets [m..upperBound-1] */
/* TODO: fix these snprintf's like in main.c */
            snprintf(upperBoundChar, sizeof(upperBoundChar), "%d", upperBound-1);
            snprintf(lowerBoundChar, sizeof(lowerBoundChar), "%d", m);
/* TODO:    See if this can be changed so it is not creating it for every child */
	        char *mergeCall[] = {"./merge", lowerBoundChar, upperBoundChar, NULL};
            execvp(mergeCall[0], mergeCall);
        }
    }else if(firstForkVal == 0) /* left child process */
    {       /* gets [lowerBound..m-1] */
/* TODO:    make this into a function */
        snprintf(upperBoundChar, sizeof(upperBoundChar), "%d", m-1);
        snprintf(lowerBoundChar, sizeof(lowerBoundChar), "%d", lowerBound);
	    char *merge[] = {"./merge", lowerBoundChar, upperBoundChar, NULL};
        execvp(merge[0], merge);
    }

    

/*	print shared memory array	
    for(i=edges[0]; i<edges[1]; i++)
    {
	sprintf(buf, "ShmPTR[%d] = %d\n", i, ShmPTR->a[i]);
	write(1, buf, strlen(buf));
    }
*/
	
	/*	remove shared memory when finished	*/
    shmctl(ShmID, IPC_RMID, NULL); 
    printf("Merge: Shared memory removed\n");
    return 0;
}
