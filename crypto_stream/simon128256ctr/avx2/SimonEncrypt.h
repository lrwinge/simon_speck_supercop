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
#define AND _mm256_and_si256
#define SL _mm256_slli_epi64
#define SR _mm256_srli_epi64
#define SWAP SET(0x0e0d0c0b0a09080fLL,0x0605040302010007LL,0x0e0d0c0b0a09080fLL,0x0605040302010007LL)

#define STORE(ip,X)  _mm256_storeu_si256((__m256i *)(ip),X)
#define LOAD(ip) _mm256_loadu_si256((__m256i *)(ip))

#define SET _mm256_set_epi64x
#define LOW _mm256_unpacklo_epi64
#define HIGH _mm256_unpackhi_epi64



#define ROL(X,r) (XOR(SL(X,r),SR(X,(64-r))))

#define RD(U,V,k) (V=XOR(V,ROL(U,2)), V=XOR(V,AND(ROL(U,1),SHFL(U,SWAP))), V=XOR(V,k) )

#define Sx20(U,V,k) (RD(U[0],V[0],k), RD(U[1],V[1],k), RD(U[2],V[2],k), RD(U[3],V[3],k), RD(U[4],V[4],k))
#define Sx16(U,V,k) (RD(U[0],V[0],k), RD(U[1],V[1],k), RD(U[2],V[2],k), RD(U[3],V[3],k))
#define Sx12(U,V,k) (RD(U[0],V[0],k), RD(U[1],V[1],k), RD(U[2],V[2],k))
#define Sx8(U,V,k)  (RD(U[0],V[0],k), RD(U[1],V[1],k))
#define Sx4(U,V,k)  (RD(U[0],V[0],k))


#define Rx20(X,Y,k,l) (Sx20(X,Y,k), Sx20(Y,X,l))
#define Rx16(X,Y,k,l) (Sx16(X,Y,k), Sx16(Y,X,l))
#define Rx12(X,Y,k,l) (Sx12(X,Y,k), Sx12(Y,X,l))
#define Rx8(X,Y,k,l)  (Sx8(X,Y,k),  Sx8(Y,X,l))
#define Rx4(X,Y,k,l)  (Sx4(X,Y,k),  Sx4(Y,X,l))


#define Ex20(X,Y,k) (Rx20(X,Y,k[0],k[1]),   Rx20(X,Y,k[2],k[3]),   Rx20(X,Y,k[4],k[5]),   Rx20(X,Y,k[6],k[7]),   Rx20(X,Y,k[8],k[9]), \
		     Rx20(X,Y,k[10],k[11]), Rx20(X,Y,k[12],k[13]), Rx20(X,Y,k[14],k[15]), Rx20(X,Y,k[16],k[17]), Rx20(X,Y,k[18],k[19]), \
		     Rx20(X,Y,k[20],k[21]), Rx20(X,Y,k[22],k[23]), Rx20(X,Y,k[24],k[25]), Rx20(X,Y,k[26],k[27]), Rx20(X,Y,k[28],k[29]), \
		     Rx20(X,Y,k[30],k[31]), Rx20(X,Y,k[32],k[33]), Rx20(X,Y,k[34],k[35]), Rx20(X,Y,k[36],k[37]), Rx20(X,Y,k[38],k[39]), \
		     Rx20(X,Y,k[40],k[41]), Rx20(X,Y,k[42],k[43]), Rx20(X,Y,k[44],k[45]), Rx20(X,Y,k[46],k[47]), Rx20(X,Y,k[48],k[49]),	\
		     Rx20(X,Y,k[50],k[51]), Rx20(X,Y,k[52],k[53]), Rx20(X,Y,k[54],k[55]), Rx20(X,Y,k[56],k[57]), Rx20(X,Y,k[58],k[59]), \
		     Rx20(X,Y,k[60],k[61]), Rx20(X,Y,k[62],k[63]), Rx20(X,Y,k[64],k[65]), Rx20(X,Y,k[66],k[67]), Rx20(X,Y,k[68],k[69]),\
		     Rx20(X,Y,k[70],k[71]))

