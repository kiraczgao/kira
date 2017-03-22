#include "cuptransproc.h"

CUPTransProc::CUPTransProc()
{

}

void CUPTransProc::setBatchId(QString batchIdStr)
{
    g_iBatchId = batchIdStr.toInt();
}

void CUPTransProc::setMerchId(QString merchIdStr)
{
    memset(g_chMerchId, 0, sizeof(g_chMerchId));
    memcpy(g_chMerchId, merchIdStr.toLatin1().data(), merchIdStr.length());
}

void CUPTransProc::setTermId(QString termIdStr)
{
    memset(g_chTermId, 0, sizeof(g_chTermId));
    memcpy(g_chTermId, termIdStr.toLatin1().data(), termIdStr.length());
}

void CUPTransProc::setTermMk(QString termMkStr)
{
    unsigned char term_mk[32];
    memset(term_mk, 0, sizeof(term_mk));
    memcpy(term_mk, termMkStr.toLatin1().data(), termMkStr.length());
    for(int i=0; i<16; i++)
    {
        unsigned char mkH = AscToHex(term_mk[2*i]);
        unsigned char mkL = AscToHex(term_mk[2*i+1]);

        TERM_MK[i] = ((mkH<<4)&0xf0) + (mkL&0x0f);
    }

}

unsigned char CUPTransProc::AscToHex(unsigned char aChar)
{
    if((aChar>=0x30)&&(aChar<=0x39))
        aChar -= 0x30;
    else if((aChar>=0x41)&&(aChar<=0x46))//大写字母
        aChar -= 0x37;
    else if((aChar>=0x61)&&(aChar<=0x66))//小写字母
        aChar -= 0x57;
    else
        aChar = 0xff;
    return aChar;
}

unsigned char CUPTransProc::HexToAsc(unsigned char aHex)
{
    unsigned char  ASCII_Data;
    ASCII_Data = aHex & 0x0f;
    if(ASCII_Data < 10)
        ASCII_Data = ASCII_Data + 0x30;         //‘0--9’
    else
        ASCII_Data = ASCII_Data + 0x37;         //‘A--F’
    return ASCII_Data;
}

//设置新批次
int CUPTransProc::SetBatch(char* pi_chBatchId)
{
    QString iBatchidStr = QByteArray::fromRawData(pi_chBatchId, 10);
    if(iBatchidStr.toInt() != g_iBatchId)
    {
        printf("需更新批次号\n");
        g_iBatchId = iBatchidStr.toInt();

        writeSetInfo(unionPayConfigFile, unionPayGroup, "batchId", iBatchidStr);
        //更新流水
        writeSetInfo(unionPayConfigFile, unionPayGroup, "Seq", "0");
    }
    else
        printf("无需更新批次号\n");

    return SLSY_TRANS_SUCCESS;
}

//打印8583报文,通过结构
int CUPTransProc::Print8583Msg(CUP8583_MSG i_cup8583Msg)
{
    int i, iLen;
    unsigned char ucFiled[1024];

    for (i = 0; i <= 64; i++)
    {
        if (i_cup8583Msg.Field[i].StateFlag == 1)
        {
            memset(ucFiled, 0, sizeof(ucFiled));
            CUP8583_GetField_ASC(&i_cup8583Msg, i, ucFiled, &iLen);
            // DebugLog::Log
            printf("第%02d域 : %s\n", i, (char*)ucFiled);
        }
    }

    return SLSY_TRANS_SUCCESS;
}

//打印8583报文,通过报文
int CUPTransProc::Print8583Msg2(unsigned char* pi_ucMsg, int i_iMsgLen)
{
    int i, iLen;
    unsigned char ucFiled[1024];
    CUP8583_MSG cup8583Msg;

    memset(&cup8583Msg, 0, sizeof(cup8583Msg));

    if (CUP8583_GetMsg(&cup8583Msg, pi_ucMsg + 13, i_iMsgLen) != 0)
    {
        //DebugLog::Log("解析失败\r\n");
        return SLSY_TRANS_ERROR;
    }

    for (i = 0; i <= 64; i++)
    {
        if (cup8583Msg.Field[i].StateFlag == 1)
        {
            memset(ucFiled, 0, sizeof(ucFiled));
            CUP8583_GetField_ASC(&cup8583Msg, i, ucFiled, &iLen);

            printf("第%02d域 : %s\n", i, (char*)ucFiled);
        }
    }

    return SLSY_TRANS_SUCCESS;
}

