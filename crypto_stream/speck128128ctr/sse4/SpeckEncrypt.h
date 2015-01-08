/***********************************************************************************
 ** DISCLAIMER. THIS SOFTWARE WAS WRITTEN BY EMPLOYEES OF THE U.S.
 ** GOVERNMENT AS A PART OF THEIR OFFICIAL DUTIES AND, THEREFORE, IS NOT
 ** PROTECTED BY COPYRIGHT. THE U.S. GOVERNMENT MAKES NO WARRANTY, EITHER
 ** EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO ANY IMPLIED WARRANTIES
 ** OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE, REGARDING THIS SOFTWARE.
 ** THE U.S. GOVERNMENT FURTHER MAKES NO WARRANTY THAT THIS SOFTWARE WILL NOT
 ** INFRINGE ANY OTHER UNITED STATES OR FOREIGN PATENT OR OTHER
 ** INTELLECTUAL PROPERTY RIGHT. IN NO EVENT SHALL THE U.S. GOVERNMENT BE
 ** LIABLE TO ANYONE FOR COMPENSATORY, PUNITIVE, EXEMPLARY, SPECIAL,
 ** COLLATERAL, INCIDENTAL, CONSEQUENTIAL, OR ANY OTHER TYPE OF DAMAGES IN
 ** CONNECTION WITH OR ARISING OUT OF COPY OR USE OF THIS SOFTWARE.
 ***********************************************************************************/



#include "x86intrin.h"


#define u64 unsigned long long
#define u32 unsigned 
#define LCS _lrotl
#define RCS _lrotr

#define XOR _mm_xor_si128     
#define SHFL _mm_shuffle_epi8 
#define SET _mm_set_epi64x
#define STORE(ip,X) _mm_storeu_si128((__m128i *)(ip),X)
#define LOAD(ip) _mm_loadu_si128((__m128i *)(ip))
#define ADD _mm_add_epi64
#define SWAP _mm_set_epi64x(0x080f0e0d0c0b0a09LL,0x0007060504030201LL)
#define LOW _mm_unpacklo_epi64
#define HIGH _mm_unpackhi_epi64
#define SR _mm_srli_epi64
#define SL _mm_slli_epi64


#define ROL(X,r) (XOR(SL(X,r),SR(X,(64-(r)))))
#define R(X,Y,k) (X=XOR(ADD(SHFL(X,SWAP),Y),k), Y=XOR(ROL(Y,3),X)) 

#define Rx6(X,Y,k) (R(X[0],Y[0],k), R(X[1],Y[1],k), R(X[2],Y[2],k))
#define Rx4(X,Y,k)  (R(X[0],Y[0],k), R(X[1],Y[1],k))
#define Rx2(X,Y,k)  (R(X[0],Y[0],k))


#define Rx8(X,Y,k) (X[0]=SHFL(X[0],SWAP), X[0]=ADD(X[0],Y[0]), X[0]=XOR(X[0],k), \
		    X[1]=SHFL(X[1],SWAP), X[1]=ADD(X[1],Y[1]), X[1]=XOR(X[1],k), \
		    X[2]=SHFL(X[2],SWAP), X[2]=ADD(X[2],Y[2]), X[2]=XOR(X[2],k), \
		    X[3]=SHFL(X[3],SWAP), X[3]=ADD(X[3],Y[3]), X[3]=XOR(X[3],k), \
		    Z[0]=Y[0], Z[1]=Y[1], Z[2]=Y[2], Z[3]=Y[3],		\
		    Z[0]=SL(Z[0],3), Z[1]=SL(Z[1],3), Z[2]=SL(Z[2],3), Z[3]=SL(Z[3],3), \
		    Y[0]=SR(Y[0],61), Y[1]=SR(Y[1],61), Y[2]=SR(Y[2],61), Y[3]=SR(Y[3],61), \
		    Y[0]=XOR(Y[0],Z[0]), Y[1]=XOR(Y[1],Z[1]), Y[2]=XOR(Y[2],Z[2]), Y[3]=XOR(Y[3],Z[3]), \
		    Y[0]=XOR(X[0],Y[0]), Y[1]=XOR(X[1],Y[1]), Y[2]=XOR(X[2],Y[2]), Y[3]=XOR(X[3],Y[3]))

