#include "endes.h"

EnDes::EnDes()
{

}

//PACK8()  Pack 64 unsigned chars at 1 bit/unsigned char into 8 unsigned chars at 8 bits/unsigned char
void EnDes::pack8(unsigned char *packed,unsigned char *binary)
{
   register unsigned short i, j, k;

   for( i = 0; i < 8; i++ ) {
      k = 0;
      for( j = 0; j < 8; j++ )
         k = ( k<<1 ) + *binary++;
      *packed++ = (unsigned char)k;
   }
}

//UNPACK8()  Unpack 8 unsigned chars at 8 bits/unsigned char into 64 unsigned chars at 1 bit/unsigned char
void EnDes::unpack8(unsigned char *packed,unsigned char *binary)
{
   register unsigned short i, j, k;

   for( i = 0; i < 8; i++ ) {
      k = *packed++;
      for( j = 0; j < 8; j++ )
         *binary++ = ( k >> (7-j) ) & 0x01;
   }
}

//异或
void EnDes::Do_XOR(unsigned char *dest, unsigned char *source,  int size)
{
    int i;
    for (i=0;i<size;i++)
    dest[i]^=source[i];
}

//异或
void EnDes::BitXor(unsigned char *Data1, unsigned char *Data2, int Len, unsigned char *Dest)
{
    int i;
    for (i = 0; i < Len; i++)
        Dest[i] = Data1[i] ^ Data2[i];
}

//设置加解密参数以及密钥 Decrypt: 0-加密 1-解密 key:密钥,8字节
void EnDes::DES_setkey( unsigned char sw,unsigned char *pkey)
{
   register unsigned short i, j, k, t1, t2;
   static   unsigned char key[64];
   static   unsigned char CD[56];

   /* Schedule of left shifts for C and D blocks */
   unsigned short shifts[] = { 1,1,2,2,2,2,2,2,1,2,2,2,2,2,2,1 };
   /* PERMUTED CHOICE 1 (PC1) */
   unsigned short PC1[] = {
      57, 49, 41, 33, 25, 17,  9,
       1, 58, 50, 42, 34, 26, 18,
      10,  2, 59, 51, 43, 35, 27,
      19, 11,  3, 60, 52, 44, 36,
      63, 55, 47, 39, 31, 23, 15,
      7, 62, 54, 46, 38, 30, 22,
      14,  6, 61, 53, 45, 37, 29,
      21, 13,  5, 28, 20, 12,  4
   };
   /* PERMUTED CHOICE 2 (PC2) */
   unsigned short PC2[] = {
      14, 17, 11, 24,  1,  5,
       3, 28, 15,  6, 21, 10,
      23, 19, 12,  4, 26,  8,
      16,  7, 27, 20, 13,  2,
      41, 52, 31, 37, 47, 55,
      30, 40, 51, 45, 33, 48,
      44, 49, 39, 56, 34, 53,
      46, 42, 50, 36, 29, 32
   };
   /* Assume default algorymth = crypt */
   if( sw != 0 && sw != 1 )
      sw = 0;
   /* Unpack KEY from 8 bits/unsigned char into 1 bit/unsigned char */
   unpack8( pkey, key );
   /* Permute unpacked key with PC1 to generate C and D */
   for( i = 0; i < 56; i++ )
      CD[i] = key[ PC1[i] - 1 ];
   /* Rotate and permute C and D to generate 16 subkeys */
   for( i = 0; i < 16; i++ ) {
      /* Rotate C and D */
      for( j = 0; j < shifts[i]; j++ ) {
         t1 = CD[0];
         t2 = CD[28];
         for( k = 0; k < 27; k++ ) {
            CD[k] = CD[k+1];
            CD[k+28] = CD[k+29];
         }
         CD[27] = (unsigned char)t1;
         CD[55] = (unsigned char)t2;
      }
      /* Set order of subkeys for type of cryption */
      j = sw ? 15-i : i;
      /* Permute C and D with PC2 to generate KS[j] */
      for( k = 0; k < 48; k++ )
         KS[j][k] = CD[ PC2[k] - 1 ];
   }
   return;
}

