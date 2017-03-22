#include "cup8583.h"

struct CUP8583_DEF Cup8583[65] =
{
    {   4, T_NUM, F_FIX_LEN},		/* �� 0  ��Ϣ����           */
    {  16, T_NUM, F_FIX_LEN},		/* �� 1  λͼ               */
    {  19,  T_NS, F_LLVAR_LEN},	/* �� 2  ���˺�             */
    {   6,  T_NS, F_FIX_LEN},		/* �� 3  ���״�����         */
    {  12, T_NUM, F_FIX_LEN},		/* �� 4  ���׽��           */
    {  12, T_NUM, F_FIX_LEN},		/* �� 5  ������           */
    {  12, T_NUM, F_FIX_LEN},		/* �� 6  �ֿ��˿��˽��     */
    {  10,  T_NS, F_FIX_LEN},		/* �� 7  ���״���ʱ��       */
    {   0, T_NUM, F_FIX_LEN},		/* �� 8                     */
    {   8, T_NUM, F_FIX_LEN},		/* �� 9  �������           */
    {   8, T_NUM, F_FIX_LEN},		/* �� 10 �ֿ��˿��˻���     */
    {   6, T_NUM, F_FIX_LEN},		/* �� 11 ϵͳ���ٺ�         */
    {   6,  T_NS, F_FIX_LEN},		/* �� 12 �ܿ������ڵ�ʱ��   */
    {   4,  T_NS, F_FIX_LEN},		/* �� 13 �ܿ������ڵ�����   */
    {   4,  T_NS, F_FIX_LEN},		/* �� 14 ����Ч��           */
    {   4,  T_NS, F_FIX_LEN},		/* �� 15 ��������           */
    {   4,  T_NS, F_FIX_LEN},		/* �� 16 �һ�����           */
    {   0, T_NUM, F_FIX_LEN},		/* �� 17                    */
    {   4,  T_NS, F_FIX_LEN},		/* �� 18 �̻�����           */
    {   3, T_NUM, F_FIX_LEN},		/* �� 19 ����������Ҵ���   */
    {   0, T_NUM, F_FIX_LEN},		/* �� 20                    */
    {   0, T_NUM, F_FIX_LEN},		/* �� 21                    */
    {   3,  T_NS, F_FIX_LEN},		/* �� 22 ��������뷽ʽ��   */
    {   3, T_NUM, F_FIX_LEN},		/* �� 23 �����к�           */
    {   0, T_NUM, F_FIX_LEN},		/* �� 24                    */
    {   2,  T_NS, F_FIX_LEN},		/* �� 25 �����������       */
    {   2, T_NUM, F_FIX_LEN},		/* �� 26 ����� PIN ��ȡ��  */
    {   0, T_NUM, F_LLVAR_LEN},		/* �� 27                    */
    {   9,  T_XN, F_FIX_LEN},		/* �� 28 ���׷�             */
    {   0, T_NUM, F_LLVAR_LEN},		/* �� 29                    */
    {   0, T_NUM, F_LLVAR_LEN},		/* �� 30                    */
    {   0, T_NUM, F_LLVAR_LEN},		/* �� 31                    */
    {  11, T_NUM, F_LLVAR_LEN},		/* �� 32 ���������ʶ��     */
    {  11, T_NUM, F_LLVAR_LEN},		/* �� 33 ���ͻ�����ʶ��     */
    {   0, T_NUM, F_LLLVAR_LEN},	/* �� 34                    */
    {  37, T_Z, F_LLVAR_LEN},		/* �� 35 �ڶ��ŵ�����       */
    { 104, T_Z, F_LLLVAR_LEN},		/* �� 36 �����ŵ�����       */
    {  12, T_ANS, F_FIX_LEN},		/* �� 37 �����ο���         */
    {   6, T_ANS, F_FIX_LEN},		/* �� 38 ��Ȩ��ʶӦ����     */
    {   2,  T_AN, F_FIX_LEN},		/* �� 39 Ӧ����             */
    {   0, T_NUM, F_LLVAR_LEN},		/* �� 40                    */
    {   8, T_ANS, F_FIX_LEN},		/* �� 41 �ܿ����ն˱�ʶ��   */
    {  15, T_ANS, F_FIX_LEN},		/* �� 42 �ܿ�����ʶ��       */
    {  40, T_ANS, F_FIX_LEN},		/* �� 43 �ܿ������Ƶ�ַ     */
    {  25, T_ANS, F_LLVAR_LEN},		/* �� 44 ������Ӧ����       */
    {  79,   T_Z, F_LLVAR_LEN},		/* �� 45 ��һ�ŵ�����       */
    {   0, T_NUM, F_LLLVAR_LEN},	/* �� 46                    */
    {   0, T_NUM, F_LLLVAR_LEN},	/* �� 47                    */
    { 322, T_NS, F_LLLVAR_LEN},	/* �� 48 �������ݣ�˽��     */
    {   3,  T_AN, F_FIX_LEN},		/* �� 49 ���׻��Ҵ���       */
    {   3,  T_AN, F_FIX_LEN},		/* �� 50 ������Ҵ���       */
    {   3,  T_AN, F_FIX_LEN},		/* �� 51 �ֿ����˻����Ҵ��� */
    {   8, T_BIT, F_FIX_LEN},		/* �� 52 ���˱�ʶ������     */
    {  16, T_NUM, F_FIX_LEN},		/* �� 53 ��ȫ������Ϣ       */
    {  40,  T_AN, F_LLLVAR_LEN},	/* �� 54 ʵ�����           */
    { 255, T_BCD, F_LLLVAR_LEN},	/* �� 55 ���� PBOC ����Ǳ�׼��IC�������� */
    {   0, T_NUM, F_LLLVAR_LEN},	/* �� 56                    */
    { 100, T_ANS, F_LLLVAR_LEN},	/* �� 57 ���ӽ�����Ϣ       */
    { 100, T_ANS, F_LLLVAR_LEN},	/* �� 58 ����PBOC ����Ǯ��/����IC����׼�Ľ������� */
    { 600, T_ANS, F_LLLVAR_LEN},	/* �� 59 ��ϸ��ѯ����       */
    {  17,  T_NS, F_LLLVAR_LEN},	/* �� 60 �Զ�����           */
    {  29,  T_NS, F_LLLVAR_LEN},	/* �� 61 ԭʼ��Ϣ��         */
    { 512, T_ANS, F_LLLVAR_LEN},	/* �� 62 �Զ�����           */
    { 163, T_ANS, F_LLLVAR_LEN},	/* �� 63 �Զ�����           */
    {   8, T_BIT, F_FIX_LEN},		/* �� 64 ���ļ�����         */
};

