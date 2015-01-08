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

#define STORE(ip,X)  _mm256_storeu_si256((__m256i *)(ip),X)
#define LOAD(ip) _mm256_loadu_si256((__m256i *)(ip))
#define SET _mm256_set_epi32
#define LOW _mm256_unpacklo_epi32
#define HIGH _mm256_unpackhi_epi32
#define AND _mm256_and_si256

#define XOR _mm256_xor_si256     
#define SHFL _mm256_shuffle_epi8
#define SL _mm256_slli_epi32
#define SR _mm256_srli_epi32
#define SWAP SET(0x0e0d0c0f,0x0a09080b,0x06050407,0x02010003,0x0e0d0c0f,0x0a09080b,0x06050407,0x02010003)



#define ROL(X,r) (XOR(SL(X,r),SR(X,(32-r))))

#define RD(U,V,k) (V=XOR(V,ROL(U,2)), V=XOR(V,AND(ROL(U,1),SHFL(U,SWAP))), V=XOR(V,k) )

#define Sx40(U,V,k) (RD(U[0],V[0],k), RD(U[1],V[1],k), RD(U[2],V[2],k), RD(U[3],V[3],k), RD(U[4],V[4],k))
#define Sx32(U,V,k) (RD(U[0],V[0],k), RD(U[1],V[1],k), RD(U[2],V[2],k), RD(U[3],V[3],k))
#define Sx24(U,V,k) (RD(U[0],V[0],k), RD(U[1],V[1],k), RD(U[2],V[2],k))
#define Sx16(U,V,k) (RD(U[0],V[0],k), RD(U[1],V[1],k))
#define Sx8(U,V,k)  (RD(U[0],V[0],k))


#define Rx40(X,Y,k,l) (Sx40(X,Y,k), Sx40(Y,X,l))
#define Rx32(X,Y,k,l) (Sx32(X,Y,k), Sx32(Y,X,l))
#define Rx24(X,Y,k,l) (Sx24(X,Y,k), Sx24(Y,X,l))
#define Rx16(X,Y,k,l) (Sx16(X,Y,k), Sx16(Y,X,l))
#define Rx8(X,Y,k,l)  (Sx8(X,Y,k),  Sx8(Y,X,l))



#define Ex40(X,Y,k) (Rx40(X,Y,k[0],k[1]),   Rx40(X,Y,k[2],k[3]),   Rx40(X,Y,k[4],k[5]),   Rx40(X,Y,k[6],k[7]),   Rx40(X,Y,k[8],k[9]), \
		     Rx40(X,Y,k[10],k[11]), Rx40(X,Y,k[12],k[13]), Rx40(X,Y,k[14],k[15]), Rx40(X,Y,k[16],k[17]), Rx40(X,Y,k[18],k[19]), \
		     Rx40(X,Y,k[20],k[21]), Rx40(X,Y,k[22],k[23]), Rx40(X,Y,k[24],k[25]), Rx40(X,Y,k[26],k[27]), Rx40(X,Y,k[28],k[29]), \
		     Rx40(X,Y,k[30],k[31]), Rx40(X,Y,k[32],k[33]), Rx40(X,Y,k[34],k[35]), Rx40(X,Y,k[36],k[37]), Rx40(X,Y,k[38],k[39]), \
		     Rx40(X,Y,k[40],k[41]))

#define Ex32(X,Y,k) (Rx32(X,Y,k[0],k[1]),   Rx32(X,Y,k[2],k[3]),   Rx32(X,Y,k[4],k[5]),   Rx32(X,Y,k[6],k[7]),   Rx32(X,Y,k[8],k[9]), \
                     Rx32(X,Y,k[10],k[11]), Rx32(X,Y,k[12],k[13]), Rx32(X,Y,k[14],k[15]), Rx32(X,Y,k[16],k[17]), Rx32(X,Y,k[18],k[19]),	\
                     Rx32(X,Y,k[20],k[21]), Rx32(X,Y,k[22],k[23]), Rx32(X,Y,k[24],k[25]), Rx32(X,Y,k[26],k[27]), Rx32(X,Y,k[28],k[29]),	\
                     Rx32(X,Y,k[30],k[31]), Rx32(X,Y,k[32],k[33]), Rx32(X,Y,k[34],k[35]), Rx32(X,Y,k[36],k[37]), Rx32(X,Y,k[38],k[39]),	\
                     Rx32(X,Y,k[40],k[41]))

