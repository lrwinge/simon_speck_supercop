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


#define XOR _mm_xor_si128     
#define SHFL _mm_shuffle_epi8  
#define SET _mm_set_epi32
#define AND _mm_and_si128
#define SR _mm_srli_epi32
#define SL _mm_slli_epi32
#define LOW _mm_unpacklo_epi32
#define HIGH _mm_unpackhi_epi32
#define SWAP SET(0x0e0d0c0f,0x0a09080b,0x06050407,0x02010003)
#define STORE(ip,X) _mm_storeu_si128((__m128i *)(ip),X)
#define LOAD(ip) _mm_loadu_si128((__m128i *)(ip))


#define ROL(X,r) (XOR(SL(X,r),SR(X,(32-r))))

#define RD(U,V,k) (V=XOR(V,ROL(U,2)), V=XOR(V,AND(ROL(U,1),SHFL(U,SWAP))), V=XOR(V,k) )

#define Sx16(U,V,k) (RD(U[0],V[0],k), RD(U[1],V[1],k), RD(U[2],V[2],k), RD(U[3],V[3],k))
#define Sx12(U,V,k) (RD(U[0],V[0],k), RD(U[1],V[1],k), RD(U[2],V[2],k))
#define Sx8(U,V,k)  (RD(U[0],V[0],k), RD(U[1],V[1],k))
#define Sx4(U,V,k)  (RD(U[0],V[0],k))


#define Rx16(X,Y,k,l) (Sx16(X,Y,k), Sx16(Y,X,l))
#define Rx12(X,Y,k,l) (Sx12(X,Y,k), Sx12(Y,X,l))
#define Rx8(X,Y,k,l)  (Sx8(X,Y,k), Sx8(Y,X,l))
#define Rx4(X,Y,k,l)  (Sx4(X,Y,k), Sx4(Y,X,l))


#define Ex16(X,Y,k) (Rx16(X,Y,k[0],k[1]),   Rx16(X,Y,k[2],k[3]),   Rx16(X,Y,k[4],k[5]),   Rx16(X,Y,k[6],k[7]),   Rx16(X,Y,k[8],k[9]), \
		     Rx16(X,Y,k[10],k[11]), Rx16(X,Y,k[12],k[13]), Rx16(X,Y,k[14],k[15]), Rx16(X,Y,k[16],k[17]), Rx16(X,Y,k[18],k[19]), \
		     Rx16(X,Y,k[20],k[21]), Rx16(X,Y,k[22],k[23]), Rx16(X,Y,k[24],k[25]), Rx16(X,Y,k[26],k[27]), Rx16(X,Y,k[28],k[29]), \
		     Rx16(X,Y,k[30],k[31]), Rx16(X,Y,k[32],k[33]), Rx16(X,Y,k[34],k[35]), Rx16(X,Y,k[36],k[37]), Rx16(X,Y,k[38],k[39]), \
		     Rx16(X,Y,k[40],k[41]), Rx16(X,Y,k[42],k[43]))

#define Ex12(X,Y,k) (Rx12(X,Y,k[0],k[1]),   Rx12(X,Y,k[2],k[3]),   Rx12(X,Y,k[4],k[5]),   Rx12(X,Y,k[6],k[7]),   Rx12(X,Y,k[8],k[9]), \
		     Rx12(X,Y,k[10],k[11]), Rx12(X,Y,k[12],k[13]), Rx12(X,Y,k[14],k[15]), Rx12(X,Y,k[16],k[17]), Rx12(X,Y,k[18],k[19]), \
		     Rx12(X,Y,k[20],k[21]), Rx12(X,Y,k[22],k[23]), Rx12(X,Y,k[24],k[25]), Rx12(X,Y,k[26],k[27]), Rx12(X,Y,k[28],k[29]), \
		     Rx12(X,Y,k[30],k[31]), Rx12(X,Y,k[32],k[33]), Rx12(X,Y,k[34],k[35]), Rx12(X,Y,k[36],k[37]), Rx12(X,Y,k[38],k[39]), \
		     Rx12(X,Y,k[40],k[41]), Rx12(X,Y,k[42],k[43]))

#define Ex8(X,Y,k) (Rx8(X,Y,k[0],k[1]),   Rx8(X,Y,k[2],k[3]),   Rx8(X,Y,k[4],k[5]),   Rx8(X,Y,k[6],k[7]),   Rx8(X,Y,k[8],k[9]),	\
		    Rx8(X,Y,k[10],k[11]), Rx8(X,Y,k[12],k[13]), Rx8(X,Y,k[14],k[15]), Rx8(X,Y,k[16],k[17]), Rx8(X,Y,k[18],k[19]), \
		    Rx8(X,Y,k[20],k[21]), Rx8(X,Y,k[22],k[23]), Rx8(X,Y,k[24],k[25]), Rx8(X,Y,k[26],k[27]), Rx8(X,Y,k[28],k[29]), \
		    Rx8(X,Y,k[30],k[31]), Rx8(X,Y,k[32],k[33]), Rx8(X,Y,k[34],k[35]), Rx8(X,Y,k[36],k[37]), Rx8(X,Y,k[38],k[39]), \
		    Rx8(X,Y,k[40],k[41]), Rx8(X,Y,k[42],k[43]))

