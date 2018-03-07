# Fitnite-Topology-Enumeration
Enumerates The Number of Topologies on Set of N Members
Based on V. Krishnamurthy's paper "On the Number of Topologies on a Finite Set"
The American Mathematical Monthly, Vol 73 No2 (Feb, 1966), pp. 154-157

1.) Introduction

In 1972 I was taking an undergraduate Topology course and looking around for a
topic to write a paper on, and came accross V. Krishnamurthy's 1966 paper.  It was
agreeably easy to understand and the algorithm he proved readily implementable in my
limited FORTRAN skills.  I wrote the paper and the program, end of story -- for 45 years.  
I am now a rather ancient retired software developer looking for something to occupy my time.  
I bought the components and assembled a VR capable PC for my grandson to play with.  Now the 
grandson is off to California looking for work in the gaming industry and I have a PC 
sitting in the basement...

Krishnamurthy showed that for each topology on a finite set with N members, there is an
NxN matrix (a(i,j)) of zeros and ones with a(i,i)=1 for all i and with the following 
property: (*) For every ordered pair (R(i),R(j)) (i,j=1,2,...,n) of rows of a(i,j)
and for every index k, a(j,i)=1=a(i,k)=1  ==> a(j,k)=1.  

So to count the number of topologies on a finite set N, one need only to generate all
possible NxN matrices of zeros and ones with ones down the diagonal, and check for
property (*).  The FORTRAN program I implemented to do this ran on the University IBM 360/50 and 
computed f(5)=6942. To do so required generating and testing 1048576 matrices which took about 
two hours run time.  To compute f(6) would require generating and testing for property (*) 
1024 times as many matrices.

2.) Improvements to Krishnamurthy's Alogorithm

There are three ideas, that when combined, served to make my new program much more efficient.

2.1 Make use of N-1 x N-1 submatrices.  The N-1 x N-1 matrix obtained by deleting row N
    and column N from an N x N matrix satisfying property * also satisfies property *.
    This means that if you have an N x N matrice satisfying property star, you can
    generate new N+1 x N+1 matrices satisfying property *  just by adding a new Nth row
    and an new Nth column and testing for poperty *.

2.2 The unique rows obtained by the pairwise inclusive OR of all possible rows of an
    N x N matrix satisfying property (*) will constitute the set of valid N+1 (last) 
    rows in the N+1 x N+1 matrices satisfying property * created from the previous N x N matrix.
    When used with idea 1 above, provides a way to reduce the number of last rows to be tested.
   
2.3 The transpose of an N x N matrice satisfying property * also satisfies property *. 
    This of course reduces by half the number of matrices to be tested for property *,
    but also makes it more efficient to compute the transpose of an N+1 x N+1 matrice
    when the transpose of the N x N submatrix is already known.
    
4.) Representation of N x N matricres satisfying property (*)

A natural way to represent the N x N matrices is as a list of integers where the first
integer holds row 0 and the last holds row N-1.  The columns of the matrices are the bits
of the integers where the high order bit is column N-1 and the low order bit is column 0.

Krishnamurthy also defined a condensed representation of a N x N matrice satisfying property
(*) called an N Basic Number obtained by deleteing the diagonal of ones from the rows of the
N x N matrice and then concatenating the rows into a single N*(N-1) bit number.

For example, a single topology from a set with 7 members is represented by a 42 bit number.
All 9535241 topologies on a set with seven members are stored in a file of 123105544 bytes.

5.) Program Execution Modes

There are two modes of program execution and a variation of the second mode provides a way to
implement distributed processing on an arbitrarily large network.

5.1 Basic Execution Mode

fte 8

This example computes the number of topologies on a set with 8 elements.  Execution proceeds by
starting with a set with one element and then creating a new 2x2 matrix that is tested for 
Property (*).  If the new matrice satisfyies property *

Execution proceeds as follows:

5.1.1  Contstuct a 1x1 matrix representing the single topoology on a set with one member.
This is just a single integer containing the number 1.

5.1.2  Add a second row and second column to the 1x1 matrix and test for property (*)
if the resulting 2x2 matrix satisfysiesproperty *, add it to a ??????? tree

...............

The Basic Execution Mode only utilitize a single thread regardless of how many cores
are available.


5.2 Submatrix Mode

fte 8 nbasic5.txt

This example also computes the number of topologies on a set with 8 elements, but does so by
utilizing the 6942 5 basic numbers computed earlier.

Execution proceeds as follows:

5.2.1 Read a 5 basic number frome the nbasic5.txt file and convert it to a 5x5 matrix.
Program execution is then the same as in 5.1 except that instead of starting with a 1x1
matrix we begin with a 5x5 matrix

5.2.2 Continue until all 5 basic numbers have been processed.

In this mode ptThreads functionaltiy is used to process each 5 basic number in a seperate
thread and the threads are run concurrently on as many cores as are available.

5.3  Distributed Processing Mode

cat seed7.txt | process.sh 10








     