#define Ex24(X,Y,k) (Rx24(X,Y,k[0],k[1]),   Rx24(X,Y,k[2],k[3]),   Rx24(X,Y,k[4],k[5]),   Rx24(X,Y,k[6],k[7]),   Rx24(X,Y,k[8],k[9]), \
                     Rx24(X,Y,k[10],k[11]), Rx24(X,Y,k[12],k[13]), Rx24(X,Y,k[14],k[15]), Rx24(X,Y,k[16],k[17]), Rx24(X,Y,k[18],k[19]),	\
                     Rx24(X,Y,k[20],k[21]), Rx24(X,Y,k[22],k[23]), Rx24(X,Y,k[24],k[25]), Rx24(X,Y,k[26],k[27]), Rx24(X,Y,k[28],k[29]),	\
                     Rx24(X,Y,k[30],k[31]), Rx24(X,Y,k[32],k[33]), Rx24(X,Y,k[34],k[35]), Rx24(X,Y,k[36],k[37]), Rx24(X,Y,k[38],k[39]),	\
                     Rx24(X,Y,k[40],k[41]))

#define Ex16(X,Y,k) (Rx16(X,Y,k[0],k[1]),   Rx16(X,Y,k[2],k[3]),   Rx16(X,Y,k[4],k[5]),   Rx16(X,Y,k[6],k[7]),   Rx16(X,Y,k[8],k[9]),\
		     Rx16(X,Y,k[10],k[11]), Rx16(X,Y,k[12],k[13]), Rx16(X,Y,k[14],k[15]), Rx16(X,Y,k[16],k[17]), Rx16(X,Y,k[18],k[19]), \
		     Rx16(X,Y,k[20],k[21]), Rx16(X,Y,k[22],k[23]), Rx16(X,Y,k[24],k[25]), Rx16(X,Y,k[26],k[27]), Rx16(X,Y,k[28],k[29]), \
		     Rx16(X,Y,k[30],k[31]), Rx16(X,Y,k[32],k[33]), Rx16(X,Y,k[34],k[35]), Rx16(X,Y,k[36],k[37]), Rx16(X,Y,k[38],k[39]), \
		     Rx16(X,Y,k[40],k[41]))

#define Ex8(X,Y,k) (Rx8(X,Y,k[0],k[1]),   Rx8(X,Y,k[2],k[3]),   Rx8(X,Y,k[4],k[5]),   Rx8(X,Y,k[6],k[7]),   Rx8(X,Y,k[8],k[9]),	\
		    Rx8(X,Y,k[10],k[11]), Rx8(X,Y,k[12],k[13]), Rx8(X,Y,k[14],k[15]), Rx8(X,Y,k[16],k[17]), Rx8(X,Y,k[18],k[19]), \
		    Rx8(X,Y,k[20],k[21]), Rx8(X,Y,k[22],k[23]), Rx8(X,Y,k[24],k[25]), Rx8(X,Y,k[26],k[27]), Rx8(X,Y,k[28],k[29]), \
		    Rx8(X,Y,k[30],k[31]), Rx8(X,Y,k[32],k[33]), Rx8(X,Y,k[34],k[35]), Rx8(X,Y,k[36],k[37]), Rx8(X,Y,k[38],k[39]), \
		    Rx8(X,Y,k[40],k[41]))



#define f(x) ((LCS(x,1) & LCS(x,8)) ^ LCS(x,2))
#define R2(x,y,k1,k2) (y^=f(x), y^=k1, x^=f(y), x^=k2) 

#define Rx4(x,y,k,l) (R2(x[0],y[0],k,l), R2(x[1],y[1],k,l), R2(x[2],y[2],k,l), R2(x[3],y[3],k,l))

#define Ex4(X,Y,k) (Rx4(X,Y,k[0],k[1]),   Rx4(X,Y,k[2],k[3]),   Rx4(X,Y,k[4],k[5]),   Rx4(X,Y,k[6],k[7]),   Rx4(X,Y,k[8],k[9]),	\
		    Rx4(X,Y,k[10],k[11]), Rx4(X,Y,k[12],k[13]), Rx4(X,Y,k[14],k[15]), Rx4(X,Y,k[16],k[17]), Rx4(X,Y,k[18],k[19]), \
		    Rx4(X,Y,k[20],k[21]), Rx4(X,Y,k[22],k[23]), Rx4(X,Y,k[24],k[25]), Rx4(X,Y,k[26],k[27]), Rx4(X,Y,k[28],k[29]), \
		    Rx4(X,Y,k[30],k[31]), Rx4(X,Y,k[32],k[33]), Rx4(X,Y,k[34],k[35]), Rx4(X,Y,k[36],k[37]), Rx4(X,Y,k[38],k[39]), \
		    Rx4(X,Y,k[40],k[41]))

