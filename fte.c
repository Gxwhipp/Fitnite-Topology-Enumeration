#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include "fte.h"

pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
int counter = 0;

int sCnt;
ULLI *seedList;
int targetDim;

ULLI hostTally[maxElements+1] = {0};
time_t t1,t2;
char SEEDFILE[255];
char *RESULTSFILE = "/var/log/topologyCounts.log";
int main(int argc, char *argv[]) 
{
	FILE *fd = NULL;
	int seedDim, seedCount;

	t1 = time(0);
	printDateTime();

	if (!(argc == 2 || argc == 3))
	{
		printf("Invalid command line argument count.\n");
		exit(1);
	}

	targetDim = atoi(argv[1]);
	if (targetDim < 2 || targetDim > maxElements)
	{
		printf("Number of set members entered must be >= 2 and <= %d\n",
				maxElements);
		exit(1);
	}
	printf("Number of set members = %d\n", targetDim);

	if (argc == 2)
	{
		// No nbasic number file specified.  Set default to one set element
		// with the corresponding nBasic number = 1
		strcpy(SEEDFILE, "--");
		sCnt = seedCount = 1;
		if((seedList = (ULLI *)malloc(seedCount*sizeof(ULLI))) == NULL)
		{
			printf("malloc failed line %d\n", __LINE__);
		}
		seedList[0] = 1;
		seedDim = 1;
	}
	else if (argc == 3)
	{
		printf("N basic number file = %s\n", argv[2]);
		if((fd = fopen(argv[2], "r")) == NULL)
		{
			printf("Can't open file %s\n", argv[2]);
			exit(1);
		}
		strcpy(SEEDFILE, argv[2]);
		if (fscanf(fd, "%d %d", &seedDim, &seedCount) == EOF)
		{
			printf("Error reading first line of file %s\n", argv[2]);
			exit(1);
		}
		printf("Seed Dimension = %d, N Basic Number seed Count = %d\n",
				   seedDim, seedCount);
		if(seedDim >= targetDim)
		{
			printf("seed Dimension from input file must be less than %d\n"
						"(number of set members from command line)",
						targetDim);
			exit(1);
		}

		if((seedList = (ULLI *)malloc(seedCount*sizeof(ULLI))) == NULL)
		{
			printf("malloc failed line %d\n", __LINE__);
		}

		sCnt = 0;
		ULLI seedNb;
		while(fscanf(fd, "%llu", &seedNb) == 1)
		{
			/*printf("nbasic number = %llu\n", seedNb);*/
			if(sCnt < seedCount)
			{
				seedList[sCnt] = seedNb;
			}
			sCnt++;
		}
		fclose(fd);

		if (sCnt != seedCount)
		{
			printf("Count of nbasic numbers in seed file incorrect.\n"
						"Expected count = %d, actual count = %d\n",
						seedCount, sCnt);
			exit(1);
		}

	}    


	struct parms tp;
	tp.seedDim = seedDim;
	tp.targetDim = targetDim;

	int threadCount = sysconf(_SC_NPROCESSORS_CONF);
	/* if a seed file wasn't specified, only one thread is used */
	printf("Available Threads: %d\n", threadCount);
	if (argc == 2)
		threadCount = 1;
	printf("Threads Used: %d\n", threadCount);

	pthread_t thread[threadCount];
	for (int i = 0; i < threadCount; i++)
		pthread_create(&thread[i], NULL, (void *)&threadHandler, &tp);

	 // Wait for all threads to finish
	for (int i = 0; i < threadCount; i++)
		pthread_join(thread[i], NULL);

	free(seedList);
	printTally();

	char hostName[255];
	fd = NULL;

	t2 = time(0);
	double millions  = hostTally[targetDim]/1000000.;
	int seconds = (int)(t2-t1);
	double tpm = seconds/millions;
	int hr = seconds/3600;
	int min = (seconds - hr*3600)/60;
	int sec = seconds - hr*3600 - min*60;

	int gethostname(char *name, size_t len);
	gethostname(hostName, sizeof(hostName));

	printf("> %2d %llu %s %s %d(%d:%02d:%02d) %10.7f\n",
		targetDim, hostTally[targetDim], SEEDFILE, hostName, seconds, 
			hr, min, sec, tpm);
	exit(0);
}

