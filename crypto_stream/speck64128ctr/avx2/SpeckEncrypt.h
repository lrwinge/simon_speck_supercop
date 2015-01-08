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

#define u32 unsigned 
#define u64 unsigned long long
#define LCS _rotl
#define RCS _rotr

#define SET _mm256_set_epi32
#define LOW _mm256_unpacklo_epi32
#define HIGH _mm256_unpackhi_epi32
#define LOAD(ip) _mm256_loadu_si256((__m256i *)(ip))
#define STORE(ip,X) _mm256_storeu_si256((__m256i *)(ip),X)

#define XOR _mm256_xor_si256     
#define SHFL _mm256_shuffle_epi8
#define ADD _mm256_add_epi32
#define SL _mm256_slli_epi32
#define SR _mm256_srli_epi32
#define SWAP SET(0x0c0f0e0d,0x080b0a09,0x04070605,0x00030201,0x0c0f0e0d,0x080b0a09,0x04070605,0x00030201)

#define ROL(X,r) (XOR(SL(X,r),SR(X,(32-(r)))))
#define R(X,Y,k) (X=XOR(ADD(SHFL(X,SWAP),Y),k), Y=XOR(ROL(Y,3),X)) 

#define Rx40(X,Y,k) (R(X[0],Y[0],k), R(X[1],Y[1],k), R(X[2],Y[2],k), R(X[3],Y[3],k), R(X[4],Y[4],k))
#define Rx32(X,Y,k) (R(X[0],Y[0],k), R(X[1],Y[1],k), R(X[2],Y[2],k), R(X[3],Y[3],k))
#define Rx24(X,Y,k) (R(X[0],Y[0],k), R(X[1],Y[1],k), R(X[2],Y[2],k))
#define Rx16(X,Y,k) (R(X[0],Y[0],k), R(X[1],Y[1],k))
#define Rx8(X,Y,k)  (R(X[0],Y[0],k))


#define Ex40(X,Y,k) (Rx40(X,Y,k[0]),  Rx40(X,Y,k[1]),  Rx40(X,Y,k[2]),  Rx40(X,Y,k[3]),  Rx40(X,Y,k[4]),  Rx40(X,Y,k[5]),  Rx40(X,Y,k[6]),  Rx40(X,Y,k[7]), \
		     Rx40(X,Y,k[8]),  Rx40(X,Y,k[9]),  Rx40(X,Y,k[10]), Rx40(X,Y,k[11]), Rx40(X,Y,k[12]), Rx40(X,Y,k[13]), Rx40(X,Y,k[14]), Rx40(X,Y,k[15]), \
		     Rx40(X,Y,k[16]), Rx40(X,Y,k[17]), Rx40(X,Y,k[18]), Rx40(X,Y,k[19]), Rx40(X,Y,k[20]), Rx40(X,Y,k[21]), Rx40(X,Y,k[22]), Rx40(X,Y,k[23]), \
		     Rx40(X,Y,k[24]), Rx40(X,Y,k[25]), Rx40(X,Y,k[26]))

#define Ex32(X,Y,k) (Rx32(X,Y,k[0]),  Rx32(X,Y,k[1]),  Rx32(X,Y,k[2]),  Rx32(X,Y,k[3]),  Rx32(X,Y,k[4]),  Rx32(X,Y,k[5]),  Rx32(X,Y,k[6]),  Rx32(X,Y,k[7]), \
		     Rx32(X,Y,k[8]),  Rx32(X,Y,k[9]),  Rx32(X,Y,k[10]), Rx32(X,Y,k[11]), Rx32(X,Y,k[12]), Rx32(X,Y,k[13]), Rx32(X,Y,k[14]), Rx32(X,Y,k[15]), \
		     Rx32(X,Y,k[16]), Rx32(X,Y,k[17]), Rx32(X,Y,k[18]), Rx32(X,Y,k[19]), Rx32(X,Y,k[20]), Rx32(X,Y,k[21]), Rx32(X,Y,k[22]), Rx32(X,Y,k[23]), \
		     Rx32(X,Y,k[24]), Rx32(X,Y,k[25]), Rx32(X,Y,k[26]))