#define Rx2(x,y,k,l) (R2(x[0],y[0],k,l), R2(x[1],y[1],k,l))

#define Ex2(X,Y,k) (Rx2(X,Y,k[0],k[1]),   Rx2(X,Y,k[2],k[3]),   Rx2(X,Y,k[4],k[5]),   Rx2(X,Y,k[6],k[7]),   Rx2(X,Y,k[8],k[9]),	\
		    Rx2(X,Y,k[10],k[11]), Rx2(X,Y,k[12],k[13]), Rx2(X,Y,k[14],k[15]), Rx2(X,Y,k[16],k[17]), Rx2(X,Y,k[18],k[19]), \
		    Rx2(X,Y,k[20],k[21]), Rx2(X,Y,k[22],k[23]), Rx2(X,Y,k[24],k[25]), Rx2(X,Y,k[26],k[27]), Rx2(X,Y,k[28],k[29]), \
		    Rx2(X,Y,k[30],k[31]), Rx2(X,Y,k[32],k[33]), Rx2(X,Y,k[34],k[35]), Rx2(X,Y,k[36],k[37]), Rx2(X,Y,k[38],k[39]), \
		    Rx2(X,Y,k[40],k[41]))

#define Rx1(x,y,k,l) (R2(x[0],y[0],k,l))

#define Ex1(X,Y,k) (Rx1(X,Y,k[0],k[1]),   Rx1(X,Y,k[2],k[3]),   Rx1(X,Y,k[4],k[5]),   Rx1(X,Y,k[6],k[7]),   Rx1(X,Y,k[8],k[9]),	\
		    Rx1(X,Y,k[10],k[11]), Rx1(X,Y,k[12],k[13]), Rx1(X,Y,k[14],k[15]), Rx1(X,Y,k[16],k[17]), Rx1(X,Y,k[18],k[19]), \
		    Rx1(X,Y,k[20],k[21]), Rx1(X,Y,k[22],k[23]), Rx1(X,Y,k[24],k[25]), Rx1(X,Y,k[26],k[27]), Rx1(X,Y,k[28],k[29]), \
		    Rx1(X,Y,k[30],k[31]), Rx1(X,Y,k[32],k[33]), Rx1(X,Y,k[34],k[35]), Rx1(X,Y,k[36],k[37]), Rx1(X,Y,k[38],k[39]), \
		    Rx1(X,Y,k[40],k[41]))



#define _c 0xfffffffcLL
#define _d 0xfffffffdLL

#define RK(c0,c1,c2,A,B,C,rk,key,i) (A^=c0^(RCS(C,3)^RCS(C,4)), key[i]=A,   rk[i]=SET(A,A,A,A,A,A,A,A), \
				     B^=c1^(RCS(A,3)^RCS(A,4)), key[i+1]=B, rk[i+1]=SET(B,B,B,B,B,B,B,B), \
				     C^=c2^(RCS(B,3)^RCS(B,4)), key[i+2]=C, rk[i+2]=SET(C,C,C,C,C,C,C,C))

#define EK(A,B,C,rk,key) (RK(_d,_c,_d,A,B,C,rk,key,3),  RK(_c,_d,_d,A,B,C,rk,key,6),  RK(_d,_d,_c,A,B,C,rk,key,9),  RK(_d,_d,_d,A,B,C,rk,key,12), \
			  RK(_c,_c,_c,A,B,C,rk,key,15), RK(_c,_c,_c,A,B,C,rk,key,18), RK(_d,_d,_c,A,B,C,rk,key,21), RK(_d,_c,_c,A,B,C,rk,key,24), \
			  RK(_d,_c,_c,A,B,C,rk,key,27), RK(_d,_d,_c,A,B,C,rk,key,30), RK(_c,_c,_d,A,B,C,rk,key,33), RK(_c,_d,_c,A,B,C,rk,key,36), \
			  RK(_c,_c,_c,A,B,C,rk,key,39))
