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
All 9535241 topologies on a set with seven members is stored in a file of 123105544 bytes.

5.) 


     



