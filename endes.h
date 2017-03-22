#ifndef ENDES_H
#define ENDES_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "unionpaystructure.h"

class EnDes
{
public:
    EnDes();

public:
    //PACK8 Pack 64 unsigned chars at 1 bit/unsigned char into 8 unsigned chars at 8 bits/unsigned char
    static void pack8(unsigned char *packed,unsigned char *binary);

    //UNPACK8 Unpack 8 unsigned chars at 8 bits/unsigned char into 64 unsigned chars at 1 bit/unsigned char
    static void unpack8(unsigned char *packed,unsigned char *binary);

    //异或
    static void Do_XOR(unsigned char *dest,unsigned char *source,int size);

    //异或
    void BitXor(unsigned char *Data1, unsigned char *Data2, int Len, unsigned char *Dest);

    //设置加解密参数以及密钥 Decrypt: 0-加密 1-解密 key:密钥,8字节
    void DES_setkey( unsigned char sw,unsigned char *pkey);

    //DES计算
    void DES_calcul(unsigned char *in,unsigned char *out);

    //加解密函数 Decrypt: 0-加密 1-解密 key:密钥,8字节 Data:数据,8字节 Result:结果,8字节
    void DES(unsigned char Decrypt,unsigned char *Key,unsigned char *Data,unsigned char *Result );

    //3DES加解密函数 Decrypt: 0-加密 1-解密 key:密钥,16字节 Data:数据,8字节 Result:结果,8字节
    void Triple_DES(unsigned char Decrypt,unsigned char* Key,unsigned char* Data,unsigned char* X_Data );

    //EBC 3DES
    void ECB_3DES(unsigned char Decrypt,unsigned char* Key,unsigned char* Data,int Data_Len, unsigned char* Result);

    //CBC 3DES
    int CBC_3DES(unsigned char Decrypt,unsigned char* Key,unsigned char* Init,unsigned char* Data,int Data_Len, unsigned char* Result);

    //CBC 3DES 加密
    void En_CBC_3DES(unsigned char* Key,unsigned char* Init,unsigned char* Data,int Data_Len, unsigned char* Result);

    //CBC 3DES 解密
    int De_CBC_3DES(unsigned char* Key,unsigned char* Init,unsigned char* Data,int Data_Len, unsigned char* Result);

    //MAC
    void MAC(unsigned char *Key, unsigned char *Src, int iSrcLen, unsigned char *Init, unsigned char *Dst);

    //MAC PBOC 强制补80
    void MAC_PBOC(unsigned char *Key, unsigned char *Src, int iSrcLen, unsigned char *Init, unsigned char *Dst);

    //MAC 3DES
    void MAC_3DES(unsigned char *Key, unsigned char *Src, int iSrcLen, unsigned char *Init, unsigned char *Dst);

private:
    //Key schedule of 16 48-bit subkeys generated from 64-bit key
    unsigned char KS[16][48];

};

#endif // ENDES_H
