#include "cup8583.h"

struct CUP8583_DEF Cup8583[65] =
{
    {   4, T_NUM, F_FIX_LEN},		/* 域 0  消息类型           */
    {  16, T_NUM, F_FIX_LEN},		/* 域 1  位图               */
    {  19,  T_NS, F_LLVAR_LEN},	/* 域 2  主账号             */
    {   6,  T_NS, F_FIX_LEN},		/* 域 3  交易处理码         */
    {  12, T_NUM, F_FIX_LEN},		/* 域 4  交易金额           */
    {  12, T_NUM, F_FIX_LEN},		/* 域 5  清算金额           */
    {  12, T_NUM, F_FIX_LEN},		/* 域 6  持卡人扣账金额     */
    {  10,  T_NS, F_FIX_LEN},		/* 域 7  交易传输时间       */
    {   0, T_NUM, F_FIX_LEN},		/* 域 8                     */
    {   8, T_NUM, F_FIX_LEN},		/* 域 9  清算汇率           */
    {   8, T_NUM, F_FIX_LEN},		/* 域 10 持卡人扣账汇率     */
    {   6, T_NUM, F_FIX_LEN},		/* 域 11 系统跟踪号         */
    {   6,  T_NS, F_FIX_LEN},		/* 域 12 受卡方所在地时间   */
    {   4,  T_NS, F_FIX_LEN},		/* 域 13 受卡方所在地日期   */
    {   4,  T_NS, F_FIX_LEN},		/* 域 14 卡有效期           */
    {   4,  T_NS, F_FIX_LEN},		/* 域 15 清算日期           */
    {   4,  T_NS, F_FIX_LEN},		/* 域 16 兑换日期           */
    {   0, T_NUM, F_FIX_LEN},		/* 域 17                    */
    {   4,  T_NS, F_FIX_LEN},		/* 域 18 商户类型           */
    {   3, T_NUM, F_FIX_LEN},		/* 域 19 受理机构国家代码   */
    {   0, T_NUM, F_FIX_LEN},		/* 域 20                    */
    {   0, T_NUM, F_FIX_LEN},		/* 域 21                    */
    {   3,  T_NS, F_FIX_LEN},		/* 域 22 服务点输入方式码   */
    {   3, T_NUM, F_FIX_LEN},		/* 域 23 卡序列号           */
    {   0, T_NUM, F_FIX_LEN},		/* 域 24                    */
    {   2,  T_NS, F_FIX_LEN},		/* 域 25 服务点条件码       */
    {   2, T_NUM, F_FIX_LEN},		/* 域 26 服务点 PIN 获取码  */
    {   0, T_NUM, F_LLVAR_LEN},		/* 域 27                    */
    {   9,  T_XN, F_FIX_LEN},		/* 域 28 交易费             */
    {   0, T_NUM, F_LLVAR_LEN},		/* 域 29                    */
    {   0, T_NUM, F_LLVAR_LEN},		/* 域 30                    */
    {   0, T_NUM, F_LLVAR_LEN},		/* 域 31                    */
    {  11, T_NUM, F_LLVAR_LEN},		/* 域 32 受理机构标识码     */
    {  11, T_NUM, F_LLVAR_LEN},		/* 域 33 发送机构标识码     */
    {   0, T_NUM, F_LLLVAR_LEN},	/* 域 34                    */
    {  37, T_Z, F_LLVAR_LEN},		/* 域 35 第二磁道数据       */
    { 104, T_Z, F_LLLVAR_LEN},		/* 域 36 第三磁道数据       */
    {  12, T_ANS, F_FIX_LEN},		/* 域 37 检索参考号         */
    {   6, T_ANS, F_FIX_LEN},		/* 域 38 授权标识应答码     */
    {   2,  T_AN, F_FIX_LEN},		/* 域 39 应答码             */
    {   0, T_NUM, F_LLVAR_LEN},		/* 域 40                    */
    {   8, T_ANS, F_FIX_LEN},		/* 域 41 受卡机终端标识码   */
    {  15, T_ANS, F_FIX_LEN},		/* 域 42 受卡方标识码       */
    {  40, T_ANS, F_FIX_LEN},		/* 域 43 受卡方名称地址     */
    {  25, T_ANS, F_LLVAR_LEN},		/* 域 44 附加响应数据       */
    {  79,   T_Z, F_LLVAR_LEN},		/* 域 45 第一磁道数据       */
    {   0, T_NUM, F_LLLVAR_LEN},	/* 域 46                    */
    {   0, T_NUM, F_LLLVAR_LEN},	/* 域 47                    */
    { 322, T_NS, F_LLLVAR_LEN},	/* 域 48 附加数据－私有     */
    {   3,  T_AN, F_FIX_LEN},		/* 域 49 交易货币代码       */
    {   3,  T_AN, F_FIX_LEN},		/* 域 50 清算货币代码       */
    {   3,  T_AN, F_FIX_LEN},		/* 域 51 持卡人账户货币代码 */
    {   8, T_BIT, F_FIX_LEN},		/* 域 52 个人标识码数据     */
    {  16, T_NUM, F_FIX_LEN},		/* 域 53 安全控制信息       */
    {  40,  T_AN, F_LLLVAR_LEN},	/* 域 54 实际余额           */
    { 255, T_BCD, F_LLLVAR_LEN},	/* 域 55 基于 PBOC 借贷记标准的IC卡数据域 */
    {   0, T_NUM, F_LLLVAR_LEN},	/* 域 56                    */
    { 100, T_ANS, F_LLLVAR_LEN},	/* 域 57 附加交易信息       */
    { 100, T_ANS, F_LLLVAR_LEN},	/* 域 58 基于PBOC 电子钱包/存折IC卡标准的交易数据 */
    { 600, T_ANS, F_LLLVAR_LEN},	/* 域 59 明细查询数据       */
    {  17,  T_NS, F_LLLVAR_LEN},	/* 域 60 自定义域           */
    {  29,  T_NS, F_LLLVAR_LEN},	/* 域 61 原始信息域         */
    { 512, T_ANS, F_LLLVAR_LEN},	/* 域 62 自定义域           */
    { 163, T_ANS, F_LLLVAR_LEN},	/* 域 63 自定义域           */
    {   8, T_BIT, F_FIX_LEN},		/* 域 64 报文鉴别码         */
};