#define Ex16(X,Y,k) (Rx16(X,Y,k[0],k[1]),   Rx16(X,Y,k[2],k[3]),   Rx16(X,Y,k[4],k[5]),   Rx16(X,Y,k[6],k[7]),   Rx16(X,Y,k[8],k[9]), \
                     Rx16(X,Y,k[10],k[11]), Rx16(X,Y,k[12],k[13]), Rx16(X,Y,k[14],k[15]), Rx16(X,Y,k[16],k[17]), Rx16(X,Y,k[18],k[19]),	\
                     Rx16(X,Y,k[20],k[21]), Rx16(X,Y,k[22],k[23]), Rx16(X,Y,k[24],k[25]), Rx16(X,Y,k[26],k[27]), Rx16(X,Y,k[28],k[29]),	\
                     Rx16(X,Y,k[30],k[31]), Rx16(X,Y,k[32],k[33]), Rx16(X,Y,k[34],k[35]), Rx16(X,Y,k[36],k[37]), Rx16(X,Y,k[38],k[39]),	\
                     Rx16(X,Y,k[40],k[41]), Rx16(X,Y,k[42],k[43]), Rx16(X,Y,k[44],k[45]), Rx16(X,Y,k[46],k[47]), Rx16(X,Y,k[48],k[49]),	\
                     Rx16(X,Y,k[50],k[51]), Rx16(X,Y,k[52],k[53]), Rx16(X,Y,k[54],k[55]), Rx16(X,Y,k[56],k[57]), Rx16(X,Y,k[58],k[59]),	\
                     Rx16(X,Y,k[60],k[61]), Rx16(X,Y,k[62],k[63]), Rx16(X,Y,k[64],k[65]), Rx16(X,Y,k[66],k[67]), Rx16(X,Y,k[68],k[69]),	\
		     Rx16(X,Y,k[70],k[71]))

#define Ex12(X,Y,k) (Rx12(X,Y,k[0],k[1]),   Rx12(X,Y,k[2],k[3]),   Rx12(X,Y,k[4],k[5]),   Rx12(X,Y,k[6],k[7]),   Rx12(X,Y,k[8],k[9]), \
                     Rx12(X,Y,k[10],k[11]), Rx12(X,Y,k[12],k[13]), Rx12(X,Y,k[14],k[15]), Rx12(X,Y,k[16],k[17]), Rx12(X,Y,k[18],k[19]),	\
                     Rx12(X,Y,k[20],k[21]), Rx12(X,Y,k[22],k[23]), Rx12(X,Y,k[24],k[25]), Rx12(X,Y,k[26],k[27]), Rx12(X,Y,k[28],k[29]),	\
                     Rx12(X,Y,k[30],k[31]), Rx12(X,Y,k[32],k[33]), Rx12(X,Y,k[34],k[35]), Rx12(X,Y,k[36],k[37]), Rx12(X,Y,k[38],k[39]),	\
                     Rx12(X,Y,k[40],k[41]), Rx12(X,Y,k[42],k[43]), Rx12(X,Y,k[44],k[45]), Rx12(X,Y,k[46],k[47]), Rx12(X,Y,k[48],k[49]),	\
                     Rx12(X,Y,k[50],k[51]), Rx12(X,Y,k[52],k[53]), Rx12(X,Y,k[54],k[55]), Rx12(X,Y,k[56],k[57]), Rx12(X,Y,k[58],k[59]),	\
                     Rx12(X,Y,k[60],k[61]), Rx12(X,Y,k[62],k[63]), Rx12(X,Y,k[64],k[65]), Rx12(X,Y,k[66],k[67]), Rx12(X,Y,k[68],k[69]),	\
		     Rx12(X,Y,k[70],k[71]))

#define Ex8(X,Y,k) (Rx8(X,Y,k[0],k[1]),   Rx8(X,Y,k[2],k[3]),   Rx8(X,Y,k[4],k[5]),   Rx8(X,Y,k[6],k[7]),   Rx8(X,Y,k[8],k[9]),\
		    Rx8(X,Y,k[10],k[11]), Rx8(X,Y,k[12],k[13]), Rx8(X,Y,k[14],k[15]), Rx8(X,Y,k[16],k[17]), Rx8(X,Y,k[18],k[19]), \
		    Rx8(X,Y,k[20],k[21]), Rx8(X,Y,k[22],k[23]), Rx8(X,Y,k[24],k[25]), Rx8(X,Y,k[26],k[27]), Rx8(X,Y,k[28],k[29]), \
		    Rx8(X,Y,k[30],k[31]), Rx8(X,Y,k[32],k[33]), Rx8(X,Y,k[34],k[35]), Rx8(X,Y,k[36],k[37]), Rx8(X,Y,k[38],k[39]), \
		    Rx8(X,Y,k[40],k[41]), Rx8(X,Y,k[42],k[43]), Rx8(X,Y,k[44],k[45]), Rx8(X,Y,k[46],k[47]), Rx8(X,Y,k[48],k[49]), \
		    Rx8(X,Y,k[50],k[51]), Rx8(X,Y,k[52],k[53]), Rx8(X,Y,k[54],k[55]), Rx8(X,Y,k[56],k[57]), Rx8(X,Y,k[58],k[59]), \
		    Rx8(X,Y,k[60],k[61]), Rx8(X,Y,k[62],k[63]), Rx8(X,Y,k[64],k[65]), Rx8(X,Y,k[66],k[67]), Rx8(X,Y,k[68],k[69]), \
		    Rx8(X,Y,k[70],k[71]))

