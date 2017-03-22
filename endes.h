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

    //���
    static void Do_XOR(unsigned char *dest,unsigned char *source,int size);

    //���
    void BitXor(unsigned char *Data1, unsigned char *Data2, int Len, unsigned char *Dest);

    //���üӽ��ܲ����Լ���Կ Decrypt: 0-���� 1-���� key:��Կ,8�ֽ�
    void DES_setkey( unsigned char sw,unsigned char *pkey);

    //DES����
    void DES_calcul(unsigned char *in,unsigned char *out);

    //�ӽ��ܺ��� Decrypt: 0-���� 1-���� key:��Կ,8�ֽ� Data:����,8�ֽ� Result:���,8�ֽ�
    void DES(unsigned char Decrypt,unsigned char *Key,unsigned char *Data,unsigned char *Result );

    //3DES�ӽ��ܺ��� Decrypt: 0-���� 1-���� key:��Կ,16�ֽ� Data:����,8�ֽ� Result:���,8�ֽ�
    void Triple_DES(unsigned char Decrypt,unsigned char* Key,unsigned char* Data,unsigned char* X_Data );

    //EBC 3DES
    void ECB_3DES(unsigned char Decrypt,unsigned char* Key,unsigned char* Data,int Data_Len, unsigned char* Result);

    //CBC 3DES
    int CBC_3DES(unsigned char Decrypt,unsigned char* Key,unsigned char* Init,unsigned char* Data,int Data_Len, unsigned char* Result);

    //CBC 3DES ����
    void En_CBC_3DES(unsigned char* Key,unsigned char* Init,unsigned char* Data,int Data_Len, unsigned char* Result);

    //CBC 3DES ����
    int De_CBC_3DES(unsigned char* Key,unsigned char* Init,unsigned char* Data,int Data_Len, unsigned char* Result);

    //MAC
    void MAC(unsigned char *Key, unsigned char *Src, int iSrcLen, unsigned char *Init, unsigned char *Dst);

    //MAC PBOC ǿ�Ʋ�80
    void MAC_PBOC(unsigned char *Key, unsigned char *Src, int iSrcLen, unsigned char *Init, unsigned char *Dst);

    //MAC 3DES
    void MAC_3DES(unsigned char *Key, unsigned char *Src, int iSrcLen, unsigned char *Init, unsigned char *Dst);

private:
    //Key schedule of 16 48-bit subkeys generated from 64-bit key
    unsigned char KS[16][48];

};

#endif // ENDES_H