//重置流水
int CUPTransProc::ResetSeq()
{
    //更新流水
    writeSetInfo(unionPayConfigFile, unionPayGroup, "Seq", "0");

    return SLSY_TRANS_SUCCESS;
}

//生成新流水
char* CUPTransProc::GenSeq(char* pio_chSeq)
{
    int iSeq;
    //读取当前流水
    QString seqstr;
    readSetInfo(unionPayConfigFile, unionPayGroup, "Seq", seqstr);

    //流水增加
    //iSeq = strtol(chSeq, NULL, 10) + 1;
    iSeq = seqstr.toInt() + 1;
    QString newseqstr = QString::number(iSeq, 10);
    printf("11域流水号 = %d \n", iSeq);
    sprintf(pio_chSeq, "%06d", iSeq);
    //qDebug(qPrintable(newseqstr));

    //更新流水
    writeSetInfo(unionPayConfigFile, unionPayGroup, "Seq", newseqstr);

    return pio_chSeq;
}

//读取旧流水
char* CUPTransProc::NowGenSeq(char* pio_chSeq)
{
    int iSeq;
    //读取当前流水
    QString seqstr;
    readSetInfo(unionPayConfigFile, unionPayGroup, "Seq", seqstr);

    //流水增加
    iSeq = seqstr.toInt();
    printf("11域流水号 = %d\n", iSeq);
    sprintf(pio_chSeq, "%06d", iSeq);

    return pio_chSeq;
}

//生成新批次
int CUPTransProc::GenNewBatch()
{
    //更新批次
    QString batchIdstr = QString::number(g_iBatchId + 1, 10);
    writeSetInfo(unionPayConfigFile, unionPayGroup, "BatchId", batchIdstr);

    //更新流水
    writeSetInfo(unionPayConfigFile, unionPayGroup, "Seq", "0");

    return SLSY_TRANS_SUCCESS;
}