CUP8583::CUP8583()
{

}

//����ĳ�����ֵ
int CUP8583::CUP8583_SetField(CUP8583_MSG* pio_Msg, int i_iFieldNum, const char* pi_chFieldData, int i_iFieldLen)
{
    int iLength;
    unsigned char ucTemp[256];
    unsigned char ucLength[5];

    CUP8583_DeleteFeild(pio_Msg, i_iFieldNum);

    //�������ȷ��
    if ((i_iFieldNum < 0) || (i_iFieldNum > 128) || (i_iFieldNum == 1)) return -1;

    //��鳤����ȷ��
    if ((i_iFieldLen < 0) || (i_iFieldLen > Cup8583[i_iFieldNum].Length)) return -2;

    memset(ucLength, 0, sizeof(ucLength));
    memset(pio_Msg->Field[i_iFieldNum].Buf, 0, sizeof(pio_Msg->Field[i_iFieldNum].Buf));

    //��������
    if (i_iFieldNum == 0)
    {
        if (i_iFieldLen != 4) return -2;
        AsctoHex((unsigned char*)pi_chFieldData, 4, pio_Msg->Field[i_iFieldNum].Buf);
        pio_Msg->Field[i_iFieldNum].Length = 2;
    }

    if (Cup8583[i_iFieldNum].Flag == F_LLVAR_LEN)
    {
        sprintf((char*)ucLength, "%02d", i_iFieldLen);
        AsctoHex(ucLength, 2, pio_Msg->Field[i_iFieldNum].Buf);
        pio_Msg->Field[i_iFieldNum].Length = 1;
    }
    else if (Cup8583[i_iFieldNum].Flag == F_LLLVAR_LEN)
    {
        sprintf((char*)ucLength, "%04d", i_iFieldLen);
        AsctoHex(ucLength, 4, pio_Msg->Field[i_iFieldNum].Buf);
        pio_Msg->Field[i_iFieldNum].Length = 2;
    }

    if ((Cup8583[i_iFieldNum].Type == T_NS) || (Cup8583[i_iFieldNum].Type == T_Z))
    {
        if (Cup8583[i_iFieldNum].Flag == F_FIX_LEN)
        {
            if (Cup8583[i_iFieldNum].Length % 2 != 0) iLength = Cup8583[i_iFieldNum].Length + 1;
            else iLength = Cup8583[i_iFieldNum].Length;
        }
        else if ((Cup8583[i_iFieldNum].Flag == F_LLVAR_LEN) || (Cup8583[i_iFieldNum].Flag == F_LLLVAR_LEN))
        {
            if (i_iFieldLen % 2 != 0) iLength = i_iFieldLen + 1;
            else iLength = i_iFieldLen;
        }

        //�����Ҳ�0
        memset(ucTemp, '0', iLength);
        memcpy(ucTemp, pi_chFieldData, i_iFieldLen);
        AsctoHex(ucTemp, iLength, pio_Msg->Field[i_iFieldNum].Buf + pio_Msg->Field[i_iFieldNum].Length);
        pio_Msg->Field[i_iFieldNum].Length += iLength/2;
    }
    else if (Cup8583[i_iFieldNum].Type == T_NUM)
    {
        if (Cup8583[i_iFieldNum].Flag == F_FIX_LEN)
        {
            if (Cup8583[i_iFieldNum].Length % 2 != 0) iLength = Cup8583[i_iFieldNum].Length + 1;
            else iLength = Cup8583[i_iFieldNum].Length;

            //�ҿ�����0
            memset(ucTemp, '0', iLength);
            memcpy(ucTemp + iLength - i_iFieldLen, pi_chFieldData, i_iFieldLen);
        }
        else if ((Cup8583[i_iFieldNum].Flag == F_LLVAR_LEN) || (Cup8583[i_iFieldNum].Flag == F_LLLVAR_LEN))
        {
            if (i_iFieldLen % 2 != 0) iLength = i_iFieldLen + 1;
            else iLength = i_iFieldLen;

            //�����Ҳ�0
            memset(ucTemp, '0', iLength);
            memcpy(ucTemp, pi_chFieldData, i_iFieldLen);
        }

        AsctoHex(ucTemp, iLength, pio_Msg->Field[i_iFieldNum].Buf + pio_Msg->Field[i_iFieldNum].Length);
        pio_Msg->Field[i_iFieldNum].Length += iLength/2;
    }
    else if ((Cup8583[i_iFieldNum].Type == T_AN) || (Cup8583[i_iFieldNum].Type == T_ANS) || (Cup8583[i_iFieldNum].Type == T_ANSB) || (Cup8583[i_iFieldNum].Type == T_XN))
    {
        if (Cup8583[i_iFieldNum].Flag == F_FIX_LEN)
        {
            iLength = Cup8583[i_iFieldNum].Length;
        }
        else if ((Cup8583[i_iFieldNum].Flag == F_LLVAR_LEN) || (Cup8583[i_iFieldNum].Flag == F_LLLVAR_LEN))
        {
            iLength = i_iFieldLen;
        }

        memset(pio_Msg->Field[i_iFieldNum].Buf + pio_Msg->Field[i_iFieldNum].Length, ' ', iLength);
        memcpy(pio_Msg->Field[i_iFieldNum].Buf + pio_Msg->Field[i_iFieldNum].Length, pi_chFieldData, i_iFieldLen);

        pio_Msg->Field[i_iFieldNum].Length += iLength;
    }
    else if ((Cup8583[i_iFieldNum].Type == T_BIT) || (Cup8583[i_iFieldNum].Type == T_BCD))
    {
        if (Cup8583[i_iFieldNum].Flag == F_FIX_LEN)
        {
            iLength = Cup8583[i_iFieldNum].Length;
        }
        else if ((Cup8583[i_iFieldNum].Flag == F_LLVAR_LEN) || (Cup8583[i_iFieldNum].Flag == F_LLLVAR_LEN))
        {
            iLength = i_iFieldLen;
        }

        memset(pio_Msg->Field[i_iFieldNum].Buf + pio_Msg->Field[i_iFieldNum].Length, 0x00, iLength);
        memcpy(pio_Msg->Field[i_iFieldNum].Buf + pio_Msg->Field[i_iFieldNum].Length, pi_chFieldData, i_iFieldLen);

        pio_Msg->Field[i_iFieldNum].Length += iLength;
    }
    else
    {
        return -3;
    }


    pio_Msg->Field[1].StateFlag = 1;
    pio_Msg->Field[1].Length = 8;
    pio_Msg->Field[1].Buf[(i_iFieldNum - 1)/8] |= (unsigned char)pow(2,(7 - (i_iFieldNum - 1)%8));
    pio_Msg->Field[i_iFieldNum].StateFlag = 1;

    return 0;
}