CUP8583::CUP8583()
{

}

//设置某个域的值
int CUP8583::CUP8583_SetField(CUP8583_MSG* pio_Msg, int i_iFieldNum, const char* pi_chFieldData, int i_iFieldLen)
{
    int iLength;
    unsigned char ucTemp[256];
    unsigned char ucLength[5];

    CUP8583_DeleteFeild(pio_Msg, i_iFieldNum);

    //检查域正确性
    if ((i_iFieldNum < 0) || (i_iFieldNum > 128) || (i_iFieldNum == 1)) return -1;

    //检查长度正确性
    if ((i_iFieldLen < 0) || (i_iFieldLen > Cup8583[i_iFieldNum].Length)) return -2;

    memset(ucLength, 0, sizeof(ucLength));
    memset(pio_Msg->Field[i_iFieldNum].Buf, 0, sizeof(pio_Msg->Field[i_iFieldNum].Buf));

    //报文类型
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

        //左靠齐右补0
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

            //右靠齐左补0
            memset(ucTemp, '0', iLength);
            memcpy(ucTemp + iLength - i_iFieldLen, pi_chFieldData, i_iFieldLen);
        }
        else if ((Cup8583[i_iFieldNum].Flag == F_LLVAR_LEN) || (Cup8583[i_iFieldNum].Flag == F_LLLVAR_LEN))
        {
            if (i_iFieldLen % 2 != 0) iLength = i_iFieldLen + 1;
            else iLength = i_iFieldLen;

            //左靠齐右补0
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

//获取某个域的值
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

//获取某个域的值,ASC方式
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

//删除某个域外
int CUP8583::CUP8583_DeleteFeild(CUP8583_MSG* pio_Msg,int i_iFieldNum)
{
    memset(&(pio_Msg->Field[i_iFieldNum]),0,sizeof(pio_Msg->Field[i_iFieldNum]));

    pio_Msg->Field[1].Buf[(i_iFieldNum - 1)/8] &= (unsigned char)(0xFF ^ (int)pow(2,(7 - (i_iFieldNum - 1)%8)));
    pio_Msg->Field[i_iFieldNum].StateFlag = 0;

    return 0;
}

//获取报文
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

//获取报文结构
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



//计算报文MAC
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