//获取脱机消费报文ANS
int CUPTransProc::GetOfflineConsumeMsg(char* chFiled37, char chFiled37Len, char* pi_chPansn, int i_iTransAmt, unsigned char* pi_ucIccData, int i_iIccDataLen, unsigned char* pio_ucMsg, int* pio_iMsgLen,char *T_5f24)
{
    char chLen[5];
    char chSeq[7];
    char chFiled4[13];
    char chFiled23[5];
    char chFiled53[17];
    char chFiled60[18];
    char i;

    unsigned char ucMac1[16];
    //unsigned char ucMac[16];
    //unsigned char ucMACASC[32];
    CUP8583_MSG Msg8583;

    memset(chSeq, 0, sizeof(chSeq));
    memset(chFiled4, 0, sizeof(chFiled4));
    memset(chFiled23, 0, sizeof(chFiled23));
    memset(chFiled60, 0, sizeof(chFiled60));

    GenSeq(chSeq);
    sprintf(chFiled53, "%016d", 0);
    sprintf(chFiled4, "%012d", i_iTransAmt);
    sprintf(chFiled23, "%.3s", pi_chPansn);
    //logprintf("脱机消费报文 : ", pio_ucMsg, *pio_iMsgLen);
    sprintf(chFiled60, "22%06d00060", g_iBatchId);

    //准备消费数据
    memset(&Msg8583, 0, sizeof(Msg8583));
    ASSERT_FAIL(CUP8583_SetField(&Msg8583, 0, "0200", 4));
    ASSERT_FAIL(CUP8583_SetField(&Msg8583, 3, "000000", 6));
    ASSERT_FAIL(CUP8583_SetField(&Msg8583, 4, chFiled4, 12));

    ASSERT_FAIL(CUP8583_SetField(&Msg8583, 11, chSeq, 6));

    ASSERT_FAIL(CUP8583_SetField(&Msg8583, 14, T_5f24, 4));

    ASSERT_FAIL(CUP8583_SetField(&Msg8583, 22, "072", 3));

    ASSERT_FAIL(CUP8583_SetField(&Msg8583, 23, chFiled23, 3));

    ASSERT_FAIL(CUP8583_SetField(&Msg8583, 25, "00", 2));

    ASSERT_FAIL(CUP8583_SetField(&Msg8583, 35, chFiled37, chFiled37Len));
    for(i=0; i<20; i++)
    {
        if(chFiled37[i] == 'D')
             chFiled37[i] = 0;
    }

    ASSERT_FAIL(CUP8583_SetField(&Msg8583, 2,chFiled37, strlen(chFiled37)));

    ASSERT_FAIL(CUP8583_SetField(&Msg8583, 41, g_chTermId, 8));

    ASSERT_FAIL(CUP8583_SetField(&Msg8583, 42, g_chMerchId, 15));

    ASSERT_FAIL(CUP8583_SetField(&Msg8583, 49, "156", 3));


    ASSERT_FAIL(CUP8583_SetField(&Msg8583, 53,chFiled53, 16));

    ASSERT_FAIL(CUP8583_SetField(&Msg8583, 55, (char*)pi_ucIccData, i_iIccDataLen));
    ASSERT_FAIL(CUP8583_SetField(&Msg8583, 60, chFiled60, strlen(chFiled60)));
    //  ASSERT_FAIL(CUP8583_SetField(&Msg8583, 63, "CUP", 3));
    ASSERT_FAIL(CUP8583_SetField(&Msg8583, 64, "\x30\x30\x30\x30\x30\x30\x30\x00", 8));
    ASSERT_FAIL(CUP8583_GetBuf(&Msg8583, pio_ucMsg + 13, pio_iMsgLen));

    CalMsgMac(g_ucMak,pio_ucMsg + 13,*pio_iMsgLen - 8,ucMac1);
    ASSERT_FAIL(CUP8583_SetField(&Msg8583, 64, (char*)ucMac1, 8));
    ASSERT_FAIL(CUP8583_GetBuf(&Msg8583, pio_ucMsg + 13, pio_iMsgLen));

    memcpy(pio_ucMsg + 2, "\x60\x00\x30\x00\x00\x60\x32\x00\x00\x00\x00", 11);

    memset(chLen, 0, sizeof(chLen));
    sprintf(chLen, "%04X", *pio_iMsgLen + 11);
    AsctoHex((unsigned char*)chLen, 4, pio_ucMsg);

    *pio_iMsgLen += 13;

    //打印8583报文
    Print8583Msg(Msg8583);

    return SLSY_TRANS_SUCCESS;
}