#define Ex4(X,Y,k) (Rx4(X,Y,k[0],k[1]),   Rx4(X,Y,k[2],k[3]),   Rx4(X,Y,k[4],k[5]),   Rx4(X,Y,k[6],k[7]),   Rx4(X,Y,k[8],k[9]),	\
		    Rx4(X,Y,k[10],k[11]), Rx4(X,Y,k[12],k[13]), Rx4(X,Y,k[14],k[15]), Rx4(X,Y,k[16],k[17]), Rx4(X,Y,k[18],k[19]), \
		    Rx4(X,Y,k[20],k[21]), Rx4(X,Y,k[22],k[23]), Rx4(X,Y,k[24],k[25]), Rx4(X,Y,k[26],k[27]), Rx4(X,Y,k[28],k[29]), \
		    Rx4(X,Y,k[30],k[31]), Rx4(X,Y,k[32],k[33]), Rx4(X,Y,k[34],k[35]), Rx4(X,Y,k[36],k[37]), Rx4(X,Y,k[38],k[39]), \
		    Rx4(X,Y,k[40],k[41]), Rx4(X,Y,k[42],k[43]))



#define f(x) ((LCS(x,1) & LCS(x,8)) ^ LCS(x,2))
#define R2(x,y,k1,k2) (y^=f(x), y^=k1, x^=f(y), x^=k2) 


#define Rx2(x,y,k,l) (R2(x[0],y[0],k,l), R2(x[1],y[1],k,l))

#define Ex2(X,Y,k) (Rx2(X,Y,k[0],k[1]),   Rx2(X,Y,k[2],k[3]),   Rx2(X,Y,k[4],k[5]),   Rx2(X,Y,k[6],k[7]),   Rx2(X,Y,k[8],k[9]),	\
		    Rx2(X,Y,k[10],k[11]), Rx2(X,Y,k[12],k[13]), Rx2(X,Y,k[14],k[15]), Rx2(X,Y,k[16],k[17]), Rx2(X,Y,k[18],k[19]), \
		    Rx2(X,Y,k[20],k[21]), Rx2(X,Y,k[22],k[23]), Rx2(X,Y,k[24],k[25]), Rx2(X,Y,k[26],k[27]), Rx2(X,Y,k[28],k[29]), \
		    Rx2(X,Y,k[30],k[31]), Rx2(X,Y,k[32],k[33]), Rx2(X,Y,k[34],k[35]), Rx2(X,Y,k[36],k[37]), Rx2(X,Y,k[38],k[39]), \
		    Rx2(X,Y,k[40],k[41]), Rx2(X,Y,k[42],k[43]))


#define Rx1(x,y,k,l) (R2(x[0],y[0],k,l))

#define Ex1(X,Y,k) (Rx1(X,Y,k[0],k[1]),   Rx1(X,Y,k[2],k[3]),   Rx1(X,Y,k[4],k[5]),   Rx1(X,Y,k[6],k[7]),   Rx1(X,Y,k[8],k[9]),	\
		    Rx1(X,Y,k[10],k[11]), Rx1(X,Y,k[12],k[13]), Rx1(X,Y,k[14],k[15]), Rx1(X,Y,k[16],k[17]), Rx1(X,Y,k[18],k[19]), \
		    Rx1(X,Y,k[20],k[21]), Rx1(X,Y,k[22],k[23]), Rx1(X,Y,k[24],k[25]), Rx1(X,Y,k[26],k[27]), Rx1(X,Y,k[28],k[29]), \
		    Rx1(X,Y,k[30],k[31]), Rx1(X,Y,k[32],k[33]), Rx1(X,Y,k[34],k[35]), Rx1(X,Y,k[36],k[37]), Rx1(X,Y,k[38],k[39]), \
		    Rx1(X,Y,k[40],k[41]), Rx1(X,Y,k[42],k[43]))




#define _c 0xfffffffcLL
#define _d 0xfffffffdLL

#define RK(c0,c1,c2,c3,A,B,C,D,rk,key,i) (T=RCS(D,3)^B, A^=c0^T^RCS(T,1), key[i]=A,   rk[i]=SET(A,A,A,A), \
					  T=RCS(A,3)^C, B^=c1^T^RCS(T,1), key[i+1]=B, rk[i+1]=SET(B,B,B,B), \
					  T=RCS(B,3)^D, C^=c2^T^RCS(T,1), key[i+2]=C, rk[i+2]=SET(C,C,C,C), \
					  T=RCS(C,3)^A, D^=c3^T^RCS(T,1), key[i+3]=D, rk[i+3]=SET(D,D,D,D))

#define EK(A,B,C,D,rk,key) (RK(_d,_d,_c,_d,A,B,C,D,rk,key,4),  RK(_d,_c,_d,_d,A,B,C,D,rk,key,8),  RK(_d,_c,_d,_c,A,B,C,D,rk,key,12), \
			    RK(_d,_d,_c,_c,A,B,C,D,rk,key,16), RK(_c,_d,_d,_c,A,B,C,D,rk,key,20), RK(_c,_d,_c,_d,A,B,C,D,rk,key,24), \
			    RK(_d,_d,_d,_c,A,B,C,D,rk,key,28), RK(_c,_c,_c,_c,A,B,C,D,rk,key,32), RK(_c,_d,_c,_c,A,B,C,D,rk,key,36), \
			    RK(_d,_c,_c,_c,A,B,C,D,rk,key,40))
