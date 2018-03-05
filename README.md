# Fitnite-Topology-Enumeration
Enumerates The Number of Topologies on Set of N Members
Based on V. Krishnamurthy's paper "On the Number of Topologies on a Finite Set"
The American Mathematical Monthly, Vol 73 No2 (Feb, 1966), pp. 154-157

In 1972 I was taking an undergraduate Topology course and looking around for a
topic to write a paper on, and came accross V. Krishnamurthy's 1966 paper.  It was
agreeably easy to understand and the algorithm he proved readily implementable in my
limited FORTRAN skills.  I wrote the paper and the program, end of story -- for 45 years.  
I was now a rather ancient former software developer looking for something to occupy my time.  
I bought the components and assembled a VR capable PC for my grandson to play with.  Now the 
grandson is off to California looking for work and I have a PC sitting in the basement...

Krishnamurthy showed that for each topology on a finite set with N members, there is an
NxN matrix (a(i,j)) of zeros and ones with a(i,i)=1 for all i and with the following 
property:  (*) For every ordered pair (R(i), R(j)) (i,j=1,2,...,n) of rows of a(i,j)
and for every idex k, a(j,i)=1=a(i,k)=1  ==> a(j,k)=1.  

So to count the number of topologies on a finite set N, one need only to generate all
possible NxN matrices of zeros and ones with ones down the diagonal, and check for
property (*).  The FORTRAN program ran on an IBM 360/50 and computed f(5)=6942. To do so
required generating and testing 1048576 matrices which took about two hours run time.