//获取冲正报文
int CUPTransProc::GetConsumeMsgReversing(char* chFiled37, char chFiled37Len, char* pi_chPansn, int i_iTransAmt, unsigned char* pi_ucIccData, int i_iIccDataLen, unsigned char* pio_ucMsg, int* pio_iMsgLen,char *T_5f24)
{
    char chLen[5];
    char chSeq[7];
    char chFiled4[13];
    char chFiled23[5];
    char chFiled53[17];
    char chFiled60[18];
    char i;

    //////
    unsigned char ucMac1[16];
    //////
    //unsigned char ucMac[16];
    //unsigned char ucMACASC[32];
    CUP8583_MSG Msg8583;
    //
    memset(chSeq, 0, sizeof(chSeq));
    memset(chFiled4, 0, sizeof(chFiled4));
    memset(chFiled23, 0, sizeof(chFiled23));
    memset(chFiled60, 0, sizeof(chFiled60));

    NowGenSeq(chSeq);
    sprintf(chFiled53, "%016d", 0);
    sprintf(chFiled4, "%012d", i_iTransAmt);
    sprintf(chFiled23, "%.3s", pi_chPansn);
    //logprintf("脱机消费报文 : ", pio_ucMsg, *pio_iMsgLen);
    sprintf(chFiled60, "22%06d00060", g_iBatchId);

    //准备消费数据
    memset(&Msg8583, 0, sizeof(Msg8583));
    ASSERT_FAIL(CUP8583_SetField(&Msg8583, 0, "0400", 4));
    ASSERT_FAIL(CUP8583_SetField(&Msg8583, 3, "000000", 6));
    ASSERT_FAIL(CUP8583_SetField(&Msg8583, 4, chFiled4, 12));

    ASSERT_FAIL(CUP8583_SetField(&Msg8583, 11, chSeq, 6));

    ASSERT_FAIL(CUP8583_SetField(&Msg8583, 14, T_5f24, 4));

    ASSERT_FAIL(CUP8583_SetField(&Msg8583, 22, "072", 3));

    ASSERT_FAIL(CUP8583_SetField(&Msg8583, 23, chFiled23, 3));

    ASSERT_FAIL(CUP8583_SetField(&Msg8583, 25, "00", 2));

    //  ASSERT_FAIL(CUP8583_SetField(&Msg8583, 35, chFiled37, chFiled37Len));
    for(i=0; i<20; i++)
    {
        if(chFiled37[i] == 'D')
             chFiled37[i] = 0;
    }

    ASSERT_FAIL(CUP8583_SetField(&Msg8583, 2,chFiled37, strlen(chFiled37)));
    ASSERT_FAIL(CUP8583_SetField(&Msg8583, 39, "98", 2));
    ASSERT_FAIL(CUP8583_SetField(&Msg8583, 41, g_chTermId, 8));

    ASSERT_FAIL(CUP8583_SetField(&Msg8583, 42, g_chMerchId, 15));

    ASSERT_FAIL(CUP8583_SetField(&Msg8583, 49, "156", 3));


    ASSERT_FAIL(CUP8583_SetField(&Msg8583, 53,chFiled53, 16));

    // ASSERT_FAIL(CUP8583_SetField(&Msg8583, 55, (char*)pi_ucIccData, i_iIccDataLen));
    ASSERT_FAIL(CUP8583_SetField(&Msg8583, 60, chFiled60, strlen(chFiled60)));
    // ASSERT_FAIL(CUP8583_SetField(&Msg8583, 63, "CUP", 3));
    ASSERT_FAIL(CUP8583_SetField(&Msg8583, 64, "\x30\x30\x30\x30\x30\x30\x30\x00", 8));
    ASSERT_FAIL(CUP8583_GetBuf(&Msg8583, pio_ucMsg + 13, pio_iMsgLen));

    CalMsgMac(g_ucMak,pio_ucMsg + 13,*pio_iMsgLen - 8,ucMac1);  //????±¨??MAC
    ASSERT_FAIL(CUP8583_SetField(&Msg8583, 64, (char*)ucMac1, 8));
    ASSERT_FAIL(CUP8583_GetBuf(&Msg8583, pio_ucMsg + 13, pio_iMsgLen));

    memcpy(pio_ucMsg + 2, "\x60\x00\x30\x00\x00\x60\x32\x00\x00\x00\x00", 11);

    memset(chLen, 0, sizeof(chLen));
    sprintf(chLen, "%04X", *pio_iMsgLen + 11);
    AsctoHex((unsigned char*)chLen, 4, pio_ucMsg);

    *pio_iMsgLen += 13;

    //打印8583报文
    Print8583Msg(Msg8583);

    //DebugLog::
    //logprintf("[脱机消费报文]:", pio_ucMsg, *pio_iMsgLen);

    return SLSY_TRANS_SUCCESS;
}