#define Ex4(X,Y,k) (Rx4(X,Y,k[0],k[1]),   Rx4(X,Y,k[2],k[3]),   Rx4(X,Y,k[4],k[5]),   Rx4(X,Y,k[6],k[7]),   Rx4(X,Y,k[8],k[9]),	\
		    Rx4(X,Y,k[10],k[11]), Rx4(X,Y,k[12],k[13]), Rx4(X,Y,k[14],k[15]), Rx4(X,Y,k[16],k[17]), Rx4(X,Y,k[18],k[19]), \
		    Rx4(X,Y,k[20],k[21]), Rx4(X,Y,k[22],k[23]), Rx4(X,Y,k[24],k[25]), Rx4(X,Y,k[26],k[27]), Rx4(X,Y,k[28],k[29]), \
		    Rx4(X,Y,k[30],k[31]), Rx4(X,Y,k[32],k[33]), Rx4(X,Y,k[34],k[35]), Rx4(X,Y,k[36],k[37]), Rx4(X,Y,k[38],k[39]), \
		    Rx4(X,Y,k[40],k[41]), Rx4(X,Y,k[42],k[43]), Rx4(X,Y,k[44],k[45]), Rx4(X,Y,k[46],k[47]), Rx4(X,Y,k[48],k[49]), \
		    Rx4(X,Y,k[50],k[51]), Rx4(X,Y,k[52],k[53]), Rx4(X,Y,k[54],k[55]), Rx4(X,Y,k[56],k[57]), Rx4(X,Y,k[58],k[59]), \
		    Rx4(X,Y,k[60],k[61]), Rx4(X,Y,k[62],k[63]), Rx4(X,Y,k[64],k[65]), Rx4(X,Y,k[66],k[67]), Rx4(X,Y,k[68],k[69]),\
		    Rx4(X,Y,k[70],k[71]))



#define f(x) ((LCS(x,1) & LCS(x,8)) ^ LCS(x,2))
#define R2(x,y,k1,k2) (y^=f(x), y^=k1, x^=f(y), x^=k2) 

#define Rx2(x,y,k,l) (R2(x[0],y[0],k,l), R2(x[1],y[1],k,l))

#define Ex2(X,Y,k) (Rx2(X,Y,k[0],k[1]),   Rx2(X,Y,k[2],k[3]),   Rx2(X,Y,k[4],k[5]),   Rx2(X,Y,k[6],k[7]),   Rx2(X,Y,k[8],k[9]),	\
		    Rx2(X,Y,k[10],k[11]), Rx2(X,Y,k[12],k[13]), Rx2(X,Y,k[14],k[15]), Rx2(X,Y,k[16],k[17]), Rx2(X,Y,k[18],k[19]), \
		    Rx2(X,Y,k[20],k[21]), Rx2(X,Y,k[22],k[23]), Rx2(X,Y,k[24],k[25]), Rx2(X,Y,k[26],k[27]), Rx2(X,Y,k[28],k[29]), \
		    Rx2(X,Y,k[30],k[31]), Rx2(X,Y,k[32],k[33]), Rx2(X,Y,k[34],k[35]), Rx2(X,Y,k[36],k[37]), Rx2(X,Y,k[38],k[39]), \
		    Rx2(X,Y,k[40],k[41]), Rx2(X,Y,k[42],k[43]), Rx2(X,Y,k[44],k[45]), Rx2(X,Y,k[46],k[47]), Rx2(X,Y,k[48],k[49]), \
		    Rx2(X,Y,k[50],k[51]), Rx2(X,Y,k[52],k[53]), Rx2(X,Y,k[54],k[55]), Rx2(X,Y,k[56],k[57]), Rx2(X,Y,k[58],k[59]), \
		    Rx2(X,Y,k[60],k[61]), Rx2(X,Y,k[62],k[63]), Rx2(X,Y,k[64],k[65]), Rx2(X,Y,k[66],k[67]), Rx2(X,Y,k[68],k[69]), \
		    Rx2(X,Y,k[70],k[71]))