//��ȡĳ�����ֵ
int CUP8583::CUP8583_GetField(CUP8583_MSG* pi_Msg, int i_iFieldNum, unsigned char* pio_ucFieldBuf, int* pio_iFieldBufLen)
{
    int iLength,iOffset;
    unsigned char ucTemp[1024];

    if (i_iFieldNum < 0) return -1;

    if (pi_Msg->Field[i_iFieldNum].StateFlag != 1) return -2;

    if (Cup8583[i_iFieldNum].Flag == F_LLVAR_LEN)
    {
        memset(ucTemp, 0, sizeof(ucTemp));
        HextoAsc(pi_Msg->Field[i_iFieldNum].Buf, 1, ucTemp);
        iLength = atoi((char*)ucTemp);
        iOffset = 1;
    }
    else if (Cup8583[i_iFieldNum].Flag == F_LLLVAR_LEN)
    {
        memset(ucTemp, 0, sizeof(ucTemp));
        HextoAsc(pi_Msg->Field[i_iFieldNum].Buf, 2, ucTemp);
        iLength = atoi((char*)ucTemp);
        iOffset = 2;
    }
    else if (Cup8583[i_iFieldNum].Flag == F_FIX_LEN)
    {
        //iLength = pi_Msg->Field[i_iFieldNum].Length;
        iLength = Cup8583[i_iFieldNum].Length;
        iOffset = 0;
    }

    if (Cup8583[i_iFieldNum].Type == T_NUM)
    {
        memset(ucTemp, 0, sizeof(ucTemp));
        HextoAsc(pi_Msg->Field[i_iFieldNum].Buf + iOffset,pi_Msg->Field[i_iFieldNum].Length - iOffset,ucTemp);

        if (Cup8583[i_iFieldNum].Flag == F_FIX_LEN)
            memcpy(pio_ucFieldBuf, ucTemp + strlen((char*)ucTemp) - iLength, iLength);
        else
            memcpy(pio_ucFieldBuf,ucTemp,iLength);
    }
    else if ((Cup8583[i_iFieldNum].Type == T_NS) || (Cup8583[i_iFieldNum].Type == T_Z))
    {
        memset(ucTemp, 0, sizeof(ucTemp));
        HextoAsc(pi_Msg->Field[i_iFieldNum].Buf + iOffset, pi_Msg->Field[i_iFieldNum].Length - iOffset, ucTemp);
        memcpy(pio_ucFieldBuf, ucTemp, iLength);
    }
    else
    {
        memcpy(pio_ucFieldBuf, pi_Msg->Field[i_iFieldNum].Buf + iOffset, iLength);
    }

    *pio_iFieldBufLen = iLength;

    return 0;
}

