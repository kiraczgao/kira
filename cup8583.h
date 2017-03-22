#ifndef CUP8583_H
#define CUP8583_H

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include "endes.h"
#include "unionpaystructure.h"

class CUP8583 : public EnDes
{
public:
    CUP8583();

public:
    //����ĳ�����ֵ
    int CUP8583_SetField(CUP8583_MSG* pio_Msg,int i_iFieldNum,const char* pi_chFieldData,int i_iFieldLen);

    //��ȡĳ�����ֵ
    int CUP8583_GetField(CUP8583_MSG* pi_Msg,int i_iFieldNum,unsigned char* pio_ucFieldBuf,int* pio_iFieldBufLen);

    //ɾ��ĳ������
    int CUP8583_DeleteFeild(CUP8583_MSG* pio_Msg,int i_iFieldNum);

    //��ȡ����
    int CUP8583_GetBuf(CUP8583_MSG* pi_Msg,unsigned char* pio_ucMsgBuf,int* pio_iMsgBufLen);

    //��ȡ���Ľṹ
    int CUP8583_GetMsg(CUP8583_MSG* pio_Msg,const unsigned char* pi_ucMsgBuf,int i_iMsgBufLen);

    //��ȡĳ�����ֵ,ASC��ʽ
    int CUP8583_GetField_ASC(CUP8583_MSG* pi_Msg,int i_iFieldNum,unsigned char* pio_ucFieldBuf,int* pio_iFieldBufLen);

    //���㱨��MAC
    int CalMsgMac(unsigned char* pi_ucMak,unsigned char* pi_ucMsgData,int i_iMsgDataLen,unsigned char* pio_ucMac);

    //AsctoHex
    unsigned char* AsctoHex(const unsigned char *Data, int Len,unsigned char* Result);

    //HextoAsc
    unsigned char* HextoAsc(const unsigned char *Data, int len,unsigned char* Result);

    int CalMsgMacEn(unsigned char* pi_ucMak,unsigned char* pi_ucMsgData,int i_iMsgDataLen,unsigned char* pio_ucMac);
    int CalMsgcheck(unsigned char* pi_ucMak,unsigned char* key_index,unsigned char* pi_ucMsgData, int i_iMsgDataLen, unsigned char* pio_ucMac);
};

#endif // CUP8583_H
