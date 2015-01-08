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
#define LCS _lrotl
#define RCS _lrotr

#define XOR _mm256_xor_si256     
#define SHFL _mm256_shuffle_epi8
#define ADD _mm256_add_epi64
#define SL _mm256_slli_epi64
#define SR _mm256_srli_epi64
#define SWAP SET(0x080f0e0d0c0b0a09LL,0x0007060504030201LL,0x080f0e0d0c0b0a09LL,0x0007060504030201LL)

#define LOAD(ip) _mm256_loadu_si256((__m256i *)(ip))
#define STORE(ip,X) _mm256_storeu_si256((__m256i *)(ip),X)
#define SET _mm256_set_epi64x
#define LOW _mm256_unpacklo_epi64
#define HIGH _mm256_unpackhi_epi64





#define Rx20(X,Y,k) (X[0]=SHFL(X[0],SWAP), X[0]=ADD(X[0],Y[0]), X[0]=XOR(X[0],k),\
                     X[1]=SHFL(X[1],SWAP), X[1]=ADD(X[1],Y[1]), X[1]=XOR(X[1],k),\
                     X[2]=SHFL(X[2],SWAP), X[2]=ADD(X[2],Y[2]), X[2]=XOR(X[2],k),\
                     X[3]=SHFL(X[3],SWAP), X[3]=ADD(X[3],Y[3]), X[3]=XOR(X[3],k),\
                     X[4]=SHFL(X[4],SWAP), X[4]=ADD(X[4],Y[4]), X[4]=XOR(X[4],k),\
		     Z[0]=Y[0], Z[1]=Y[1], Z[2]=Y[2], Z[3]=Y[3], Z[4]=Y[4],\
		     Z[0]=SL(Z[0],3), Z[1]=SL(Z[1],3), Z[2]=SL(Z[2],3), Z[3]=SL(Z[3],3), Z[4]=SL(Z[4],3),\
		     Y[0]=SR(Y[0],61), Y[1]=SR(Y[1],61), Y[2]=SR(Y[2],61), Y[3]=SR(Y[3],61), Y[4]=SR(Y[4],61),\
		     Y[0]=XOR(Y[0],Z[0]), Y[1]=XOR(Y[1],Z[1]), Y[2]=XOR(Y[2],Z[2]), Y[3]=XOR(Y[3],Z[3]), Y[4]=XOR(Y[4],Z[4]),\
		     Y[0]=XOR(X[0],Y[0]), Y[1]=XOR(X[1],Y[1]), Y[2]=XOR(X[2],Y[2]), Y[3]=XOR(X[3],Y[3]), Y[4]=XOR(X[4],Y[4]))

#define Ex20(X,Y,k) (Rx20(X,Y,k[0]), Rx20(X,Y,k[1]), Rx20(X,Y,k[2]), Rx20(X,Y,k[3]), Rx20(X,Y,k[4]), Rx20(X,Y,k[5]), Rx20(X,Y,k[6]), Rx20(X,Y,k[7]),\
		     Rx20(X,Y,k[8]), Rx20(X,Y,k[9]), Rx20(X,Y,k[10]), Rx20(X,Y,k[11]), Rx20(X,Y,k[12]), Rx20(X,Y,k[13]), Rx20(X,Y,k[14]), Rx20(X,Y,k[15]), \
		     Rx20(X,Y,k[16]), Rx20(X,Y,k[17]), Rx20(X,Y,k[18]), Rx20(X,Y,k[19]), Rx20(X,Y,k[20]), Rx20(X,Y,k[21]), Rx20(X,Y,k[22]), Rx20(X,Y,k[23]), \
		     Rx20(X,Y,k[24]), Rx20(X,Y,k[25]), Rx20(X,Y,k[26]), Rx20(X,Y,k[27]), Rx20(X,Y,k[28]), Rx20(X,Y,k[29]), Rx20(X,Y,k[30]), Rx20(X,Y,k[31]),\
		     Rx20(X,Y,k[32]), Rx20(X,Y,k[33]))