//DES计算
void EnDes::DES_calcul(unsigned char *in,unsigned char *out)
{
   register unsigned short i, j, k, t;
   static   unsigned char block[64];            /* unpacked 64-bit input/output block  */
   static   unsigned char LR[64], f[32], preS[48];

   /* INITIAL PERMUTATION (IP) */
   unsigned short IP[] = {
      58, 50, 42, 34, 26, 18, 10,  2,
      60, 52, 44, 36, 28, 20, 12,  4,
      62, 54, 46, 38, 30, 22, 14,  6,
      64, 56, 48, 40, 32, 24, 16,  8,
      57, 49, 41, 33, 25, 17,  9,  1,
      59, 51, 43, 35, 27, 19, 11,  3,
      61, 53, 45, 37, 29, 21, 13,  5,
      63, 55, 47, 39, 31, 23, 15,  7
   };
   /* REVERSE FINAL PERMUTATION (IP-1) */
   unsigned short RFP[] = {
      8, 40, 16, 48, 24, 56, 32, 64,
      7, 39, 15, 47, 23, 55, 31, 63,
      6, 38, 14, 46, 22, 54, 30, 62,
      5, 37, 13, 45, 21, 53, 29, 61,
      4, 36, 12, 44, 20, 52, 28, 60,
      3, 35, 11, 43, 19, 51, 27, 59,
      2, 34, 10, 42, 18, 50, 26, 58,
      1, 33,  9, 41, 17, 49, 25, 57
   };
   /* E BIT-SELECTION TABLE */
   unsigned short E[] = {
      32,  1,  2,  3,  4,  5,
       4,  5,  6,  7,  8,  9,
       8,  9, 10, 11, 12, 13,
      12, 13, 14, 15, 16, 17,
      16, 17, 18, 19, 20, 21,
      20, 21, 22, 23, 24, 25,
      24, 25, 26, 27, 28, 29,
      28, 29, 30, 31, 32,  1
   };
   /* PERMUTATION FUNCTION P */
   unsigned short P[] = {
      16,  7, 20, 21,
      29, 12, 28, 17,
       1, 15, 23, 26,
       5, 18, 31, 10,
       2,  8, 24, 14,
      32, 27,  3,  9,
      19, 13, 30,  6,
      22, 11,  4, 25
   };
   /* 8 S-BOXES */
   unsigned short S[8][64] = {
      14, 4,13, 1, 2,15,11, 8, 3,10, 6,12, 5, 9, 0, 7,
       0,15, 7, 4,14, 2,13, 1,10, 6,12,11, 9, 5, 3, 8,
       4, 1,14, 8,13, 6, 2,11,15,12, 9, 7, 3,10, 5, 0,
      15,12, 8, 2, 4, 9, 1, 7, 5,11, 3,14,10, 0, 6,13,

      15, 1, 8,14, 6,11, 3, 4, 9, 7, 2,13,12, 0, 5,10,
       3,13, 4, 7,15, 2, 8,14,12, 0, 1,10, 6, 9,11, 5,
       0,14, 7,11,10, 4,13, 1, 5, 8,12, 6, 9, 3, 2,15,
      13, 8,10, 1, 3,15, 4, 2,11, 6, 7,12, 0, 5,14, 9,

      10, 0, 9,14, 6, 3,15, 5, 1,13,12, 7,11, 4, 2, 8,
      13, 7, 0, 9, 3, 4, 6,10, 2, 8, 5,14,12,11,15, 1,
      13, 6, 4, 9, 8,15, 3, 0,11, 1, 2,12, 5,10,14, 7,
       1,10,13, 0, 6, 9, 8, 7, 4,15,14, 3,11, 5, 2,12,

       7,13,14, 3, 0, 6, 9,10, 1, 2, 8, 5,11,12, 4,15,
      13, 8,11, 5, 6,15, 0, 3, 4, 7, 2,12, 1,10,14, 9,
      10, 6, 9, 0,12,11, 7,13,15, 1, 3,14, 5, 2, 8, 4,
       3,15, 0, 6,10, 1,13, 8, 9, 4, 5,11,12, 7, 2,14,

       2,12, 4, 1, 7,10,11, 6, 8, 5, 3,15,13, 0,14, 9,
      14,11, 2,12, 4, 7,13, 1, 5, 0,15,10, 3, 9, 8, 6,
       4, 2, 1,11,10,13, 7, 8,15, 9,12, 5, 6, 3, 0,14,
      11, 8,12, 7, 1,14, 2,13, 6,15, 0, 9,10, 4, 5, 3,

      12, 1,10,15, 9, 2, 6, 8, 0,13, 3, 4,14, 7, 5,11,
      10,15, 4, 2, 7,12, 9, 5, 6, 1,13,14, 0,11, 3, 8,
       9,14,15, 5, 2, 8,12, 3, 7, 0, 4,10, 1,13,11, 6,
       4, 3, 2,12, 9, 5,15,10,11,14, 1, 7, 6, 0, 8,13,

       4,11, 2,14,15, 0, 8,13, 3,12, 9, 7, 5,10, 6, 1,
      13, 0,11, 7, 4, 9, 1,10,14, 3, 5,12, 2,15, 8, 6,
       1, 4,11,13,12, 3, 7,14,10,15, 6, 8, 0, 5, 9, 2,
       6,11,13, 8, 1, 4,10, 7, 9, 5, 0,15,14, 2, 3,12,

      13, 2, 8, 4, 6,15,11, 1,10, 9, 3,14, 5, 0,12, 7,
       1,15,13, 8,10, 3, 7, 4,12, 5, 6,11, 0,14, 9, 2,
       7,11, 4, 1, 9,12,14, 2, 0, 6,10,13,15, 3, 5, 8,
       2, 1,14, 7, 4,10, 8,13,15,12, 9, 0, 3, 5, 6, 11
   };
   /* Unpack the INPUT block */
   unpack8( in, block );

   /* Permute unpacked input block with IP to generate L and R */
   for( j = 0; j < 64; j++ )
      LR[j] = block[ IP[j] - 1 ];
   /* Perform 16 rounds */
   for( i = 0; i < 16; i++ ) {
      /* Expand R to 48 bits with E and XOR with i-th subkey */
      for( j = 0; j < 48; j++ )
         preS[j] = LR[E[j]+31] ^ KS[i][j];

      /* Map 8 6-bit blocks into 8 4-bit blocks using S-Boxes */
      for( j = 0; j < 8; j++ ) {
         /* Compute index t into j-th S-box */
         k = 6 * j;
         t = preS[k];
         t = ( t << 1 ) | preS[k+5];
         t = ( t << 1 ) | preS[k+1];
         t = ( t << 1 ) | preS[k+2];
         t = ( t << 1 ) | preS[k+3];
         t = ( t << 1 ) | preS[k+4];
         /* Fetch t-th entry from j-th S-box */
         t = S[j][t];
         /* Generate 4-bit block from S-box entry */
         k = 4 * j;
         f[k]   = ( t >> 3 ) & 1;
         f[k+1] = ( t >> 2 ) & 1;
         f[k+2] = ( t >> 1 ) & 1;
         f[k+3] = t & 1;
      }
      for( j = 0; j < 32; j++ ) {
         /* Copy R */
         t = LR[j+32];
         /* Permute f w/ P and XOR w/ L to generate new R */
         LR[j+32] = LR[j] ^ f[P[j]-1];
         /* Copy original R to new L */
         LR[j] = (unsigned char)t;
      }
   }

   /* Permute L and R with reverse IP-1 to generate output block */
   for( j = 0; j < 64; j++ )
      block[j] = LR[ RFP[j] - 1 ];
   /* Pack data into 8 bits per unsigned char */
   pack8( out, block );
}