//获取批结算报文
int CUPTransProc::GetbanlanceMsg(unsigned char* pio_ucMsg, int* pio_iMsgLen)
{
    char chLen[5];
    char chSeq[7];
    char chFiled60[18];
    char chFiled48[63];
    CUP8583_MSG CMLogIn;

    memset(chFiled60, 0, sizeof(chFiled60));
    memset(chFiled48, 0, sizeof(chFiled48));
    memset(chSeq, 0, sizeof(chSeq));
    GenSeq(chSeq);
    sprintf(chFiled60, "00%06d201", g_iBatchId);

    sprintf(chFiled48, "%012d%03d%012d%03d0", 1, 1, 0, 0);

    memset(&CMLogIn, 0, sizeof(CMLogIn));
    ASSERT_FAIL(CUP8583_SetField(&CMLogIn, 0, "0500", 4));
    ASSERT_FAIL(CUP8583_SetField(&CMLogIn, 11, chSeq, 6));
    ASSERT_FAIL(CUP8583_SetField(&CMLogIn, 41, g_chTermId, 8));
    ASSERT_FAIL(CUP8583_SetField(&CMLogIn, 42, g_chMerchId, 15));

    ASSERT_FAIL(CUP8583_SetField(&CMLogIn, 48, chFiled48, 62));

    ASSERT_FAIL(CUP8583_SetField(&CMLogIn, 49, "156", 3));
    ASSERT_FAIL(CUP8583_SetField(&CMLogIn, 60, chFiled60, 11));
    ASSERT_FAIL(CUP8583_SetField(&CMLogIn, 63, "001", 3));
    ASSERT_FAIL(CUP8583_GetBuf(&CMLogIn, pio_ucMsg + 13, pio_iMsgLen));
    memcpy(pio_ucMsg + 2, "\x60\x00\x30\x00\x00\x60\x21\x00\x00\x00\x00", 11);

    memset(chLen, 0, sizeof(chLen));
    sprintf(chLen, "%04X", *pio_iMsgLen + 11);
    AsctoHex((unsigned char*)chLen, 4, pio_ucMsg);

    *pio_iMsgLen += 13;

    //打印8583报文
    Print8583Msg(CMLogIn);

    return SLSY_TRANS_SUCCESS;
}

//获取签到报文
int CUPTransProc::GetLoginMsg(unsigned char* pio_ucMsg, int* pio_iMsgLen)
{
    char chLen[5];
    char chSeq[7];
    char chFiled60[18];
    CUP8583_MSG CMLogIn;

    // 写死
    // strcpy(g_chTermId,"09676900");
    // strcpy(g_chMerchId,"898330189991055");

    memset(chFiled60, 0, sizeof(chFiled60));
    memset(chSeq, 0, sizeof(chSeq));
    GenSeq(chSeq);
    sprintf(chFiled60, "00%06d003", g_iBatchId);

    memset(&CMLogIn, 0, sizeof(CMLogIn));
    ASSERT_FAIL(CUP8583_SetField(&CMLogIn, 0, "0800", 4));
    ASSERT_FAIL(CUP8583_SetField(&CMLogIn, 11, chSeq, 6));
    ASSERT_FAIL(CUP8583_SetField(&CMLogIn, 41, g_chTermId, 8));
    ASSERT_FAIL(CUP8583_SetField(&CMLogIn, 42, g_chMerchId, 15));
    ASSERT_FAIL(CUP8583_SetField(&CMLogIn, 60, chFiled60, 11));
    ASSERT_FAIL(CUP8583_SetField(&CMLogIn, 63, "001", 3));
    ASSERT_FAIL(CUP8583_GetBuf(&CMLogIn, pio_ucMsg + 13, pio_iMsgLen));
    memcpy(pio_ucMsg + 2, "\x60\x00\x30\x00\x00\x60\x32\x00\x00\x00\x00", 11);
    memset(chLen, 0, sizeof(chLen));
    sprintf(chLen, "%04X", *pio_iMsgLen + 11);
    AsctoHex((unsigned char*)chLen, 4, pio_ucMsg);

    *pio_iMsgLen += 13;

    //打印8583报文
    Print8583Msg(CMLogIn);

    return SLSY_TRANS_SUCCESS;
}