//��ȡĳ�����ֵ,ASC��ʽ
int CUP8583::CUP8583_GetField_ASC(CUP8583_MSG* pi_Msg, int i_iFieldNum,unsigned char* pio_ucFieldBuf, int* pio_iFieldBufLen)
{
    int iLength,iOffset;
    unsigned char ucTemp[1024];

    if (i_iFieldNum < 0) return -1;

    if (pi_Msg->Field[i_iFieldNum].StateFlag != 1) return -2;

    if (Cup8583[i_iFieldNum].Flag == F_LLVAR_LEN)
    {
        memset(ucTemp, 0, sizeof(ucTemp));
        HextoAsc(pi_Msg->Field[i_iFieldNum].Buf, 1, ucTemp);
        iLength = atoi((char*)ucTemp);
        iOffset = 1;
    }
    else if (Cup8583[i_iFieldNum].Flag == F_LLLVAR_LEN)
    {
        memset(ucTemp, 0, sizeof(ucTemp));
        HextoAsc(pi_Msg->Field[i_iFieldNum].Buf, 2, ucTemp);
        iLength = atoi((char*)ucTemp);
        iOffset = 2;
    }
    else if (Cup8583[i_iFieldNum].Flag == F_FIX_LEN)
    {
        //iLength = pi_Msg->Field[i_iFieldNum].Length;
        iLength = Cup8583[i_iFieldNum].Length;
        iOffset = 0;
    }

    if (Cup8583[i_iFieldNum].Type == T_NUM)
    {
        memset(ucTemp, 0, sizeof(ucTemp));
        HextoAsc(pi_Msg->Field[i_iFieldNum].Buf + iOffset,pi_Msg->Field[i_iFieldNum].Length - iOffset,ucTemp);

        if (Cup8583[i_iFieldNum].Flag == F_FIX_LEN)
            memcpy(pio_ucFieldBuf, ucTemp + strlen((char*)ucTemp) - iLength, iLength);
        else
            memcpy(pio_ucFieldBuf, ucTemp, iLength);
    }
    else if ((Cup8583[i_iFieldNum].Type == T_NS) || (Cup8583[i_iFieldNum].Type == T_Z))
    {
        memset(ucTemp, 0, sizeof(ucTemp));
        HextoAsc(pi_Msg->Field[i_iFieldNum].Buf + iOffset,pi_Msg->Field[i_iFieldNum].Length - iOffset,ucTemp);
        memcpy(pio_ucFieldBuf, ucTemp, iLength);
    }
    else
    {
        HextoAsc(pi_Msg->Field[i_iFieldNum].Buf + iOffset,iLength,pio_ucFieldBuf);
        iLength *= 2;
    }

    *pio_iFieldBufLen = iLength;

    return 0;
}