void *threadHandler(struct parms *tp)
{

	extern ULLI hostTally[];
	ULLI tauTally[maxElements+1];
	UI xp[maxElements], yp[maxElements];
	int seedDim = tp->seedDim;
	int targetDim = tp->targetDim;

	pthread_mutex_lock(&mutex1);
	counter++;
	/*printf("Counter value: %d\n",counter);*/

	while (sCnt-- > 0) 
	{

		int k = sCnt;
		pthread_mutex_unlock(&mutex1);
	
		/*printf(" >>> Job starting: %d, %llu\n", k, seedList[k]);*/
		convertNbasicNumberToMatrix(seedDim, seedList[k], xp);
		transpose(seedDim, xp, yp);

		for (int i=0; i<=targetDim; i++)
		{
			tauTally[i] = 0;
		}
		countTopologies(seedDim, xp, yp, targetDim, tauTally);
	
		/*printf(" >>> Job done: %d, %llu\n", k, seedList[k]);*/
	
		pthread_mutex_lock(&mutex1);
		for (int i = seedDim; i<=targetDim; i++)
		{
			hostTally[i] += tauTally[i];
		}
	}
	pthread_mutex_unlock(&mutex1);
	
	return NULL;

}

struct tnode *pushNode(struct tnode *p, int dim, UI *xp, UI *yp)
{
	struct tnode *k;

	if ((k = (struct tnode *)malloc(sizeof(struct tnode))) == NULL)
	{
		printf("malloc failed\n");
		exit(1);
	}

	for (int i=0; i<dim; i++)
	{
		k->xp[i] = xp[i];
		k->yp[i] = yp[i];
	}

	k->dim = dim;
	k->prev = p;

	return(k);
}

struct tnode *popNode(struct tnode **p)
{
	struct tnode *k = NULL;
	if (*p != NULL)
	{
		k = *p;
		*p = (*p)->prev;
	}
	return (k);
}



void freenode(struct tnode *p)
{
	free(p);
	return;
}