//加解密函数 Decrypt: 0-加密 1-解密 key:密钥,8字节 Data:数据,8字节 Result:结果,8字节
void EnDes::DES( unsigned char Decrypt, unsigned char *Key, unsigned char *Data, unsigned char *Result )
{
   /* Set decryption or encryption key parameter for DES calculation */
   DES_setkey( Decrypt, Key );
   /* Decrypt or Encrypt the data and replace */
   DES_calcul( Data, Result );
   return;
}

//3DES加解密函数 Decrypt: 0-加密 1-解密 key:密钥,16字节 Data:数据,8字节 Result:结果,8字节
void EnDes::Triple_DES( unsigned char Decrypt, unsigned char* Key, unsigned char* Data, unsigned char* Result )
{
   unsigned char  Tmp1[16], Tmp2[16];

   if( Decrypt == ENCRYPT ) {
      DES( ENCRYPT, Key    , Data, Tmp1   );
      DES( DECRYPT, Key + 8, Tmp1, Tmp2   );
      DES( ENCRYPT, Key    , Tmp2, Result );
   }
   else {
      DES( DECRYPT, Key    , Data, Tmp1   );
      DES( ENCRYPT, Key + 8, Tmp1, Tmp2   );
      DES( DECRYPT, Key    , Tmp2, Result );
   }
   return;
}