#define Ex8(X,Y,k) (Rx8(X,Y,k[0]),  Rx8(X,Y,k[1]),  Rx8(X,Y,k[2]),  Rx8(X,Y,k[3]),  Rx8(X,Y,k[4]),  Rx8(X,Y,k[5]),  Rx8(X,Y,k[6]),  Rx8(X,Y,k[7]), \
		    Rx8(X,Y,k[8]),  Rx8(X,Y,k[9]),  Rx8(X,Y,k[10]), Rx8(X,Y,k[11]), Rx8(X,Y,k[12]), Rx8(X,Y,k[13]), Rx8(X,Y,k[14]), Rx8(X,Y,k[15]), \
		    Rx8(X,Y,k[16]), Rx8(X,Y,k[17]), Rx8(X,Y,k[18]), Rx8(X,Y,k[19]), Rx8(X,Y,k[20]), Rx8(X,Y,k[21]), Rx8(X,Y,k[22]), Rx8(X,Y,k[23]), \
		    Rx8(X,Y,k[24]), Rx8(X,Y,k[25]), Rx8(X,Y,k[26]), Rx8(X,Y,k[27]), Rx8(X,Y,k[28]), Rx8(X,Y,k[29]), Rx8(X,Y,k[30]), Rx8(X,Y,k[31]))

#define Ex6(X,Y,k) (Rx6(X,Y,k[0]),  Rx6(X,Y,k[1]),  Rx6(X,Y,k[2]),  Rx6(X,Y,k[3]),  Rx6(X,Y,k[4]),  Rx6(X,Y,k[5]),  Rx6(X,Y,k[6]),  Rx6(X,Y,k[7]), \
		    Rx6(X,Y,k[8]),  Rx6(X,Y,k[9]),  Rx6(X,Y,k[10]), Rx6(X,Y,k[11]), Rx6(X,Y,k[12]), Rx6(X,Y,k[13]), Rx6(X,Y,k[14]), Rx6(X,Y,k[15]), \
		    Rx6(X,Y,k[16]), Rx6(X,Y,k[17]), Rx6(X,Y,k[18]), Rx6(X,Y,k[19]), Rx6(X,Y,k[20]), Rx6(X,Y,k[21]), Rx6(X,Y,k[22]), Rx6(X,Y,k[23]), \
		    Rx6(X,Y,k[24]), Rx6(X,Y,k[25]), Rx6(X,Y,k[26]), Rx6(X,Y,k[27]), Rx6(X,Y,k[28]), Rx6(X,Y,k[29]), Rx6(X,Y,k[30]), Rx6(X,Y,k[31]))

#define Ex4(X,Y,k) (Rx4(X,Y,k[0]),  Rx4(X,Y,k[1]),  Rx4(X,Y,k[2]),  Rx4(X,Y,k[3]),  Rx4(X,Y,k[4]),  Rx4(X,Y,k[5]),  Rx4(X,Y,k[6]),  Rx4(X,Y,k[7]), \
		    Rx4(X,Y,k[8]),  Rx4(X,Y,k[9]),  Rx4(X,Y,k[10]), Rx4(X,Y,k[11]), Rx4(X,Y,k[12]), Rx4(X,Y,k[13]), Rx4(X,Y,k[14]), Rx4(X,Y,k[15]), \
		    Rx4(X,Y,k[16]), Rx4(X,Y,k[17]), Rx4(X,Y,k[18]), Rx4(X,Y,k[19]), Rx4(X,Y,k[20]), Rx4(X,Y,k[21]), Rx4(X,Y,k[22]), Rx4(X,Y,k[23]), \
		    Rx4(X,Y,k[24]), Rx4(X,Y,k[25]), Rx4(X,Y,k[26]), Rx4(X,Y,k[27]), Rx4(X,Y,k[28]), Rx4(X,Y,k[29]), Rx4(X,Y,k[30]), Rx4(X,Y,k[31]))