#define Rx16(X,Y,k) (X[0]=SHFL(X[0],SWAP), X[0]=ADD(X[0],Y[0]), X[0]=XOR(X[0],k),\
                     X[1]=SHFL(X[1],SWAP), X[1]=ADD(X[1],Y[1]), X[1]=XOR(X[1],k),\
                     X[2]=SHFL(X[2],SWAP), X[2]=ADD(X[2],Y[2]), X[2]=XOR(X[2],k),\
                     X[3]=SHFL(X[3],SWAP), X[3]=ADD(X[3],Y[3]), X[3]=XOR(X[3],k),\
		     Z[0]=Y[0], Z[1]=Y[1], Z[2]=Y[2], Z[3]=Y[3],\
		     Z[0]=SL(Z[0],3), Z[1]=SL(Z[1],3), Z[2]=SL(Z[2],3), Z[3]=SL(Z[3],3),\
                     Y[0]=SR(Y[0],61), Y[1]=SR(Y[1],61), Y[2]=SR(Y[2],61), Y[3]=SR(Y[3],61),\
		     Y[0]=XOR(Y[0],Z[0]), Y[1]=XOR(Y[1],Z[1]), Y[2]=XOR(Y[2],Z[2]), Y[3]=XOR(Y[3],Z[3]),\
		     Y[0]=XOR(X[0],Y[0]), Y[1]=XOR(X[1],Y[1]), Y[2]=XOR(X[2],Y[2]), Y[3]=XOR(X[3],Y[3]))

#define Ex16(X,Y,k) (Rx16(X,Y,k[0]), Rx16(X,Y,k[1]), Rx16(X,Y,k[2]), Rx16(X,Y,k[3]), Rx16(X,Y,k[4]), Rx16(X,Y,k[5]), Rx16(X,Y,k[6]), Rx16(X,Y,k[7]),\
		     Rx16(X,Y,k[8]), Rx16(X,Y,k[9]), Rx16(X,Y,k[10]), Rx16(X,Y,k[11]), Rx16(X,Y,k[12]), Rx16(X,Y,k[13]), Rx16(X,Y,k[14]), Rx16(X,Y,k[15]), \
		     Rx16(X,Y,k[16]), Rx16(X,Y,k[17]), Rx16(X,Y,k[18]), Rx16(X,Y,k[19]), Rx16(X,Y,k[20]), Rx16(X,Y,k[21]), Rx16(X,Y,k[22]), Rx16(X,Y,k[23]), \
		     Rx16(X,Y,k[24]), Rx16(X,Y,k[25]), Rx16(X,Y,k[26]), Rx16(X,Y,k[27]), Rx16(X,Y,k[28]), Rx16(X,Y,k[29]), Rx16(X,Y,k[30]), Rx16(X,Y,k[31]), \
		     Rx16(X,Y,k[32]), Rx16(X,Y,k[33]))



#define Rx12(X,Y,k) (X[0]=SHFL(X[0],SWAP), X[0]=ADD(X[0],Y[0]), X[0]=XOR(X[0],k),\
                     X[1]=SHFL(X[1],SWAP), X[1]=ADD(X[1],Y[1]), X[1]=XOR(X[1],k),\
                     X[2]=SHFL(X[2],SWAP), X[2]=ADD(X[2],Y[2]), X[2]=XOR(X[2],k),\
		     Z[0]=Y[0], Z[1]=Y[1], Z[2]=Y[2],\
		     Z[0]=SL(Z[0],3), Z[1]=SL(Z[1],3), Z[2]=SL(Z[2],3),\
                     Y[0]=SR(Y[0],61), Y[1]=SR(Y[1],61), Y[2]=SR(Y[2],61),\
		     Y[0]=XOR(Y[0],Z[0]), Y[1]=XOR(Y[1],Z[1]), Y[2]=XOR(Y[2],Z[2]),\
		     Y[0]=XOR(X[0],Y[0]), Y[1]=XOR(X[1],Y[1]), Y[2]=XOR(X[2],Y[2]))