//ECB_3DES
void EnDes::ECB_3DES(unsigned char Decrypt,unsigned char* Key,unsigned char* Data,int Data_Len, unsigned char* Result)
{
    int ngroup;
    unsigned char temp[256];
    int i;
    if (Data_Len % 8)
    {
        ngroup = Data_Len/8 + 1;
        memset(temp,0x00,sizeof(temp));
        memcpy(temp,Data,Data_Len);
        memcpy(temp+Data_Len,"\x80",1);
    }
    else
    {
        ngroup = Data_Len / 8;
        memset(temp,0x00,sizeof(temp));
        memcpy(temp,Data,Data_Len);
    }
    for (i=0;i<ngroup;i++)
    {
        Triple_DES(Decrypt,Key,temp+(8*i),Result+(8*i));
    }
    return;
}

//CBC 3DES
int EnDes::CBC_3DES(unsigned char Decrypt,unsigned char* Key,unsigned char* Init,unsigned char* Data,int Data_Len, unsigned char* Result)
{
    if (Decrypt == 0) En_CBC_3DES(Key,Init,Data,Data_Len,Result);
    else if (Decrypt == 1) return De_CBC_3DES(Key,Init,Data,Data_Len,Result);

    return 0;
}

//CBC 3DES 加密
void EnDes::En_CBC_3DES(unsigned char* Key,unsigned char* Init,unsigned char* Data,int Data_Len, unsigned char* Result)
{
    int ngroup;
    unsigned char temp[256];
    unsigned char byInit[9]={'\0'};
    unsigned char bytDst[9] ={'\0'};
    unsigned char byScr[9]={'\0'};
    char flag=false;
    int i;
    memcpy(byInit,Init,8);
    if (Data_Len % 8)
    {
        ngroup = Data_Len/8 + 1;
        memset(temp,0x00,sizeof(temp));
        memcpy(temp,Data,Data_Len);
        memcpy(temp+Data_Len,"\x80",1);
    }
    else
    {
        ngroup = Data_Len / 8;
        memset(temp,0x00,sizeof(temp));
        memcpy(temp,Data,Data_Len);
    }

    for (i=0;i<ngroup;i++)
    {
        memset(byScr,0x00,sizeof(byScr));
        memcpy(byScr,temp+(8*i),8);
        if(flag)
        {
            Do_XOR(byScr,bytDst,8);
        }
        else
        {
            Do_XOR(byScr,byInit,8);   //开始初始化
        }

        Triple_DES(0,Key,byScr,Result+(8*i));

        memset(bytDst,0x00,sizeof(bytDst));
        memcpy(bytDst,Result+(8*i),8);
        flag=true;
    }
    return;
}

//CBC 3DES 解密
int EnDes::De_CBC_3DES(unsigned char* Key,unsigned char* Init,unsigned char* Data,int Data_Len, unsigned char* Result)
{
    int i;
    unsigned char ucInit[8];
    unsigned char ucSrc[8];

    if (Data_Len%8 != 0) return -1;

    memcpy(ucInit,Init,8);

    for (i = 0;i < Data_Len/8;i++)
    {
        Triple_DES(1,Key,Data + i*8,ucSrc);
        BitXor(ucSrc,ucInit,8,Result + i*8);
        memcpy(ucInit,Data + i*8,8);
    }

    return 0;
}

//MAC
void EnDes::MAC(unsigned char *Key, unsigned char *Src, int iSrcLen, unsigned char *Init, unsigned char *Dst)
{
    unsigned char	pbySrcTemp[8];
    unsigned char	pbyInitData[8];
    unsigned char	pbyDeaSrc[8];
    unsigned char	KeyA[8];
    unsigned char	KeyB[8];

    unsigned char pbyKey[16] = {'\0'};
    unsigned char pbyInit[8]={'\0'};
    unsigned char pbySrc[256] = {'\0'};
    unsigned char pbyDst[8] = {'\0'};

    memcpy(pbyKey,Key,16);
    memcpy(pbySrc,Src,iSrcLen);
    memcpy(pbyInit,Init,8);

    int	i, j, n, iAppend;
    n = iSrcLen / 8;
    iAppend = 8 - (iSrcLen - n * 8);

    memcpy (pbyInitData,pbyInit,8);
    memcpy (KeyA,pbyKey,8);
    memcpy (KeyB,&pbyKey[8],8);

    for(i = 0; i <= n; i++)
    {
        memset(pbySrcTemp, 0x00, 8);
        if(i==n)
        {
            if (iAppend == 8) break;
            memcpy(pbySrcTemp, &pbySrc[i*8],8 - iAppend);
            memcpy(pbySrcTemp + 8 - iAppend,"\x80",1);
            for (j = iAppend; j < 8; j++) pbySrcTemp[j] = 0x00;
        }
        else memcpy(pbySrcTemp, &pbySrc[i*8], 8);
        BitXor(pbySrcTemp, pbyInitData, 8, pbyDeaSrc);
        memset(pbyInitData, 0x00, 8);
        DES(0,KeyA, pbyDeaSrc, pbyInitData);

    }
    DES(1,KeyB, pbyInitData, pbyDeaSrc);
    DES(0,KeyA, pbyDeaSrc, pbyInitData);

    memcpy(pbyDst, pbyInitData, 8);

    memcpy(Dst,pbyDst,8);
}