void countTopologies(int seedDim, UI xSeed[], UI ySeed[], int targetDim,
		 ULLI tauTally[])
{
	UI xpp[maxElements];  
	UI ypp[maxElements];  
	UI xppt[maxElements]; 
	UI xppt_t[maxElements];
	UI xLastRows[1<<(maxElements-1)];
	UI yLastRows[1<<(maxElements-1)];

	UI *xp, *yp;

	/* initialize stack */
	tauTally[seedDim]++;
	struct tnode *preStack = pushNode(NULL, seedDim, xSeed, ySeed);
	struct tnode *nodep;
	while((nodep = popNode(&preStack)) != NULL)
	{
		int dim = nodep->dim; 
		xp = nodep->xp;
		yp = nodep->yp;
          
		/* xLastRows is the unique set rows generated by the inclusive */
          /* or of all possible combinations of the rows in the xp NxN matrix */
		/* Same for yLastRows and yp.  Note that yp is the tanspose of xp */
          /* note that the counts of xLastRows and yLastRows are the same */
		int lrCount = assembleLastRows(dim, xp, xLastRows, yp, yLastRows);

		for (int i = 0; i < lrCount; i++)
		{
			/* Converts an NxN last row to an N+1xN+1 last row */
			/* Shfit left one bit and add 1, the diagonal bit  */
			xLastRows[i] = xLastRows[i] + xLastRows[i] + 1;
			yLastRows[i] = yLastRows[i] + yLastRows[i] + 1;
		}
		
		/* Shift the NxN matrix one bit to left to make N+1xN+1 matrix */
		for (int row = 0; row < dim; row++)
		{
			/* shift the rows left one bit */
		     xpp[row] = xp[row] + xp[row];
		     ypp[row] = yp[row] + yp[row];
		}

		freenode(nodep);

		int nextDim = dim+1;
		ypp[nextDim-1] = 1;
		/*int rval = -999;*/
		for (int x = 0; x < lrCount; x++)
		{
			xpp[nextDim-1] = xLastRows[x];
			/* Compute transpose of xpp using ypp for efficiency */
			int rval = assistedTranspose(nextDim, xpp, ypp, xppt);
			if (rval == LESSTHAN)
			{       

				/* This loop is preprocessing external to following loop */
				/* for efficiency */
				UI combo = ~0;
				for (int j=0; j<dim; j++)
				{
					if (xppt[j] & 1)
						combo = combo & xppt[j];
				}


				for (int y = 0; y < lrCount; y++)
				{       
					/* test for property (*) */
					if((combo & yLastRows[y]) != yLastRows[y])
						continue;


					xppt[nextDim-1] = yLastRows[y];
					int rval=assistedTranspose(nextDim, xppt, xpp, xppt_t);

					if (rval == GREATERTHAN)
					{
						tauTally[nextDim] += 2;
						if (nextDim < targetDim)
						{
							 preStack = pushNode(preStack, nextDim, 
									xppt, xppt_t);
							 preStack = pushNode(preStack, nextDim, 
									xppt_t, xppt); 
						}
					}
					else if (rval == SYMMETRIC)
					{
						tauTally[nextDim]++;
						if(nextDim < targetDim)
						{
							preStack = pushNode(preStack, nextDim, 
								xppt, xppt); 
						}
					}
				} 
			}
			else if (rval == SYMMETRIC)
			{
				tauTally[nextDim]++;
				if (nextDim < targetDim)
				{
					 preStack = pushNode(preStack, nextDim, 
						  xpp, xpp); 
				}
			}

		}
	}
	/*
	for (int i = 0; i<targetDim+1; i++)
		printf("AA %d %llu\n", i, tauTally[i]);
	*/
	return;
}


inline int  assistedTranspose(int dim, UI tpp[], UI ypp[], UI tpp_t[])
{

/*

	ypp was the transpose of tpp before the last row of tpp was changed
     and this can be used to compute the transpose of tpp more efficiently

	return values:
	  SYMMETRIC   if input matrix is symmetric
	  LESSTHAN    if input matrix "less than" the transpose of the 
				  input matrix.
	  GREATERTHAN if input Matrix "greater than" the transpose of 
				  the input matrix.


	  Note:  Given an Nbasic matrix it will either be "less than",
		    "greater than" or symmetric (equal to) to its transpose. 

*/

	UI c = (tpp[dim-1])>>1;


	memcpy(tpp_t, ypp, maxElements<<(sizeof(UI)>>1));
	int i = dim - 2;
	while ( c )
	{
		tpp_t[i--] |= (c & 1);
		c >>= 1;
	}
	


	int rval = SYMMETRIC;
	for (int i = 0; i<dim; i++)
	{
		if(tpp[i] > tpp_t[i])
		{
			rval = GREATERTHAN;
			break;
		}
	
		else if(tpp[i] < tpp_t[i])
		{
			rval = LESSTHAN;
			break;
		}
	}
	return(rval);
}

int assembleLastRows(int dim, UI xMatrix[], UI xLastRows[],
					 UI yMatrix[], UI yLastRows[])
{

/*
	Given an input nxn nbasic matrix, computes all the valid n+1 rows of
	for a new (n+1)x(n+1) nbasic matrix.  The new rows are computed
	by finding the "inclusive OR"  combinaions of all the input rows in
	the input nxn matrix.
 
	Return Value:  the number of valid bottom rows 


*/
	unsigned char sentinal[1<<dim];
	memset(sentinal, 1, sizeof(sentinal));
	UI *xp = xMatrix;
	UI *yp = yMatrix;


	int cnt = 0;
	for (int i = 0; i<dim; i++)
	{
		if (sentinal[xp[i]])
		{
			xLastRows[cnt] = xp[i];
			yLastRows[cnt] = yp[i];
			cnt++;
			sentinal[xp[i]] = 0;
		}
	}

	xp = xLastRows;
	yp = yLastRows;
	dim = cnt;

	for (int i=0; i<dim-1; i++) /* note the dim-1 */
	{
		UI qx = xp[i];
		UI qy = yp[i];
		int newCnt = cnt;
		for (int j=0; j<cnt; j++)
		{                               
			UI nx = xLastRows[j] | qx;  
			if(sentinal[nx])        
			{               
				xLastRows[newCnt] = nx;
				yLastRows[newCnt] = yLastRows[j] | qy;
				newCnt++;                   
				sentinal[nx] = 0;       
		   }                   
	   }

	   cnt = newCnt;
	}

	xLastRows[cnt] = 0;
	yLastRows[cnt] = 0;

	cnt++;

	return (cnt);
}