//ɾ��ĳ������
int CUP8583::CUP8583_DeleteFeild(CUP8583_MSG* pio_Msg,int i_iFieldNum)
{
    memset(&(pio_Msg->Field[i_iFieldNum]),0,sizeof(pio_Msg->Field[i_iFieldNum]));

    pio_Msg->Field[1].Buf[(i_iFieldNum - 1)/8] &= (unsigned char)(0xFF ^ (int)pow(2,(7 - (i_iFieldNum - 1)%8)));
    pio_Msg->Field[i_iFieldNum].StateFlag = 0;

    return 0;
}

//��ȡ����
int CUP8583::CUP8583_GetBuf(CUP8583_MSG* pi_Msg, unsigned char* pio_ucMsgBuf, int* pio_iMsgBufLen)
{
    int i;

    *pio_iMsgBufLen = 0;

    for (i = 2; i <= 64; i++)
    {
        if (pi_Msg->Field[i].StateFlag == 1)
        {
            memcpy(pio_ucMsgBuf+ 10 + *pio_iMsgBufLen, pi_Msg->Field[i].Buf, pi_Msg->Field[i].Length);
            *pio_iMsgBufLen += pi_Msg->Field[i].Length;
        }
    }

    memcpy(pio_ucMsgBuf, pi_Msg->Field[0].Buf, 2);
    memcpy(pio_ucMsgBuf + 2, pi_Msg->Field[1].Buf, 8);
    *pio_iMsgBufLen += 10;

    return 0;
}

//��ȡ���Ľṹ
int CUP8583::CUP8583_GetMsg(CUP8583_MSG* pio_Msg,const unsigned char* pi_ucMsgBuf,int i_iMsgBufLen)
{
    int i,iOffset,iTemp,iLength;
    unsigned char ucTemp[1024];

    if (i_iMsgBufLen < 9) return -1;

    iOffset = 10;
    pio_Msg->Field[0].Length = 2;
    pio_Msg->Field[1].Length = 8;
    pio_Msg->Field[0].StateFlag = 1;
    pio_Msg->Field[1].StateFlag = 1;
    memcpy(pio_Msg->Field[0].Buf,pi_ucMsgBuf,2);
    memcpy(pio_Msg->Field[1].Buf,pi_ucMsgBuf + 2,8);

    for (i = 1;i <= 64;i++)
    {
        iTemp = (int)pow(2,(7 - (i - 1)%8));
        if ((pio_Msg->Field[1].Buf[(i - 1)/8] & iTemp) == iTemp)
        {
            if (Cup8583[i].Flag == F_LLVAR_LEN)
            {
                if (iOffset > i_iMsgBufLen)
                    return -2;

                memset(ucTemp,0,sizeof(ucTemp));
                HextoAsc(pi_ucMsgBuf + iOffset,1,ucTemp);
                iLength = atoi((char*)ucTemp);
                if(Cup8583[i].Length<=iLength)
                    iLength=Cup8583[i].Length;

                pio_Msg->Field[i].Length = 1;
            }
            else if (Cup8583[i].Flag == F_LLLVAR_LEN)
            {
                if (iOffset + 1 > i_iMsgBufLen)
                    return -2;

                memset(ucTemp,0,sizeof(ucTemp));
                HextoAsc(pi_ucMsgBuf + iOffset,2,ucTemp);
                iLength = atoi((char*)ucTemp);
                if(Cup8583[i].Length<=iLength)
                    iLength=Cup8583[i].Length;

                pio_Msg->Field[i].Length = 2;
            }
            else if (Cup8583[i].Flag == F_FIX_LEN)
            {
                iLength = Cup8583[i].Length;

                pio_Msg->Field[i].Length = 0;
            }

            if ((iLength + 1)/2 + pio_Msg->Field[i].Length + iOffset > i_iMsgBufLen)
                return -2;

            if ((Cup8583[i].Type == T_NS) || (Cup8583[i].Type == T_NUM) || (Cup8583[i].Type == T_Z))
            {
                memcpy(pio_Msg->Field[i].Buf,pi_ucMsgBuf + iOffset,pio_Msg->Field[i].Length + (iLength + 1)/2);
                pio_Msg->Field[i].Length += (iLength + 1)/2;
            }
            else
            {
                memcpy(pio_Msg->Field[i].Buf,pi_ucMsgBuf + iOffset,pio_Msg->Field[i].Length + iLength);
                pio_Msg->Field[i].Length += iLength;
            }

            iOffset += pio_Msg->Field[i].Length;
            pio_Msg->Field[i].StateFlag = 1;
        }
    }

    return 0;
}