#define Ex12(X,Y,k) (Rx12(X,Y,k[0]), Rx12(X,Y,k[1]), Rx12(X,Y,k[2]), Rx12(X,Y,k[3]), Rx12(X,Y,k[4]), Rx12(X,Y,k[5]), Rx12(X,Y,k[6]), Rx12(X,Y,k[7]),\
		     Rx12(X,Y,k[8]), Rx12(X,Y,k[9]), Rx12(X,Y,k[10]), Rx12(X,Y,k[11]), Rx12(X,Y,k[12]), Rx12(X,Y,k[13]), Rx12(X,Y,k[14]), Rx12(X,Y,k[15]), \
		     Rx12(X,Y,k[16]), Rx12(X,Y,k[17]), Rx12(X,Y,k[18]), Rx12(X,Y,k[19]), Rx12(X,Y,k[20]), Rx12(X,Y,k[21]), Rx12(X,Y,k[22]), Rx12(X,Y,k[23]), \
		     Rx12(X,Y,k[24]), Rx12(X,Y,k[25]), Rx12(X,Y,k[26]), Rx12(X,Y,k[27]), Rx12(X,Y,k[28]), Rx12(X,Y,k[29]), Rx12(X,Y,k[30]), Rx12(X,Y,k[31]),\
		     Rx12(X,Y,k[32]), Rx12(X,Y,k[33]))




#define Rx8(X,Y,k) (X[0]=SHFL(X[0],SWAP), X[0]=ADD(X[0],Y[0]), X[0]=XOR(X[0],k),\
                    X[1]=SHFL(X[1],SWAP), X[1]=ADD(X[1],Y[1]), X[1]=XOR(X[1],k),\
		    Z[0]=Y[0], Z[1]=Y[1], Z[0]=SL(Z[0],3), Z[1]=SL(Z[1],3),\
                    Y[0]=SR(Y[0],61), Y[1]=SR(Y[1],61), Y[0]=XOR(Y[0],Z[0]), Y[1]=XOR(Y[1],Z[1]),\
		    Y[0]=XOR(X[0],Y[0]), Y[1]=XOR(X[1],Y[1]))

#define Ex8(X,Y,k) (Rx8(X,Y,k[0]), Rx8(X,Y,k[1]), Rx8(X,Y,k[2]), Rx8(X,Y,k[3]), Rx8(X,Y,k[4]), Rx8(X,Y,k[5]), Rx8(X,Y,k[6]), Rx8(X,Y,k[7]),\
		    Rx8(X,Y,k[8]), Rx8(X,Y,k[9]), Rx8(X,Y,k[10]), Rx8(X,Y,k[11]), Rx8(X,Y,k[12]), Rx8(X,Y,k[13]), Rx8(X,Y,k[14]), Rx8(X,Y,k[15]), \
		    Rx8(X,Y,k[16]), Rx8(X,Y,k[17]), Rx8(X,Y,k[18]), Rx8(X,Y,k[19]), Rx8(X,Y,k[20]), Rx8(X,Y,k[21]), Rx8(X,Y,k[22]), Rx8(X,Y,k[23]), \
		    Rx8(X,Y,k[24]), Rx8(X,Y,k[25]), Rx8(X,Y,k[26]), Rx8(X,Y,k[27]), Rx8(X,Y,k[28]), Rx8(X,Y,k[29]), Rx8(X,Y,k[30]), Rx8(X,Y,k[31]),\
		    Rx8(X,Y,k[32]), Rx8(X,Y,k[33]))



#define Rx4(X,Y,k) (X[0]=SHFL(X[0],SWAP), X[0]=ADD(X[0],Y[0]), X[0]=XOR(X[0],k),\
		    Z[0]=Y[0], Z[0]=SL(Z[0],3), Y[0]=SR(Y[0],61), Y[0]=XOR(Y[0],Z[0]), Y[0]=XOR(X[0],Y[0]))