void printTally()
{
	extern time_t t1, t2;
	extern int targetDim;
	extern ULLI hostTally[];

	printDateTime();
	t2 = time(0);
	for (int dim=2; dim <= targetDim; dim++)
	{
		char s[20];
		strcpy(s, " ");
		if(hostTally[dim-1] > 0)
		{ 
			double ratio = (double)hostTally[dim]/(double)hostTally[dim-1];
			sprintf(s, "%9.3f", ratio);
		}
		printf("f(%2d): %15llu %s\n",
				dim, hostTally[dim], s);

	}

	int seconds = (int)(t2-t1);
	int hr = seconds/3600;
	int min = (seconds - hr*3600)/60;
	int sec = seconds - hr*3600 - min*60;

	printf("Elapsed Time = %d sec (%d:%02d:%02d)\n", seconds, hr, min, sec);
	double millions  = hostTally[targetDim]/1000000.;
	double tpm = seconds/millions;
	printf("Time per million topologies: %10.7f\n", tpm);

	return;
}

/* converts an n-basic number to an nxn basic matrix */
/* Note this only works for from n=1 to n=8 since a 8-basic */
/* is 56 bits and for n=9 or greater is more the 64 bit ULLI size */
void convertNbasicNumberToMatrix(int dim, ULLI nb, UI array[])
{

	UI mask = ((~0) >> (dim - 1)) << (dim -1);
	mask = ~mask;
	for (int k = 0; k<dim; k++)
	{    
		UI rowBits = nb & mask;  /* extract the rightmost dim - 1 bits */
		UI leftBits = rowBits >> k;  
		UI rightBits = (leftBits << k) ^ rowBits; 
		array[dim-1-k] = (leftBits << (k+1)) | (1 << k) | rightBits;
		nb = nb >> (dim -1);
	}

	return;
}

/* converts an n-basic matrix to an n-basic number */
ULLI convertMatrixToNbasicNumber(int dim, UI array[])
{   
	ULLI  nb = 0;
	ULLI leftBits, rightBits;

	for (int k=0; k<dim; k++)
	{

		leftBits =  (array[k] >> (dim-k-1)) << (dim-k-1);
		rightBits = leftBits ^ array[k];
		leftBits = (leftBits >> (dim-k)) << (dim-k-1);
		nb = (nb << (dim-1ULL)) | leftBits | rightBits;
	}

	if (nb == 0)
	{
		nb = 1ULL << (dim*(dim-1ULL));
	}

	return(nb);
}

void printDateTime()
{   
	time_t t = time(NULL);
	struct tm *tm = localtime(&t);
	char s[64]; 
	strftime(s, sizeof(s), "%c", tm);
		printf("%s\n", s);
}

/* Computes the transpose of an nxn n-basic matrix */
void transpose(int dim, UI nbynMatrix[], UI tp[])
{
	int  r, col;
	for (int i = 0; i < dim; i++)
	{
		tp[i] = 0;
	}

	for (int i = 0; i<dim; i++)
	{
		r = nbynMatrix[i];

		col = dim-1;


		tp[col] <<= 1;
		tp[col] += (r & (1)); 
		while (col > 0)
		{
			r >>= 1; 
			col--;
			tp[col] <<= 1;
			tp[col] += (r & (1)); 
		}

	}

	return;
}