//���㱨��MAC
int CUP8583::CalMsgMac(unsigned char* pi_ucMak,unsigned char* pi_ucMsgData,int i_iMsgDataLen,unsigned char* pio_ucMac)
{
    int i,b;
    unsigned char Tmp1[20];
    unsigned char Tmp2[20];
    unsigned char Tmp3[20];
    unsigned char Tmp4[20];
    unsigned char Tmp5[20];
    unsigned char Tmp6[20];
    //EnDes ED;
    b=(i_iMsgDataLen+7)/8;
    memset(pi_ucMsgData+i_iMsgDataLen,0,b*8-i_iMsgDataLen);
    memcpy(Tmp1,pi_ucMsgData,8);
    //	memset(Tmp1,0,8);
    for(i=1;i<b;i++)
    {
        //EnDes::BitXor(Tmp1,pi_ucMsgData+i*8,8,Tmp1);
        BitXor(Tmp1,pi_ucMsgData+i*8,8,Tmp1);
    }

    HextoAsc(Tmp1,4,Tmp2);

    HextoAsc(Tmp1 + 4,4,Tmp3);

    DES(0,pi_ucMak,Tmp2,Tmp4);

    BitXor(Tmp4,Tmp3,8,Tmp5);

    DES(0,pi_ucMak,Tmp5,Tmp6);

    HextoAsc(Tmp6,4,pio_ucMac);

    return 0;
}

//AsctoHex
unsigned char* CUP8583::AsctoHex(const unsigned char *Data, int Len,unsigned char* Result)
{
    int iLen;
    short Cpt;
    unsigned char Tmp1, LastByte,Tmp2;

    LastByte = Len % 2;           /* The string has an odd number of character */
    iLen = Len / 2;
    for( Cpt = 0; Cpt < iLen; Cpt++ ) {
        Tmp1 = Data[2*Cpt  ];
        Tmp2 = Data[2*Cpt+1];
        if( Tmp1 > '@' )
            Tmp1 -= 7;
        if( Tmp2 > '@' )
            Tmp2 -= 7;

        Result[Cpt] = (unsigned char)( ( Tmp1 << 4 ) | ( Tmp2 & 0x0F ) );
    }
    if( LastByte ) {              /* Fill the last half byte  */
        Tmp1 = Data[2*Cpt  ];
        if( Tmp1 > '@' )
            Tmp1 -= 7;
        Result[iLen] = (unsigned char)( Tmp1 << 4 );
    }

    return Result;
}

//HextoAsc
unsigned char* CUP8583::HextoAsc(const unsigned char *Data, int len, unsigned char* Result)
{
    int h,a;
    unsigned char uc;

    Result[0] = 0x0;
    if ( len < 1 ) return((unsigned char*)"\x00");
    h = len-1;
    a = len+len-1;
    Result[a+1]='\0';
    while(h >= 0) {
        uc = Data[h]&0x0f;
        Result[a--] = uc + ((uc>9)?('A'-10):'0');
        uc = (Data[h--]&0xf0)>>4;
        Result[a--] = uc + ((uc>9)?('A'-10):'0');
    }

    return Result;
}
