		
#define GPU    1
#define MACRO  1
#define STATS  0

#define UI unsigned int
#define ULLI unsigned long long int

#define maxElements 12
#define NOTNBASIC 0
#define NBASIC 1
#define NOTRANSPOSE -1
#define SYMMETRIC 1
#define LESSTHAN  2
#define GREATERTHAN 3
#define TRUE 1
#define FALSE 0


int assistedTranspose(int dim, UI tpp[], UI tp_t[], UI tpp_t[]);

void transpose(int seedDim, UI xp[], UI yp[]);

void convertNbasicNumberToMatrix(int dim, ULLI nb, UI array[]);

ULLI convertMatrixToNbasicNumber(int dim, UI array[]);

int assembleLastRows(int dim, UI xp[], UI xLrows[], UI yp[], UI yLrows[]);

void printTally(void);

void printDateTime(void);

struct tnode
{
     int dim;
     UI xp[maxElements];
     UI yp[maxElements];
     struct tnode *prev;
     int index;
};

struct parms
{
     int seedDim;
     int targetDim;
};

void *threadHandler(struct parms *tp);

void countTopologies(int seedDim, UI xp[], UI yp[], int targetDim,
		 ULLI tauTally[]);

struct tnode *pushNode(struct tnode *p, int dim, UI *xp, UI *yp);

struct tnode *popNode(struct tnode **p);

void freenode(register struct tnode *p);