#define Ex4(X,Y,k) (Rx4(X,Y,k[0]), Rx4(X,Y,k[1]), Rx4(X,Y,k[2]), Rx4(X,Y,k[3]), Rx4(X,Y,k[4]), Rx4(X,Y,k[5]), Rx4(X,Y,k[6]), Rx4(X,Y,k[7]),\
		    Rx4(X,Y,k[8]), Rx4(X,Y,k[9]), Rx4(X,Y,k[10]), Rx4(X,Y,k[11]), Rx4(X,Y,k[12]), Rx4(X,Y,k[13]), Rx4(X,Y,k[14]), Rx4(X,Y,k[15]), \
		    Rx4(X,Y,k[16]), Rx4(X,Y,k[17]), Rx4(X,Y,k[18]), Rx4(X,Y,k[19]), Rx4(X,Y,k[20]), Rx4(X,Y,k[21]), Rx4(X,Y,k[22]), Rx4(X,Y,k[23]), \
		    Rx4(X,Y,k[24]), Rx4(X,Y,k[25]), Rx4(X,Y,k[26]), Rx4(X,Y,k[27]), Rx4(X,Y,k[28]), Rx4(X,Y,k[29]), Rx4(X,Y,k[30]), Rx4(X,Y,k[31]),\
		    Rx4(X,Y,k[32]), Rx4(X,Y,k[33]))



#define Rx2(x,y,k) (x[0]=RCS(x[0],8), x[1]=RCS(x[1],8), x[0]+=y[0], x[1]+=y[1],\
                    x[0]^=k, x[1]^=k, y[0]=LCS(y[0],3), y[1]=LCS(y[1],3), y[0]^=x[0], y[1]^=x[1])

#define Ex2(X,Y,k) (Rx2(X,Y,k[0]), Rx2(X,Y,k[1]), Rx2(X,Y,k[2]), Rx2(X,Y,k[3]), Rx2(X,Y,k[4]), Rx2(X,Y,k[5]), Rx2(X,Y,k[6]), Rx2(X,Y,k[7]),\
		    Rx2(X,Y,k[8]), Rx2(X,Y,k[9]), Rx2(X,Y,k[10]), Rx2(X,Y,k[11]), Rx2(X,Y,k[12]), Rx2(X,Y,k[13]), Rx2(X,Y,k[14]), Rx2(X,Y,k[15]), \
		    Rx2(X,Y,k[16]), Rx2(X,Y,k[17]), Rx2(X,Y,k[18]), Rx2(X,Y,k[19]), Rx2(X,Y,k[20]), Rx2(X,Y,k[21]), Rx2(X,Y,k[22]), Rx2(X,Y,k[23]), \
		    Rx2(X,Y,k[24]), Rx2(X,Y,k[25]), Rx2(X,Y,k[26]), Rx2(X,Y,k[27]), Rx2(X,Y,k[28]), Rx2(X,Y,k[29]), Rx2(X,Y,k[30]), Rx2(X,Y,k[31]),\
		    Rx2(X,Y,k[32]), Rx2(X,Y,k[33]))



#define Rx1(x,y,k) (x[0]=RCS(x[0],8), x[0]+=y[0], x[0]^=k, y[0]=LCS(y[0],3), y[0]^=x[0])

#define Ex1(X,Y,k) (Rx1(X,Y,k[0]), Rx1(X,Y,k[1]), Rx1(X,Y,k[2]), Rx1(X,Y,k[3]), Rx1(X,Y,k[4]), Rx1(X,Y,k[5]), Rx1(X,Y,k[6]), Rx1(X,Y,k[7]),\
		    Rx1(X,Y,k[8]), Rx1(X,Y,k[9]), Rx1(X,Y,k[10]), Rx1(X,Y,k[11]), Rx1(X,Y,k[12]), Rx1(X,Y,k[13]), Rx1(X,Y,k[14]), Rx1(X,Y,k[15]), \
		    Rx1(X,Y,k[16]), Rx1(X,Y,k[17]), Rx1(X,Y,k[18]), Rx1(X,Y,k[19]), Rx1(X,Y,k[20]), Rx1(X,Y,k[21]), Rx1(X,Y,k[22]), Rx1(X,Y,k[23]), \
		    Rx1(X,Y,k[24]), Rx1(X,Y,k[25]), Rx1(X,Y,k[26]), Rx1(X,Y,k[27]), Rx1(X,Y,k[28]), Rx1(X,Y,k[29]), Rx1(X,Y,k[30]), Rx1(X,Y,k[31]),\
		    Rx1(X,Y,k[32]), Rx1(X,Y,k[33]))

#define Rx1b(x,y,k) (x=RCS(x,8), x+=y, x^=k, y=LCS(y,3), y^=x)