#define Ex2(X,Y,k) (Rx2(X,Y,k[0]),  Rx2(X,Y,k[1]),  Rx2(X,Y,k[2]),  Rx2(X,Y,k[3]),  Rx2(X,Y,k[4]),  Rx2(X,Y,k[5]),  Rx2(X,Y,k[6]),  Rx2(X,Y,k[7]), \
		    Rx2(X,Y,k[8]),  Rx2(X,Y,k[9]),  Rx2(X,Y,k[10]), Rx2(X,Y,k[11]), Rx2(X,Y,k[12]), Rx2(X,Y,k[13]), Rx2(X,Y,k[14]), Rx2(X,Y,k[15]), \
		    Rx2(X,Y,k[16]), Rx2(X,Y,k[17]), Rx2(X,Y,k[18]), Rx2(X,Y,k[19]), Rx2(X,Y,k[20]), Rx2(X,Y,k[21]), Rx2(X,Y,k[22]), Rx2(X,Y,k[23]), \
		    Rx2(X,Y,k[24]), Rx2(X,Y,k[25]), Rx2(X,Y,k[26]), Rx2(X,Y,k[27]), Rx2(X,Y,k[28]), Rx2(X,Y,k[29]), Rx2(X,Y,k[30]), Rx2(X,Y,k[31]))


#define Rx1(x,y,k) (x[0]=RCS(x[0],8), x[0]+=y[0], x[0]^=k, y[0]=LCS(y[0],3), y[0]^=x[0])

#define Ex1(X,Y,k) (Rx1(X,Y,k[0]),  Rx1(X,Y,k[1]),  Rx1(X,Y,k[2]),  Rx1(X,Y,k[3]),  Rx1(X,Y,k[4]),  Rx1(X,Y,k[5]),  Rx1(X,Y,k[6]),  Rx1(X,Y,k[7]), \
		    Rx1(X,Y,k[8]),  Rx1(X,Y,k[9]),  Rx1(X,Y,k[10]), Rx1(X,Y,k[11]), Rx1(X,Y,k[12]), Rx1(X,Y,k[13]), Rx1(X,Y,k[14]), Rx1(X,Y,k[15]), \
		    Rx1(X,Y,k[16]), Rx1(X,Y,k[17]), Rx1(X,Y,k[18]), Rx1(X,Y,k[19]), Rx1(X,Y,k[20]), Rx1(X,Y,k[21]), Rx1(X,Y,k[22]), Rx1(X,Y,k[23]), \
		    Rx1(X,Y,k[24]), Rx1(X,Y,k[25]), Rx1(X,Y,k[26]), Rx1(X,Y,k[27]), Rx1(X,Y,k[28]), Rx1(X,Y,k[29]), Rx1(X,Y,k[30]), Rx1(X,Y,k[31]))

#define Rx1b(x,y,k) (x=RCS(x,8), x+=y, x^=k, y=LCS(y,3), y^=x)



#define RK(A,B,k,key,i) (k[i]=SET(A,A), key[i]=A, B=RCS(B,8), B+=A, B^=i, A=LCS(A,3), A^=B)

#define EK(A,B,k,key) (RK(A,B,k,key,0),  RK(A,B,k,key,1),  RK(A,B,k,key,2),  RK(A,B,k,key,3),  RK(A,B,k,key,4),  RK(A,B,k,key,5),  RK(A,B,k,key,6), \
		       RK(A,B,k,key,7),  RK(A,B,k,key,8),  RK(A,B,k,key,9),  RK(A,B,k,key,10), RK(A,B,k,key,11), RK(A,B,k,key,12), RK(A,B,k,key,13), \
		       RK(A,B,k,key,14), RK(A,B,k,key,15), RK(A,B,k,key,16), RK(A,B,k,key,17), RK(A,B,k,key,18), RK(A,B,k,key,19), RK(A,B,k,key,20), \
		       RK(A,B,k,key,21), RK(A,B,k,key,22), RK(A,B,k,key,23), RK(A,B,k,key,24), RK(A,B,k,key,25), RK(A,B,k,key,26), RK(A,B,k,key,27), \
		       RK(A,B,k,key,28), RK(A,B,k,key,29), RK(A,B,k,key,30), RK(A,B,k,key,31))