#define Rx1(x,y,k,l) (R2(x[0],y[0],k,l))

#define Ex1(X,Y,k) (Rx1(X,Y,k[0],k[1]),   Rx1(X,Y,k[2],k[3]),   Rx1(X,Y,k[4],k[5]),   Rx1(X,Y,k[6],k[7]),   Rx1(X,Y,k[8],k[9]),	\
		    Rx1(X,Y,k[10],k[11]), Rx1(X,Y,k[12],k[13]), Rx1(X,Y,k[14],k[15]), Rx1(X,Y,k[16],k[17]), Rx1(X,Y,k[18],k[19]), \
		    Rx1(X,Y,k[20],k[21]), Rx1(X,Y,k[22],k[23]), Rx1(X,Y,k[24],k[25]), Rx1(X,Y,k[26],k[27]), Rx1(X,Y,k[28],k[29]), \
		    Rx1(X,Y,k[30],k[31]), Rx1(X,Y,k[32],k[33]), Rx1(X,Y,k[34],k[35]), Rx1(X,Y,k[36],k[37]), Rx1(X,Y,k[38],k[39]), \
		    Rx1(X,Y,k[40],k[41]), Rx1(X,Y,k[42],k[43]), Rx1(X,Y,k[44],k[45]), Rx1(X,Y,k[46],k[47]), Rx1(X,Y,k[48],k[49]), \
		    Rx1(X,Y,k[50],k[51]), Rx1(X,Y,k[52],k[53]), Rx1(X,Y,k[54],k[55]), Rx1(X,Y,k[56],k[57]), Rx1(X,Y,k[58],k[59]), \
		    Rx1(X,Y,k[60],k[61]), Rx1(X,Y,k[62],k[63]), Rx1(X,Y,k[64],k[65]), Rx1(X,Y,k[66],k[67]), Rx1(X,Y,k[68],k[69]), \
		    Rx1(X,Y,k[70],k[71]))



#define _c 0xfffffffffffffffcLL
#define _d 0xfffffffffffffffdLL

#define RK(c0,c1,c2,c3,A,B,C,D,rk,key,i) (T=RCS(D,3)^B, A^=c0^T^RCS(T,1), key[i]=A,   rk[i]=SET(A,A,A,A), \
					  T=RCS(A,3)^C, B^=c1^T^RCS(T,1), key[i+1]=B, rk[i+1]=SET(B,B,B,B), \
					  T=RCS(B,3)^D, C^=c2^T^RCS(T,1), key[i+2]=C, rk[i+2]=SET(C,C,C,C), \
					  T=RCS(C,3)^A, D^=c3^T^RCS(T,1), key[i+3]=D, rk[i+3]=SET(D,D,D,D))

#define EK(A,B,C,D,rk,key) (RK(_d,_d,_c,_d,A,B,C,D,rk,key,4),  RK(_c,_c,_c,_d,A,B,C,D,rk,key,8),  RK(_d,_d,_d,_c,A,B,C,D,rk,key,12), RK(_c,_d,_d,_c,A,B,C,D,rk,key,16), \
			    RK(_d,_c,_d,_d,A,B,C,D,rk,key,20), RK(_c,_d,_d,_c,A,B,C,D,rk,key,24), RK(_c,_c,_d,_c,A,B,C,D,rk,key,28), RK(_c,_c,_c,_c,A,B,C,D,rk,key,32), \
			    RK(_c,_d,_c,_d,A,B,C,D,rk,key,36), RK(_d,_d,_c,_c,A,B,C,D,rk,key,40), RK(_c,_c,_d,_d,A,B,C,D,rk,key,44), RK(_c,_c,_d,_c,A,B,C,D,rk,key,48), \
			    RK(_d,_c,_c,_d,A,B,C,D,rk,key,52), RK(_c,_c,_d,_d,A,B,C,D,rk,key,56), RK(_d,_c,_d,_d,A,B,C,D,rk,key,60), RK(_d,_d,_d,_d,A,B,C,D,rk,key,64),\
			    RK(_c,_d,_c,_c,A,B,C,D,rk,key,68))