#define Ex24(X,Y,k) (Rx24(X,Y,k[0]),  Rx24(X,Y,k[1]),  Rx24(X,Y,k[2]),  Rx24(X,Y,k[3]),  Rx24(X,Y,k[4]),  Rx24(X,Y,k[5]),  Rx24(X,Y,k[6]),  Rx24(X,Y,k[7]), \
		     Rx24(X,Y,k[8]),  Rx24(X,Y,k[9]),  Rx24(X,Y,k[10]), Rx24(X,Y,k[11]), Rx24(X,Y,k[12]), Rx24(X,Y,k[13]), Rx24(X,Y,k[14]), Rx24(X,Y,k[15]), \
		     Rx24(X,Y,k[16]), Rx24(X,Y,k[17]), Rx24(X,Y,k[18]), Rx24(X,Y,k[19]), Rx24(X,Y,k[20]), Rx24(X,Y,k[21]), Rx24(X,Y,k[22]), Rx24(X,Y,k[23]), \
		     Rx24(X,Y,k[24]), Rx24(X,Y,k[25]), Rx24(X,Y,k[26]))

#define Ex16(X,Y,k) (Rx16(X,Y,k[0]),  Rx16(X,Y,k[1]),  Rx16(X,Y,k[2]),  Rx16(X,Y,k[3]),  Rx16(X,Y,k[4]),  Rx16(X,Y,k[5]),  Rx16(X,Y,k[6]),  Rx16(X,Y,k[7]), \
		     Rx16(X,Y,k[8]),  Rx16(X,Y,k[9]),  Rx16(X,Y,k[10]), Rx16(X,Y,k[11]), Rx16(X,Y,k[12]), Rx16(X,Y,k[13]), Rx16(X,Y,k[14]), Rx16(X,Y,k[15]), \
		     Rx16(X,Y,k[16]), Rx16(X,Y,k[17]), Rx16(X,Y,k[18]), Rx16(X,Y,k[19]), Rx16(X,Y,k[20]), Rx16(X,Y,k[21]), Rx16(X,Y,k[22]), Rx16(X,Y,k[23]), \
		     Rx16(X,Y,k[24]), Rx16(X,Y,k[25]), Rx16(X,Y,k[26]))

#define Ex8(X,Y,k) (Rx8(X,Y,k[0]),  Rx8(X,Y,k[1]),  Rx8(X,Y,k[2]),  Rx8(X,Y,k[3]),  Rx8(X,Y,k[4]),  Rx8(X,Y,k[5]),  Rx8(X,Y,k[6]),  Rx8(X,Y,k[7]), \
		    Rx8(X,Y,k[8]),  Rx8(X,Y,k[9]),  Rx8(X,Y,k[10]), Rx8(X,Y,k[11]), Rx8(X,Y,k[12]), Rx8(X,Y,k[13]), Rx8(X,Y,k[14]), Rx8(X,Y,k[15]), \
		    Rx8(X,Y,k[16]), Rx8(X,Y,k[17]), Rx8(X,Y,k[18]), Rx8(X,Y,k[19]), Rx8(X,Y,k[20]), Rx8(X,Y,k[21]), Rx8(X,Y,k[22]), Rx8(X,Y,k[23]), \
		    Rx8(X,Y,k[24]), Rx8(X,Y,k[25]), Rx8(X,Y,k[26]))

#define Rx4(x,y,k) (x[0]=RCS(x[0],8), x[1]=RCS(x[1],8), x[2]=RCS(x[2],8), x[3]=RCS(x[3],8), \
		    x[0]+=y[0], x[1]+=y[1], x[2]+=y[2], x[3]+=y[3],	\
		    x[0]^=k, x[1]^=k, x[2]^=k, x[3]^=k,			\
		    y[0]=LCS(y[0],3), y[1]=LCS(y[1],3), y[2]=LCS(y[2],3), y[3]=LCS(y[3],3), \
		    y[0]^=x[0], y[1]^=x[1], y[2]^=x[2], y[3]^=x[3])

