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
#include <errno.h>
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


void convertToChar( int lowerBound, char **lowerBoundChar, int upperBound, char **upperBoundChar, int size, char **sizeChar)
{
    snprintf(*sizeChar, sizeof(upperBoundChar)+1, "%d", size);
    snprintf(*upperBoundChar, sizeof(upperBoundChar)+1, "%d", upperBound);
    snprintf(*lowerBoundChar, sizeof(lowerBoundChar)+1, "%d", lowerBound);
}

int binarySearch(int *a, int lowerBound, int upperBound, int val)
{
    if((upperBound-lowerBound) == 1)
    {
        return upperBound;
    }
    int m = lowerBound+(upperBound-lowerBound+1)/2;

    if(a[m] < val)
    {
        lowerBound = a[m];
        binarySearch(a, lowerBound, upperBound, val);
    }
    
    if(a[m] > val)
    {
        upperBound = a[m];
        binarySearch(a, lowerBound, upperBound, val);
    }
}

void merge(int *ShmPTR, int size, int leftLowerBound, int leftUpperBound, int rightLowerBound, int rightUpperBound)
{ /* upperBound and lower are indices */
    int i, j;
    int *tmpArray = malloc(sizeof(int)*size);
    int *left = malloc(sizeof(int)*(size/2));
    int *right = malloc(sizeof(int)*(size/2));
    for(i=0; i<size/2; i++)
    {
        left[i] = ShmPTR[leftLowerBound+i];
        right[i] = ShmPTR[rightLowerBound+i];
    }

/* TODO fork processes for every binarySearch */
    for(i=0; i<size/2; i++)
    {
        if(left[i] < right[0])
        {
            tmpArray[i] = left[i];
        }else if(left[i] > right[(size/2)-1])
        {
            tmpArray[i+(size/2)] = left[i];
        }else
        {
            int k = binarySearch(right, 0, (size/2)-1, left[i]);
            tmpArray[i+k] = left[i];
        }

        if(right[i] < left[0])
        {
            tmpArray[i] = right[i];
        }else if(right[i] > left[(size/2)-1])
        {
            tmpArray[i+(size/2)] = right[i];
        }else
        {
            int k = binarySearch(left, 0, (size/2)-1, right[i]);
            tmpArray[i+k] = right[i];
        }
    }

    free(left);
    free(right);

    for(i=0; i<size; i++)
    {   /* copy value back to shared mem */
        ShmPTR[leftLowerBound+i] = tmpArray[i];
    }
    free(tmpArray);
    return;
}

int  main(int argc, char *argv[])
{
	char          buf[50];
    key_t         ShmKEY;
    int           ShmID, i, m;
    int           *ShmPTR;
    int           upperBound = atoi(argv[2]);
	int			  lowerBound = atoi(argv[1]);
    int           size = atoi(argv[3]);
  /*  char          upperBoundChar[5], lowerBoundChar[5]; */

    pid_t         firstForkVal, secondForkVal;


	/* 		check # of arguments      */
    if (argc != 4) {                   
        printf("need 3 integer command line argument.\nThere are %d elements\n", argc);
        return 0;
    }

	/* 		construct a key     */
    ShmKEY = ftok("./", 'a');          
    if ((ShmID = shmget(ShmKEY, sizeof(int), 0666)) < 0) {
        sprintf(buf, "shmget() failed.\nErrno %d\n", errno);
        write(1, buf, strlen(buf));
        return 0;
    }
	
	/*printf("Merge: Shared memory obtained\n");*/
    ShmPTR = shmat(ShmID, NULL, 0);  /*  attach it  */
    
	/*		check shared memory attach 		*/
	if (ShmPTR == (void*)-1) {
        printf("shmat() failed.\n");   
        shmctl(ShmID, IPC_RMID, NULL); /* remove it if shmat fails  */
        return;
    }
/* NOW WE HAVE SHARED MEMORY ARRAY */

	if(upperBound - lowerBound == 1)
	{
		if(ShmPTR[lowerBound] > ShmPTR[upperBound])
		{
			swap(ShmPTR, upperBound, lowerBound);
            return;
		}else
        {
            return;
        }
	}

	/*		printing initialization info	*/
	sprintf(buf, "   ### M-PROC(%d): entering with a[%d..%d]\n", getpid(), lowerBound, upperBound);
	write(1, buf, strlen(buf));




    /* array has more than two entries */
    m = (lowerBound + upperBound + 1)/2;
    int correctBufSize = snprintf(NULL, 0, "%d", upperBound);
    char *upperBoundChar = malloc(correctBufSize+1);
    char *lowerBoundChar = malloc(correctBufSize+1);
    char *sizeChar = malloc(correctBufSize+1);

    if((firstForkVal = fork()) > 0) /* parent process */
    {  
        if((secondForkVal = fork()) > 0) /* parent process */
        {   /* wait for child processes */
            wait(NULL);
            wait(NULL);
        /* left bounds: [lowerBound, m-1]; right bounds: [m, upperBound] */
            merge(ShmPTR, size, lowerBound, m-1, m, upperBound);
        }else if(secondForkVal == 0) /* right child process */
        {   /* gets [m..upperBound] */
            size = size/2;
            lowerBound = m;
            convertToChar(lowerBound, &lowerBoundChar, upperBound, &upperBoundChar, size, &sizeChar);
            if(fork() == 0)
            {
	            char *mergeCall[] = {"./merge", lowerBoundChar, upperBoundChar, sizeChar, NULL};
                execvp(mergeCall[0], mergeCall);
                return;
            }
            wait(NULL);
        }
    }else if(firstForkVal == 0) /* left child process */
    {       /* gets [lowerBound..m-1] */

        size = size/2;
        upperBound = m-1;
        convertToChar(lowerBound, &lowerBoundChar, upperBound, &upperBoundChar, size, &sizeChar);
        if(fork() == 0)
        {
	        char *mergeCall[] = {"./merge", lowerBoundChar, upperBoundChar, sizeChar, NULL};
            execvp(mergeCall[0], mergeCall);
            return;
        }
        wait(NULL);
    }

    


/*	print shared memory array	
    for(i=edges[0]; i<edges[1]; i++)
    {
	sprintf(buf, "ShmPTR[%d] = %d\n", i, ShmPTR->a[i]);
	write(1, buf, strlen(buf));
    }
*/
	/*	remove shared memory when finished	*/
    /*printf("ShmID: %d\n", ShmID);*/


    shmdt(ShmPTR);
    free(upperBoundChar);
    free(lowerBoundChar);
    free(sizeChar);

    /*printf("Merge: Shared memory removed\n");*/
    return;
}