//MAC PBOC 强制补80
void EnDes::MAC_PBOC(unsigned char *Key, unsigned char *Src, int iSrcLen, unsigned char *Init, unsigned char *Dst)
{
    unsigned char	pbySrcTemp[8];
    unsigned char	pbyInitData[8];
    unsigned char	pbyDeaSrc[8];
    unsigned char	KeyA[8];
    unsigned char	KeyB[8];

    unsigned char pbyKey[16] = {'\0'};
    unsigned char pbyInit[8]={'\0'};
    unsigned char pbySrc[256] = {'\0'};
    unsigned char pbyDst[8] = {'\0'};

    memcpy(pbyKey,Key,16);
    memcpy(pbySrc,Src,iSrcLen);
    memcpy(pbyInit,Init,8);

    int	i, j, n, iAppend;
    n = iSrcLen / 8 + 1;
    iAppend = 8 - ( n * 8 - iSrcLen);

    memcpy (pbyInitData,pbyInit,8);
    memcpy (KeyA,pbyKey,8);
    memcpy (KeyB,&pbyKey[8],8);

    for(i = 0; i < n; i++)
    {
        memset(pbySrcTemp, 0x00, 8);
        if(i==(n-1))
        {
            memcpy(pbySrcTemp, &pbySrc[i*8], iAppend);
            pbySrcTemp[iAppend] = 0x80;
            for (j = iAppend + 1; j < 8; j++) pbySrcTemp[j] = 0x00;
        }
        else memcpy(pbySrcTemp, &pbySrc[i*8], 8);
        BitXor(pbySrcTemp, pbyInitData, 8, pbyDeaSrc);
        memset(pbyInitData, 0x00, 8);
        DES(0,KeyA, pbyDeaSrc, pbyInitData);

    }
    DES(1,KeyB, pbyInitData, pbyDeaSrc);
    DES(0,KeyA, pbyDeaSrc, pbyInitData);

    memcpy(pbyDst, pbyInitData, 8);

    memcpy(Dst,pbyDst,8);
}

//MAC 3DES
void EnDes::MAC_3DES(unsigned char *Key, unsigned char *Src, int iSrcLen, unsigned char *Init, unsigned char *Dst)
{
    unsigned char pbySrcTemp[8];
    unsigned char pbyInitData[8];
    unsigned char pbyDeaSrc[8];
    unsigned char pbyInit[8]={'\0'};
    unsigned char pbySrc[256] = {'\0'};
    unsigned char pbyDst[8] = {'\0'};

    memcpy(pbySrc,Src,iSrcLen);
    memcpy(pbyInit,Init,8);

    int	i, j, n, iAppend;
    n = iSrcLen / 8;
    iAppend = 8 - (iSrcLen - n * 8);

    memcpy (pbyInitData,pbyInit,8);

    for(i = 0; i <= n; i++)
    {
        memset(pbySrcTemp, 0x00, 8);
        if(i==n)
        {
            if (iAppend == 8) break;
            memcpy(pbySrcTemp, &pbySrc[i*8],8 - iAppend);
            memcpy(pbySrcTemp + 8 - iAppend,"\x80",1);
            for (j = iAppend; j < 8; j++) pbySrcTemp[j] = 0x00;
        }
        else memcpy(pbySrcTemp, &pbySrc[i*8], 8);
        BitXor(pbySrcTemp, pbyInitData, 8, pbyDeaSrc);
        memset(pbyInitData, 0x00, 8);
        Triple_DES(0,Key, pbyDeaSrc, pbyInitData);

    }
    Triple_DES(1,Key, pbyInitData, pbyDeaSrc);
    Triple_DES(0,Key, pbyDeaSrc, pbyInitData);

    memcpy(pbyDst, pbyInitData, 8);

    memcpy(Dst,pbyDst,8);
}
