//AVX2 C code for SIMON 128/192

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

#include <stdio.h>
#include <stdlib.h>
#include "SimonEncrypt.h"

#define numrounds 69
#define numkeywords 3


int crypto_stream_simon128192ctr_avx2(
  unsigned char *out, 
  unsigned long long outlen, 
  const unsigned char *n, 
  const unsigned char *k
)
{
  int i;
  u64 nonce[2],K[numkeywords],key[numrounds],x,y;
  unsigned char block[16];
  __m256i rk[numrounds];

  if (!outlen) return 0;

  nonce[0]=((u64 *)n)[0];
  nonce[1]=((u64 *)n)[1];

  for(i=0;i<numkeywords;i++) K[i]=((u64 *)k)[i];

  ExpandKey(K,rk,key);

  if (outlen<=16){
    x=nonce[1]; y=nonce[0]; nonce[0]++;
    for(i=0;i<68;i+=2) R2(x,y,key[i],key[i+1]);
    R1(x,y,key[68]);
    ((u64 *)block)[1]=y; ((u64 *)block)[0]=x;
    for(i=0;i<outlen;i++) out[i]=block[i];

    return 0;
  }
  
  while(outlen>=320){
    Encrypt(out,nonce,rk,key,320);
    out+=320; outlen-=320;
  }

  if (outlen>=256){
    Encrypt(out,nonce,rk,key,256);
    out+=256; outlen-=256;
  }
  if (outlen>=192){
    Encrypt(out,nonce,rk,key,192);
    out+=192; outlen-=192;
  }
  if (outlen>=128){
    Encrypt(out,nonce,rk,key,128);
    out+=128; outlen-=128;
  }
  if (outlen>=64){
    Encrypt(out,nonce,rk,key,64);
    out+=64; outlen-=64;
  }
  if (outlen>=32){
    Encrypt(out,nonce,rk,key,32);
    out+=32; outlen-=32;
  }
  if (outlen>=16){
    Encrypt(out,nonce,rk,key,16);
    out+=16; outlen-=16;
  }
  if (outlen>0){ 
    Encrypt(block,nonce,rk,key,16);
    for (i=0;i<outlen;i++) out[i] = block[i];
  }

  return 0;
}



inline int Encrypt(unsigned char *out, u64 nonce[], __m256i rk[], u64 key[], int numbytes)
{
  u64 i,x[2],y[2];
  __m256i X[5],Y[5];

  if (numbytes==16){
    x[0]=nonce[1]; y[0]=nonce[0]; nonce[0]++;
    Ex1(x,y,key);
    ((u64 *)out)[1]=y[0]; ((u64 *)out)[0]=x[0];

    return 0;
  }

  if (numbytes==32){
    x[0]=nonce[1]; y[0]=nonce[0]; nonce[0]++;
    x[1]=nonce[1]; y[1]=nonce[0]; nonce[0]++;
    Ex2(x,y,key);
    ((u64 *)out)[1]=y[0]; ((u64 *)out)[0]=x[0];
    ((u64 *)out)[3]=y[1]; ((u64 *)out)[2]=x[1];

    return 0;
  }

  X[0]=SET(nonce[1],nonce[1],nonce[1],nonce[1]);  
  Y[0]=SET(nonce[0]+3,nonce[0]+1,nonce[0]+2,nonce[0]); nonce[0]+=4;
  
  if (numbytes==64) Ex4(X,Y,rk);
  else{
    X[1]=X[0];
    Y[1]=SET(nonce[0]+3,nonce[0]+1,nonce[0]+2,nonce[0]); nonce[0]+=4;
    if (numbytes==128) Ex8(X,Y,rk);
    else{
      X[2]=X[0];
      Y[2]=SET(nonce[0]+3,nonce[0]+1,nonce[0]+2,nonce[0]); nonce[0]+=4;
      if (numbytes==192) Ex12(X,Y,rk);
      else{
	X[3]=X[0];
	Y[3]=SET(nonce[0]+3,nonce[0]+1,nonce[0]+2,nonce[0]); nonce[0]+=4;
	if (numbytes==256) Ex16(X,Y,rk);
	else{
	  X[4]=X[0];
	  Y[4]=SET(nonce[0]+3,nonce[0]+1,nonce[0]+2,nonce[0]); nonce[0]+=4;
	  Ex20(X,Y,rk);
	}
      }
    }
  }

  STORE(out,LOW(X[0],Y[0]));
  STORE(out+32,HIGH(X[0],Y[0]));

  if (numbytes>=128){
    STORE(out+64,LOW(X[1],Y[1]));
    STORE(out+96,HIGH(X[1],Y[1]));
  }
  if (numbytes>=192){
    STORE(out+128,LOW(X[2],Y[2]));
    STORE(out+160,HIGH(X[2],Y[2]));
  }
  if (numbytes>=256){
    STORE(out+192,LOW(X[3],Y[3]));
    STORE(out+224,HIGH(X[3],Y[3]));
  }
  if (numbytes>=320){
    STORE(out+256,LOW(X[4],Y[4]));
    STORE(out+288,HIGH(X[4],Y[4]));
  }
  return 0;
}