//获取签退报文
int CUPTransProc::GetLogOutMsg(unsigned char* pio_ucMsg, int* pio_iMsgLen)
{
    char chLen[5];
    char chSeq[7];
    char chFiled60[18];
    CUP8583_MSG Msg8583;

    memset(chFiled60, 0, sizeof(chFiled60));
    memset(chSeq, 0, sizeof(chSeq));
    GenSeq(chSeq);
    sprintf(chFiled60, "00%06d002", g_iBatchId);

    memset(&Msg8583, 0, sizeof(Msg8583));
    ASSERT_FAIL(CUP8583_SetField(&Msg8583, 0, "0820", 4));
    ASSERT_FAIL(CUP8583_SetField(&Msg8583, 11, chSeq, 6));
    ASSERT_FAIL(CUP8583_SetField(&Msg8583, 41, g_chTermId, 8));
    ASSERT_FAIL(CUP8583_SetField(&Msg8583, 42, g_chMerchId, 15));
    ASSERT_FAIL(CUP8583_SetField(&Msg8583, 60, chFiled60, 11));
    ASSERT_FAIL(CUP8583_GetBuf(&Msg8583, pio_ucMsg + 13, pio_iMsgLen));
    memcpy(pio_ucMsg + 2, "\x60\x00\x75\x00\x00\x60\x31\x00\x00\x00\x00", 11);

    memset(chLen, 0, sizeof(chLen));
    sprintf(chLen, "%04X", *pio_iMsgLen + 11);
    AsctoHex((unsigned char*)chLen, 4, pio_ucMsg);

    *pio_iMsgLen += 13;

    //打印8583报文
    Print8583Msg(Msg8583);

    return SLSY_TRANS_SUCCESS;
}

//获取消费报文
int CUPTransProc::GetConsumeMsg(char* pi_chPan, int i_iTransAmt, unsigned char* pio_ucMsg, int* pio_iMsgLen)
{
    char chLen[5];
    char chSeq[7];
    char chFiled4[13];
    char chFiled60[18];
    unsigned char ucMac[8];
    CUP8583_MSG Msg8583;

    memset(chSeq, 0, sizeof(chSeq));
    memset(chFiled4, 0, sizeof(chFiled4));
    memset(chFiled60, 0, sizeof(chFiled60));
    GenSeq(chSeq);
    sprintf(chFiled4, "%012d", i_iTransAmt);
    sprintf(chFiled60, "22%06d", g_iBatchId);

    //准备消费数据
    memset(&Msg8583, 0, sizeof(Msg8583));
    ASSERT_FAIL(CUP8583_SetField(&Msg8583, 0, "0200", 4));
    ASSERT_FAIL(CUP8583_SetField(&Msg8583, 2, pi_chPan, strlen(pi_chPan)));
    ASSERT_FAIL(CUP8583_SetField(&Msg8583, 3, "000000", 6));
    ASSERT_FAIL(CUP8583_SetField(&Msg8583, 4, chFiled4, 12));
    ASSERT_FAIL(CUP8583_SetField(&Msg8583, 11, chSeq, 6));
    ASSERT_FAIL(CUP8583_SetField(&Msg8583, 22, "022", 3));
    ASSERT_FAIL(CUP8583_SetField(&Msg8583, 25, "00", 2));
    ASSERT_FAIL(CUP8583_SetField(&Msg8583, 41, g_chTermId, 8));
    ASSERT_FAIL(CUP8583_SetField(&Msg8583, 42, g_chMerchId, 15));
    ASSERT_FAIL(CUP8583_SetField(&Msg8583, 49, "156", 3));
    ASSERT_FAIL(CUP8583_SetField(&Msg8583, 60, chFiled60, 11));
    ASSERT_FAIL(CUP8583_SetField(&Msg8583, 64, "\x30\x30\x30\x30\x30\x30\x30\x00", 8));
    ASSERT_FAIL(CUP8583_GetBuf(&Msg8583, pio_ucMsg + 13, pio_iMsgLen));

    CalMsgMac(g_ucMak, pio_ucMsg + 13, *pio_iMsgLen - 8, ucMac);

    ASSERT_FAIL(CUP8583_SetField(&Msg8583, 64, (char*)ucMac, 8));
    ASSERT_FAIL(CUP8583_GetBuf(&Msg8583, pio_ucMsg + 13, pio_iMsgLen));

    memcpy(pio_ucMsg + 2, "\x60\x00\x30\x00\x00\x60\x21\x00\x00\x00\x00", 11);

    memset(chLen, 0, sizeof(chLen));
    sprintf(chLen, "%04X", *pio_iMsgLen + 11);
    AsctoHex((unsigned char*)chLen, 4, pio_ucMsg);

    *pio_iMsgLen += 13;

    //打印8583报文
    Print8583Msg(Msg8583);

    return SLSY_TRANS_SUCCESS;
}