#define Ex4(X,Y,k) (Rx4(X,Y,k[0]),  Rx4(X,Y,k[1]),  Rx4(X,Y,k[2]),  Rx4(X,Y,k[3]),  Rx4(X,Y,k[4]),  Rx4(X,Y,k[5]),  Rx4(X,Y,k[6]),  Rx4(X,Y,k[7]), \
		    Rx4(X,Y,k[8]),  Rx4(X,Y,k[9]),  Rx4(X,Y,k[10]), Rx4(X,Y,k[11]), Rx4(X,Y,k[12]), Rx4(X,Y,k[13]), Rx4(X,Y,k[14]), Rx4(X,Y,k[15]), \
		    Rx4(X,Y,k[16]), Rx4(X,Y,k[17]), Rx4(X,Y,k[18]), Rx4(X,Y,k[19]), Rx4(X,Y,k[20]), Rx4(X,Y,k[21]), Rx4(X,Y,k[22]), Rx4(X,Y,k[23]), \
		    Rx4(X,Y,k[24]), Rx4(X,Y,k[25]), Rx4(X,Y,k[26]))



#define Rx2(x,y,k) (x[0]=RCS(x[0],8), x[1]=RCS(x[1],8), x[0]+=y[0], x[1]+=y[1],	\
                    x[0]^=k, x[1]^=k, y[0]=LCS(y[0],3), y[1]=LCS(y[1],3), y[0]^=x[0], y[1]^=x[1])

#define Ex2(X,Y,k) (Rx2(X,Y,k[0]),  Rx2(X,Y,k[1]),  Rx2(X,Y,k[2]),  Rx2(X,Y,k[3]),  Rx2(X,Y,k[4]),  Rx2(X,Y,k[5]),  Rx2(X,Y,k[6]),  Rx2(X,Y,k[7]), \
		    Rx2(X,Y,k[8]),  Rx2(X,Y,k[9]),  Rx2(X,Y,k[10]), Rx2(X,Y,k[11]), Rx2(X,Y,k[12]), Rx2(X,Y,k[13]), Rx2(X,Y,k[14]), Rx2(X,Y,k[15]), \
		    Rx2(X,Y,k[16]), Rx2(X,Y,k[17]), Rx2(X,Y,k[18]), Rx2(X,Y,k[19]), Rx2(X,Y,k[20]), Rx2(X,Y,k[21]), Rx2(X,Y,k[22]), Rx2(X,Y,k[23]), \
		    Rx2(X,Y,k[24]), Rx2(X,Y,k[25]), Rx2(X,Y,k[26]))


#define Rx1(x,y,k) (x[0]=RCS(x[0],8), x[0]+=y[0], x[0]^=k, y[0]=LCS(y[0],3), y[0]^=x[0])

#define Ex1(X,Y,k) (Rx1(X,Y,k[0]),  Rx1(X,Y,k[1]),  Rx1(X,Y,k[2]),  Rx1(X,Y,k[3]),  Rx1(X,Y,k[4]),  Rx1(X,Y,k[5]),  Rx1(X,Y,k[6]),  Rx1(X,Y,k[7]), \
		    Rx1(X,Y,k[8]),  Rx1(X,Y,k[9]),  Rx1(X,Y,k[10]), Rx1(X,Y,k[11]), Rx1(X,Y,k[12]), Rx1(X,Y,k[13]), Rx1(X,Y,k[14]), Rx1(X,Y,k[15]), \
		    Rx1(X,Y,k[16]), Rx1(X,Y,k[17]), Rx1(X,Y,k[18]), Rx1(X,Y,k[19]), Rx1(X,Y,k[20]), Rx1(X,Y,k[21]), Rx1(X,Y,k[22]), Rx1(X,Y,k[23]), \
		    Rx1(X,Y,k[24]), Rx1(X,Y,k[25]), Rx1(X,Y,k[26]))

#define Rx1b(x,y,k) (x=RCS(x,8), x+=y, x^=k, y=LCS(y,3), y^=x)