int crypto_stream_simon128192ctr_avx2_xor(
  unsigned char *out, 
  const unsigned char *in, 
  unsigned long long inlen, 
  const unsigned char *n, 
  const unsigned char *k
)
{
  int i;
  u64 nonce[2],K[numkeywords],key[numrounds],x,y;
  unsigned char block[16];
  __m256i rk[numrounds];

  if (!inlen) return 0;

  nonce[0]=((u64*)n)[0];
  nonce[1]=((u64*)n)[1];

  for(i=0;i<numkeywords;i++) K[i]=((u64 *)k)[i];

  ExpandKey(K,rk,key);

  if (inlen<=16){
    x=nonce[1]; y=nonce[0]; nonce[0]++;
    for(i=0;i<68;i+=2) R2(x,y,key[i],key[i+1]);
    R1(x,y,key[68]);
    ((u64 *)block)[1]=y; ((u64 *)block)[0]=x;
    for(i=0;i<inlen;i++) out[i]=block[i]^in[i];

    return 0;
  }
  
  while(inlen>=320){
    Encrypt_Xor(out,in,nonce,rk,key,320);
    in+=320; inlen-=320; out+=320;
  }

  if (inlen>=256){
    Encrypt_Xor(out,in,nonce,rk,key,256);
    in+=256; inlen-=256; out+=256;
  }
  if (inlen>=192){
    Encrypt_Xor(out,in,nonce,rk,key,192);
    in+=192; inlen-=192; out+=192;
  }
  if (inlen>=128){
    Encrypt_Xor(out,in,nonce,rk,key,128);
    in+=128; inlen-=128; out+=128;
  }
  if (inlen>=64){
    Encrypt_Xor(out,in,nonce,rk,key,64);
    in+=64; inlen-=64; out+=64;
  }
  if (inlen>=32){
    Encrypt_Xor(out,in,nonce,rk,key,32);
    in+=32; inlen-=32; out+=32;
  }
  if (inlen>=16){
    Encrypt_Xor(block,in,nonce,rk,key,16);
    ((u64 *)out)[1]=((u64 *)block)[1]^((u64 *)in)[1];
    ((u64 *)out)[0]=((u64 *)block)[0]^((u64 *)in)[0];
    in+=16; inlen-=16; out+=16;
  }
  if (inlen>0){ 
    Encrypt_Xor(block,in,nonce,rk,key,16);
    for (i=0;i<inlen;i++) out[i]=block[i]^in[i];
  }
  return 0;
}



inline int Encrypt_Xor(unsigned char *out, unsigned char *in, u64 nonce[], __m256i rk[], u64 key[], int numbytes)
{
  u64 i,x[2],y[2];
  __m256i X[5],Y[5];

  if (numbytes==16){
    x[0]=nonce[1]; y[0]=nonce[0]; nonce[0]++;
    Ex1(x,y,key);
    ((u64 *)out)[1]=y[0]; ((u64 *)out)[0]=x[0];
    return 0;
  }

  if (numbytes==32){
    x[0]=nonce[1]; y[0]=nonce[0]; nonce[0]++;
    x[1]=nonce[1]; y[1]=nonce[0]; nonce[0]++;
    Ex2(x,y,key);
    ((u64 *)out)[1]=y[0]^((u64 *)in)[1]; ((u64 *)out)[0]=x[0]^((u64 *)in)[0];
    ((u64 *)out)[3]=y[1]^((u64 *)in)[3]; ((u64 *)out)[2]=x[1]^((u64 *)in)[2];

    return 0;
  }
  
  X[0]=SET(nonce[1],nonce[1],nonce[1],nonce[1]); 
  Y[0]=SET(nonce[0]+3,nonce[0]+1,nonce[0]+2,nonce[0]); nonce[0]+=4;
  
  if (numbytes==64) Ex4(X,Y,rk);
  else{
    X[1]=X[0];
    Y[1]=SET(nonce[0]+3,nonce[0]+1,nonce[0]+2,nonce[0]); nonce[0]+=4;
    if (numbytes==128) Ex8(X,Y,rk);
    else{
      X[2]=X[0];
      Y[2]=SET(nonce[0]+3,nonce[0]+1,nonce[0]+2,nonce[0]); nonce[0]+=4;
      if (numbytes==192) Ex12(X,Y,rk);
      else{
	X[3]=X[0];
	Y[3]=SET(nonce[0]+3,nonce[0]+1,nonce[0]+2,nonce[0]); nonce[0]+=4;
	if (numbytes==256) Ex16(X,Y,rk);
	else{
	  X[4]=X[0];
	  Y[4]=SET(nonce[0]+3,nonce[0]+1,nonce[0]+2,nonce[0]); nonce[0]+=4;
	  Ex20(X,Y,rk);
	}
      }
    }
  }

  STORE(out,XOR(LOAD(in),LOW(X[0],Y[0])));
  STORE(out+32,XOR(LOAD(in+32),HIGH(X[0],Y[0])));

  if (numbytes>=128){
    STORE(out+64,XOR(LOAD(in+64),LOW(X[1],Y[1])));
    STORE(out+96,XOR(LOAD(in+96),HIGH(X[1],Y[1])));
  }
  if (numbytes>=192){
    STORE(out+128,XOR(LOAD(in+128),LOW(X[2],Y[2])));
    STORE(out+160,XOR(LOAD(in+160),HIGH(X[2],Y[2])));
  }
  if (numbytes>=256){
    STORE(out+192,XOR(LOAD(in+192),LOW(X[3],Y[3])));
    STORE(out+224,XOR(LOAD(in+224),HIGH(X[3],Y[3])));
  }
  if (numbytes>=320){
    STORE(out+256,XOR(LOAD(in+256),LOW(X[4],Y[4])));
    STORE(out+288,XOR(LOAD(in+288),HIGH(X[4],Y[4])));
  }
  return 0;
}



int ExpandKey(u64 K[],__m256i rk[],u64 key[])
{
  u64 A,B,C,D,z3=0xfc2ce51207a635dbLL;

  A=K[0]; key[0]=A; rk[0]=SET(A,A,A,A);
  B=K[1]; key[1]=B; rk[1]=SET(B,B,B,B);
  C=K[2]; key[2]=C; rk[2]=SET(C,C,C,C);

  EK(A,B,C,rk,key);

  return 0;
}
