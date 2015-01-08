//AVX2 C code for SIMON 64/128

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


#include "stdio.h"
#include "stdlib.h"
#include "SimonEncrypt.h"



int crypto_stream_simon64128ctr_avx2(
  unsigned char *out, 
  unsigned long long outlen, 
  const unsigned char *n, 
  const unsigned char *k
)
{
  int i;
  u32 nonce[2],K[4],key[44],x,y;
  unsigned char block[8];
  __m256i rk[44];

  if (!outlen) return 0;

  nonce[0]=((u32*)n)[0];
  nonce[1]=((u32*)n)[1];

  for(i=0;i<4;i++) K[i]=((u32 *)k)[i];

  ExpandKey(K,rk,key);

  if (outlen<=8){
    x=nonce[1]; y=nonce[0]; nonce[0]++;
    for(i=0;i<44;i+=2) R2(x,y,key[i],key[i+1]);
    ((u32*)block)[1]=x; ((u32*)block)[0]=y;
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
  if (outlen>=8){
    Encrypt(out,nonce,rk,key,8);
    out+=8; outlen-=8;
  }
  if (outlen>0){ 
    Encrypt(block,nonce,rk,key,8);
    for (i=0;i<outlen;i++) out[i] = block[i];
  }

  return 0;
}



inline int Encrypt(unsigned char *out, u32 nonce[],__m256i rk[], u32 key[],int numbytes)
{
  u32 i,x[4],y[4];
  __m256i X[5],Y[5];


  if (numbytes==8){
    x[0]=nonce[1]; y[0]=nonce[0]; nonce[0]++;
    Ex1(x,y,key);
    ((u32 *)out)[1]=x[0]; ((u32 *)out)[0]=y[0];

    return 0;
  }

  if (numbytes==16){
    x[0]=nonce[1]; y[0]=nonce[0]; nonce[0]++;
    x[1]=nonce[1]; y[1]=nonce[0]; nonce[0]++;
    Ex2(x,y,key);
    ((u32 *)out)[1]=x[0]; ((u32 *)out)[0]=y[0];
    ((u32 *)out)[3]=x[1]; ((u32 *)out)[2]=y[1];

    return 0;
  }
  
  if (numbytes==32){
    x[0]=nonce[1]; y[0]=nonce[0]; nonce[0]++;
    x[1]=nonce[1]; y[1]=nonce[0]; nonce[0]++;
    x[2]=nonce[1]; y[2]=nonce[0]; nonce[0]++;
    x[3]=nonce[1]; y[3]=nonce[0]; nonce[0]++;
    Ex4(x,y,key);
    ((u32 *)out)[1]=x[0]; ((u32 *)out)[0]=y[0];
    ((u32 *)out)[3]=x[1]; ((u32 *)out)[2]=y[1];
    ((u32 *)out)[5]=x[2]; ((u32 *)out)[4]=y[2];
    ((u32 *)out)[7]=x[3]; ((u32 *)out)[6]=y[3];

    return 0;
  }
  
  X[0]=SET(nonce[1],nonce[1],nonce[1],nonce[1],nonce[1],nonce[1],nonce[1],nonce[1]);  
  Y[0]=SET(nonce[0]+7,nonce[0]+6,nonce[0]+3,nonce[0]+2,nonce[0]+5,nonce[0]+4,nonce[0]+1,nonce[0]); nonce[0]+=8;

  if (numbytes==64) Ex8(X,Y,rk); 
  else{
    X[1]=X[0];
    Y[1]=SET(nonce[0]+7,nonce[0]+6,nonce[0]+3,nonce[0]+2,nonce[0]+5,nonce[0]+4,nonce[0]+1,nonce[0]); nonce[0]+=8;
    if (numbytes==128) Ex16(X,Y,rk); 
    else{
      X[2]=X[0];
      Y[2]=SET(nonce[0]+7,nonce[0]+6,nonce[0]+3,nonce[0]+2,nonce[0]+5,nonce[0]+4,nonce[0]+1,nonce[0]); nonce[0]+=8;
      if (numbytes==192) Ex24(X,Y,rk); 
      else{
	X[3]=X[0];
	Y[3]=SET(nonce[0]+7,nonce[0]+6,nonce[0]+3,nonce[0]+2,nonce[0]+5,nonce[0]+4,nonce[0]+1,nonce[0]); nonce[0]+=8;
	if (numbytes==256) Ex32(X,Y,rk); 
	else{
	  X[4]=X[0];
	  Y[4]=SET(nonce[0]+7,nonce[0]+6,nonce[0]+3,nonce[0]+2,nonce[0]+5,nonce[0]+4,nonce[0]+1,nonce[0]); nonce[0]+=8;
	  Ex40(X,Y,rk); 
	}
      }
    }
  }

  STORE(out,LOW(Y[0],X[0]));
  STORE(out+32,HIGH(Y[0],X[0]));

  if (numbytes>=128){
    STORE(out+64,LOW(Y[1],X[1]));
    STORE(out+96,HIGH(Y[1],X[1]));
  }
  if (numbytes>=192){
    STORE(out+128,LOW(Y[2],X[2]));
    STORE(out+160,HIGH(Y[2],X[2]));
  }
  if (numbytes>=256){
    STORE(out+192,LOW(Y[3],X[3]));
    STORE(out+224,HIGH(Y[3],X[3]));
  }
  if (numbytes>=320){
    STORE(out+256,LOW(Y[4],X[4]));
    STORE(out+288,HIGH(Y[4],X[4]));
  }

  return 0;
}



int crypto_stream_simon64128ctr_avx2_xor(
  unsigned char *out, 
  const unsigned char *in, 
  unsigned long long inlen, 
  const unsigned char *n, 
  const unsigned char *k
)
{
  int i;
  u32 nonce[2],K[4],key[44],x,y;
  unsigned char block[8];
  __m256i rk[44];

  if (!inlen) return 0;

  nonce[0]=((u32*)n)[0];
  nonce[1]=((u32*)n)[1];

  for(i=0;i<4;i++) K[i]=((u32 *)k)[i];

  ExpandKey(K,rk,key);

  if (inlen<=8){
    x=nonce[1]; y=nonce[0]; nonce[0]++;
    for(i=0;i<44;i+=2) R2(x,y,key[i],key[i+1]);
    ((u32*)block)[1]=x; ((u32*)block)[0]=y;
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
    Encrypt_Xor(out,in,nonce,rk,key,16);
    in+=16; inlen-=16; out+=16;
  }
  if (inlen>=8){
    Encrypt_Xor(out,in,nonce,rk,key,8);
    for(i=0;i<8;i++) out[i]^=in[i];
    in+=8; inlen-=8; out+=8;
  }
  if (inlen>0){ 
    Encrypt_Xor(block,in,nonce,rk,key,8);
    for (i=0;i<inlen;i++) out[i]=block[i]^in[i];
  }

  return 0;
}






inline int Encrypt_Xor(unsigned char *out, unsigned char *in, u32 nonce[],__m256i rk[], u32 key[],int numbytes)
{
  int i;
  u32 x[4],y[4];
  __m256i X[5],Y[5];


  if (numbytes==8){ 
    x[0]=nonce[1]; y[0]=nonce[0]; nonce[0]++;
    Ex1(x,y,key);
    ((u32 *)out)[1]=x[0]; ((u32 *)out)[0]=y[0];

    return 0;
  }

  if (numbytes==16){
    x[0]=nonce[1]; y[0]=nonce[0]; nonce[0]++;
    x[1]=nonce[1]; y[1]=nonce[0]; nonce[0]++;
    Ex2(x,y,key);
    ((u32 *)out)[1]=x[0]^((u32 *)in)[1]; ((u32 *)out)[0]=y[0]^((u32 *)in)[0];
    ((u32 *)out)[3]=x[1]^((u32 *)in)[3]; ((u32 *)out)[2]=y[1]^((u32 *)in)[2];

    return 0;
  }

  if (numbytes==32){
    x[0]=nonce[1]; y[0]=nonce[0]; nonce[0]++;
    x[1]=nonce[1]; y[1]=nonce[0]; nonce[0]++;
    x[2]=nonce[1]; y[2]=nonce[0]; nonce[0]++;
    x[3]=nonce[1]; y[3]=nonce[0]; nonce[0]++;
    Ex4(x,y,key);
    ((u32*)out)[1]=x[0]^((u32 *)in)[1]; ((u32*)out)[0]=y[0]^((u32 *)in)[0];
    ((u32*)out)[3]=x[1]^((u32 *)in)[3]; ((u32*)out)[2]=y[1]^((u32 *)in)[2];
    ((u32*)out)[5]=x[2]^((u32 *)in)[5]; ((u32*)out)[4]=y[2]^((u32 *)in)[4];
    ((u32*)out)[7]=x[3]^((u32 *)in)[7]; ((u32*)out)[6]=y[3]^((u32 *)in)[6];

    return 0;
  }
  
  X[0]=SET(nonce[1],nonce[1],nonce[1],nonce[1],nonce[1],nonce[1],nonce[1],nonce[1]);
  Y[0]=SET(nonce[0]+7,nonce[0]+6,nonce[0]+3,nonce[0]+2,nonce[0]+5,nonce[0]+4,nonce[0]+1,nonce[0]); nonce[0]+=8;

  if (numbytes==64) Ex8(X,Y,rk); 
  else{
    X[1]=X[0];
    Y[1]=SET(nonce[0]+7,nonce[0]+6,nonce[0]+3,nonce[0]+2,nonce[0]+5,nonce[0]+4,nonce[0]+1,nonce[0]); nonce[0]+=8;
    if (numbytes==128) Ex16(X,Y,rk); 
    else{
      X[2]=X[0];
      Y[2]=SET(nonce[0]+7,nonce[0]+6,nonce[0]+3,nonce[0]+2,nonce[0]+5,nonce[0]+4,nonce[0]+1,nonce[0]); nonce[0]+=8;
      if (numbytes==192) Ex24(X,Y,rk); 
      else{
	X[3]=X[0];
	Y[3]=SET(nonce[0]+7,nonce[0]+6,nonce[0]+3,nonce[0]+2,nonce[0]+5,nonce[0]+4,nonce[0]+1,nonce[0]); nonce[0]+=8;
	if (numbytes==256) Ex32(X,Y,rk); 
	else{
	  X[4]=X[0];
	  Y[4]=SET(nonce[0]+7,nonce[0]+6,nonce[0]+3,nonce[0]+2,nonce[0]+5,nonce[0]+4,nonce[0]+1,nonce[0]); nonce[0]+=8;
	  Ex40(X,Y,rk);
	}
      }
    }
  }
  
  STORE(out,XOR(LOAD(in),LOW(Y[0],X[0])));
  STORE(out+32,XOR(LOAD(in+32),HIGH(Y[0],X[0])));

  if (numbytes>=128){
    STORE(out+64,XOR(LOAD(in+64),LOW(Y[1],X[1])));
    STORE(out+96,XOR(LOAD(in+96),HIGH(Y[1],X[1])));
  }
  if (numbytes>=192){
    STORE(out+128,XOR(LOAD(in+128),LOW(Y[2],X[2])));
    STORE(out+160,XOR(LOAD(in+160),HIGH(Y[2],X[2])));
  }
  if (numbytes>=256){
    STORE(out+192,XOR(LOAD(in+192),LOW(Y[3],X[3])));
    STORE(out+224,XOR(LOAD(in+224),HIGH(Y[3],X[3])));
  }
  if (numbytes>=320){
    STORE(out+256,XOR(LOAD(in+256),LOW(Y[4],X[4])));
    STORE(out+288,XOR(LOAD(in+288),HIGH(Y[4],X[4])));
  }

  return 0;
}



int ExpandKey(u32 K[],__m256i rk[],u32 key[])
{
  u64 z3=0x0c2ce51207a635dbLL;
  u32 T,A,B,C,D;

  A=K[0]; key[0]=A; rk[0]=SET(A,A,A,A,A,A,A,A);
  B=K[1]; key[1]=B; rk[1]=SET(B,B,B,B,B,B,B,B);
  C=K[2]; key[2]=C; rk[2]=SET(C,C,C,C,C,C,C,C);
  D=K[3]; key[3]=D; rk[3]=SET(D,D,D,D,D,D,D,D);

  EK(A,B,C,D,rk,key);

  return 0;
}
