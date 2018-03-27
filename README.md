# Fitnite-Topology-Enumeration
Enumerates the Number of Topologies on a Set of N Members
Based on V. Krishnamurthy's paper "On the Number of Topologies on a Finite Set"
The American Mathematical Monthly, Vol 73 No2 (Feb, 1966), pp. 154-157

1.) Introduction

In 1972 I was taking an undergraduate Topology course and looking around for a
topic to write a paper on, and came across V. Krishnamurthy's 1966 paper.  It was
agreeably easy to understand and the algorithm he proved readily implementable in my
limited FORTRAN skills.  I wrote the paper and the program, end of story -- for 45 years.  
I am now retired and looking for something to occupy my time.  I bought the components 
and assembled a VR capable PC for my grandson to play with.  Now the  grandson is off 
to California looking for work in the gaming industry and I have a PC sitting in the basement ...

Krishnamurthy showed that for each topology on a finite set with N members, there is an
NxN matrix (a(i,j)) of zeros and ones with a(i,i)=1 for all i and with the following 
property: (*) For every ordered pair (R(i),R(j)) (i,j=1,2,...,n) of rows of a(i,j)
and for every index k, a(j,i)=1=a(i,k)=1  ==> a(j,k)=1.  

So to count the number of topologies on a finite set N, one need only to generate all
possible NxN matrices of zeros and ones with ones down the diagonal, and check for
(*).  Since there are 2^(n^2-n) different such NxN matrices we have that as an
upper bound for the number of topologies that may be defined on a set with n members.
the FORTRAN program I implemented to do this ran on the University IBM 360/50 and 
computed f(5)=6942. To do so required generating and testing 1048576 matrices which took about 
two hours run time.  

2.) Improvements to Krishnamurthy's Alogorithm

There are three ideas that, when combined, served to make my new program much more efficient.

2.1 Make use of N-1 x N-1 submatrices.  The N-1 x N-1 matrix obtained by deleting row N
    and column N from an N x N matrix satisfying (\*) also satisfies (\*).
    This means that if you have an N x N matrice satisfying (*), you can
    generate new N+1 x N+1 matrices satisfying (*) just by adding a new Nth row
    and a new Nth column and testing for (*).  We now have that 
    f(N) <= f(N-1)*2^(2N-2) as an upper bound since there are 2^(2N-2) ways to add a new
    row and new column to an N-1 X N-1 matrix with a diagonal of 1's.
    
2.2 The transpose of an N x N matrix satisfying (*) also satisfies (*). 
    This of course reduces by half the number of matrices to be tested for (*),
    but also makes it more efficient to compute the transpose of an N+1 x N+1 matrix
    when the transpose of the N x N submatrix is already known.  This also give a new
    upper bound f(N) <= f(N-1)*2^N.
 
2.3 The unique rows obtained by the inclusive OR of all possible combinations of rows in an
    N x N matrix satisfying (*) will constitute the set of valid N+1 (last) 
    rows in the N+1 x N+1 matrices satisfying (*) created from the previous N x N matrix.
    When used with idea 2.1 and 2.2 above, provides a way to reduce the number of last rows 
    to be tested.
  
3.) Representation of N x N matrices satisfying (*)

A natural way to represent the N x N matrices is as a list of integers where the first
integer holds row 0 and the last holds row N-1.  The columns of the matrices are the bits
of the integers where the high order bit is column N-1 and the low order bit is column 0.

Krishnamurthy also defined an condensed representation of a N x N matrix satisfying (*) 
called an N Basic Number which is obtained by deleteing the diagonal of ones from the rows of the
N x N matrix and then concatenating the rows into a single N*(N-1) bit number.

For example, a single topology from a set with 7 members is represented by a 42 bit number.
All 9535241 topologies on a set with seven members are stored in a file of 123105544 bytes.

4.)  Hardware and Software Envinronment

The source code is written in C and was developed on an Intel Core i5 6600K with 4 cores
running Ubuntu Linux 16.04.  

5.) Program Execution Modes

There are two modes of program execution and a variation of the second mode provides a way to
implement distributed processing on an arbitrarily large network.

5.1 Basic Execution Mode

    fte 8

This example computes the number of topologies on a set with 8 elements.  Execution proceeds by
starting with a set with one element and then creating a new 2x2 matrix by adding a second row 
and second column that is tested for (*).  If the new matrix satisfies (*) push 
the matrix (or rather a pointer to the matrix) on to the stack.  Each time a matrix is pushed onto 
the stack, increment a counter and continue until all possible 2x2 matrices have been tested and 
added to the stack if satisfying (*).  Continue by poping a 2x2 matrix off the stack
and create new 3x3 matrix by adding a third row and third column and testing for (*).
Continue until all 3x3 matrices have been created and tested for (*).  Repeat for 4x4
matrices, then 5x5, then 6x6m then 7x7 then 8x8.  Finish by printing the counters for each matrix
dimension.

        f( 2):               4     4.000
        f( 3):              29     7.250
        f( 4):             355    12.241
        f( 5):            6942    19.555
        f( 6):          209527    30.183
        f( 7):         9535241    45.508
        f( 8):       642779354    67.411
        Elapsed Time = 5 sec (0:00:05)
        
The third column in the table above is the ratio f(n)/f(n-1).
        
These results and all following results were obtained from a C language program fte.c running on an Intel 
Core i5 6600K with 4 cores running Ubuntu Linux.

The Basic Execution Mode only utilitizes a single thread and one core regardless of how many cores
are available.

To compute f(10) in Basic Execution Mode:

    fte 10
    f(10):   8977053873043   141.91
    Elapsed Time = 125005 sec (34:43:25)

5.2 Partitioned Mode

In Paritioned Mode, the results from a previous basic execution mode compuation are saved and used
in a manner that allows all of the cores available to be used concurrently.
    
    fte 8 nbasic5.txt

This example also computes the number of topologies on a set with 8 elements, but does so by
utilizing the 6942 5-basic numbers computed earlier.

Execution proceeds as follows:

Read a 5-basic number from the nbasic5.txt file and convert it to a 5x5 matrix.
Program execution is then the same as in 5.1 except that instead of starting with a 1x1
matrix we begin with a 5x5 matrix. Continue until all 6942 5-basic numbers have been
processed.

In this mode pThreads functionaltiy is used to process each 5-basic number in a seperate
thread and the threads are run concurrently on as many cores as are available.

Computing f(8) in Partitioned Mode requires about 2 seconds as opposed to 5 seconds in basic mode.
The difference is not more pronounced because of the overhead in converting the 5 basic numbers to
5x5 matrices. Computing f(10) using the nbasic5.txt file is more efficient because the overhead is
spread over a much larger computation effort.

    fte 10 nbasic5.txt
    f(10):   8977053873043   141.907
    Elapsed Time = 32531 sec (9:02:11)

5.3  Distributed Patitioned Mode

In Distributed Partitioned Mode, the results from a previous computation are also saved and used
as in the Paritioned Mode, except that the file containing the nbasic numbers is split into a 
number of subfiles and each subfile is distributed out to be processed on a networked computer.

    cat seed7.txt | process.sh 10
    
This example computes the number of topologies on 10 elements using the 9535241 7-basic
numbers computed in an earlier run.  In this case the 9535241 7-basic numbers were split
in 2000 seperate files (file names of which are in seed7.txt), each of which contains about 4768 
7-basic numbers.  Each of these 2000 separate files are distrubuted out to the 7 computers in my 
local area network where they are processed in Paritiioned Mode as described above.  As each one 
of the 2000 separate files is processed, the partial results are written back to the computer 
responsible for distributing the work.  After all 2000 files have been processed the partial results 
are recombined using a Perl script to give the the final total.
    
In this case, the head computer responsible for distributing the work load is the Intel Core I5
mentioned above, and the remaining six are various 6-8 year old boxes that I could find plus one
Raspberry Pi.  With this setup it required about 3 hours to compute f(10) as opposed to 34 hours
for Basic Mode and 9 hours for Partitioned Mode.

The seven nodes in my local area network are about as many as my wife will allow in the basement.
Seven is also about as many as I care to try to keep configured and up to date etc.

It would be better to have nodes that are similar in capability and with more cores.  The AMD 16
core Thread Rippers are enticing if I can find the time and money.  I did try to program my algorithm
to use the Nvidia 1070 graphics card that the VR games required, but without much success.  I did get
it to run but without significant run time improvement.  No doubt because I didn't know what I was doing.
I would enjoy hearing if anyone decides to try programming this algorithm on a GPU or in other
distributed environments.

Note that results up to f(17) have been published in .....  That is impressive but I don't know what
algorithm was used or what computing resources were available.   





     



