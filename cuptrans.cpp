#include "cuptrans.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include <QString>
#include <QByteArray>

CUPTrans::CUPTrans()
{

}

void CUPTrans::BCDToMMDD(unsigned char *bcd, char *ret)
{
    unsigned char Value = (bcd[0] & 0xF0) >> 4;
    int nMonth = Value * 10;
    Value = bcd[0] & 0x0F;
    nMonth += Value;
    Value = (bcd[1] & 0xF0) >> 4;
    int nDay = Value * 10;
    Value = bcd[1] & 0x0F;
    nDay += Value;
    sprintf(ret,"%02d%02d",nMonth,nDay);
}

void CUPTrans::BCDTohhmmss(unsigned char *bcd, char *ret)
{
    unsigned char value = (bcd[0] & 0xF0) >> 4;
    int h = value * 10;
    value = bcd[0] & 0x0F;
    h += value;
    value = (bcd[1] & 0xF0) >> 4;
    int m = value * 10;
    value = bcd[1] & 0x0F;
    m += value;
    value = (bcd[2] & 0xF0) >> 4;
    int s = value * 10;
    value = bcd[2] & 0x0F;
    s += value;
    sprintf(ret,"%02d%02d%02d",h,m,s);
}

/*********************************************************************************
功	能	:	签到
参	数	:	空
返	回	:	见定义
**********************************************************************************/
int CUPTrans::Login()
{
    int iRet, iLen, iLoginSendMsgLen, iLoginRecvMsgLen;
    //char chMak[17];
    unsigned char chMak[35];
    char chNNewBatch[10];
    unsigned char ucLoginSendMsg[1024];
    unsigned char ucLoginRecvMsg[1024];
    unsigned char ucFiled60[20];
    unsigned char ucWorkKey[60];
    //unsigned char ucMk[16];
    unsigned char ucMakCk[8];
    unsigned char ucRspCode[3];
    CUP8583_MSG cup8583Msg;
    //int j;
    ////////
    unsigned char ucMkKeyA[16];
    unsigned char ucMkKeyB[16];
    unsigned char g_ucMaktemp1[16];
    unsigned char g_ucMaktemp2[16];
    //unsigned char chSeq[7];
    //EnDes ED;
    //获取签到报文
    printf("into login\n");
    if (GetLoginMsg(ucLoginSendMsg, &iLoginSendMsgLen) != SLSY_TRANS_SUCCESS)
        return SLSY_TRANS_ERROR;

    printf("send login information... \n");
    //发送报文
    if (iRet = SendRecvMsg(ucLoginSendMsg, iLoginSendMsgLen, ucLoginRecvMsg, &iLoginRecvMsgLen), iRet == 0)
    {
        //成功
        //logprintf("[收到响应]:", ucLoginRecvMsg, iLoginRecvMsgLen);
        printf("收到响应 \n");
        Print8583Msg2(ucLoginRecvMsg, iLoginRecvMsgLen);
    }
    else if (iRet < -3)
    {
        close(socket_online);
        socket_online = -1;
        //无收到响应
        //logprintf("[无收到响应]\n", NULL, 0);
        printf("无响应 \n");
        return SLSY_TRANS_ERROR;
    }
    else
    {
        //未发送出报文
        close(socket_online);
        socket_online = -1;
        //logprintf("[未发送成功]\n", NULL,0);
        printf("未发送成功 \n");
        return SLSY_TRANS_ERROR;
    }

    //解析报文
    memset(&cup8583Msg, 0, sizeof(cup8583Msg));
    if (CUP8583_GetMsg(&cup8583Msg, ucLoginRecvMsg + 13, iLoginRecvMsgLen) != 0)
    {
        printf("解析失败\n");
        return SLSY_TRANS_ERROR;
    }

    //获取39域应答码
    memset(ucRspCode, 0, sizeof(ucRspCode));
    ASSERT_FAIL(CUP8583_GetField(&cup8583Msg, 39, ucRspCode, &iLen));
    if (memcmp(ucRspCode, "\x30\x30", 2) != 0)
    {
        printf("签到失败,应答码:%s\r\n", (char*)ucRspCode);
        return SLSY_TRANS_ERROR;
    }
    //获取获取签到交易流水号
    //   ASSERT_FAIL(CUP8583_GetField(&cup8583Msg, 11, chSeq, &iLen));
    //   WritePrivateProfileString("TermParam", "Seq", chSeq, LOCALCONFIG);
    //获取60域数据
    ASSERT_FAIL(CUP8583_GetField(&cup8583Msg, 60, ucFiled60, &iLen));
    printf("60域数据 : %s\r\n", (char*)ucFiled60);
    memset(chNNewBatch, 0, sizeof(chNNewBatch));
    memcpy(chNNewBatch, ucFiled60 + 2, 6);
    SetBatch(chNNewBatch);

    //获取工作密钥
    ASSERT_FAIL(CUP8583_GetField(&cup8583Msg, 62, ucWorkKey, &iLen));
    //logprintf("[工作密钥]: ", ucWorkKey, iLen);
    memcpy(g_keyMAK, ucWorkKey + 20, 16);
    //logprintf("[工作密钥MAK密文]: ", g_keyMAK, 16);
    //logprintf("[工作密钥MAK校验]: ", &ucWorkKey[36], 4);
    //解密工作密钥
    memcpy(ucMkKeyA, TERM_MK, 8);
    memcpy(ucMkKeyB, TERM_MK+8, 8);
    //////////////////////////////////////////////wait
    DES(1, ucMkKeyA, ucWorkKey + 20, g_ucMaktemp1);
    DES(0, ucMkKeyB, g_ucMaktemp1,  g_ucMaktemp2);
    DES(1, ucMkKeyA, g_ucMaktemp2, g_ucMak);

    DES(0, g_ucMak, (unsigned char*)"\x00\x00\x00\x00\x00\x00\x00\x00", ucMakCk);
    //logprintf("[本地计算的MAK校验]:", ucMakCk, 4);
    if (memcmp(ucMakCk, ucWorkKey + 36, 4) != 0)
    {
        printf("===MAK校验错误\n====\r\n");
        return SLSY_TRANS_ERROR;
    }

    //更新MAK
    memset(chMak, 0, sizeof(chMak));
    HextoAsc(g_keyMAK, 16, &chMak[0]);
    printf("strlenofmak = %d\n", strlen((char*)chMak));

    QString makstr = QByteArray::fromRawData((char*)chMak, strlen((char*)chMak));
    writeSetInfo(unionPayConfigFile, unionPayGroup, "MAK", makstr);

    printf("签到成功\r\n");
    // 发送语音 代码 99
    return SLSY_TRANS_SUCCESS;
}

/*********************************************************************************
功	能	:	消费
参	数	:	pi_chPan - 卡号
参	数	:	i_iTransAmt - 交易金额，以分为单位
返	回	:	见定义
**********************************************************************************/
int CUPTrans::Consume(char* pi_chPan, int i_iTransAmt)
{
    int iRet, iLen, iSendMsgLen, iRecvMsgLen;
    unsigned char ucRspCode[3];
    unsigned char ucSendMsg[1024];
    unsigned char ucRecvMsg[1024];
    unsigned char ucRspMac[8];
    unsigned char ucMac[8];
    CUP8583_MSG cup8583Msg;

    //获取消费报文
    if (GetConsumeMsg(pi_chPan, i_iTransAmt, ucSendMsg, &iSendMsgLen) != SLSY_TRANS_SUCCESS)
        return SLSY_TRANS_ERROR;

    //发送报文
    if (iRet = SendRecvMsg(ucSendMsg, iSendMsgLen, ucRecvMsg, &iRecvMsgLen), iRet == 0)
    {
        //成功
        //DebugLog::Log("收到响应 : ", ucRecvMsg, iRecvMsgLen);
        printf("收到回应\n");
        Print8583Msg2(ucRecvMsg, iRecvMsgLen);
    }
    else if (iRet < -3)
    {
        //无收到响应
        // DebugLog::Log
        close(socket_online);
        socket_online = -1;
        printf("无收到响应\r\n");
        return SLSY_TRANS_ERROR;
    }
    else
    {
        //未发送出报文
        // DebugLog::Log
        close(socket_online);
        socket_online = -1;
        printf("未发送出报文\r\n");
        return SLSY_TRANS_ERROR;
    }

    //解析报文
    memset(&cup8583Msg, 0, sizeof(cup8583Msg));
    if (CUP8583_GetMsg(&cup8583Msg, ucRecvMsg + 13, iRecvMsgLen - 13) != 0)
    {
        //DebugLog::Log("解析失败\r\n");
        printf("解析失败\n");
        return SLSY_TRANS_ERROR;
    }

    //获取39域应答码
    memset(ucRspCode, 0, sizeof(ucRspCode));
    ASSERT_FAIL(CUP8583_GetField(&cup8583Msg, 39, ucRspCode, &iLen));
    if (memcmp(ucRspCode, "\x30\x30", 2) != 0)
    {
        //DebugLog::Log("消费失败,应答码:%s\r\n", (char*)ucRspCode);
        printf("消费应答错误\n");
        return SLSY_TRANS_ERROR;
    }

    //获取64域MAC
    memset(ucRspMac, 0, sizeof(ucRspMac));
    ASSERT_FAIL(CUP8583_GetField(&cup8583Msg, 64, ucRspMac, &iLen));

    //检查MAC
    CalMsgMac(g_ucMak, ucRecvMsg + 13, iRecvMsgLen - 13 - 8, ucMac);
    // DebugLog::Log("MAK : ", g_ucMak, 8);
    // DebugLog::Log("MAC (POS 中心) : ", ucRspMac, 8);
    // DebugLog::Log("MAC (本地计算) : ", ucMac, 8);
    if (memcmp(ucMac, ucRspMac, 8) != 0)
    {
        // DebugLog::Log("MAC 校验值不正确\r\n");
        printf("MAC 校验不对\n");
        return SLSY_TRANS_ERROR;
    }

    //DebugLog::Log("消费成功\r\n");
    printf("消费成功\n");

    return SLSY_TRANS_SUCCESS;
}

/*********************************************************************************
功	能	:	脱机消费
参	数	:	pi_chPan - 域5A
参	数	:	i_iTransAmt - 交易金额，以分为单位
返	回	:	见定义
**********************************************************************************/
int CUPTrans::OfflineConsume(unionPayInfo_t* pTag,char* pi_chPan,char pi_chPanLen, char* pi_chPanSn, int i_iTransAmt, unsigned char* pi_ucIccData, int i_iIccDataLen,char *T_5f24)
{
    int iRet, iLen, iSendMsgLen, iRecvMsgLen;
    unsigned char ucRspCode[3];
    unsigned char ucSendMsg[1024];
    unsigned char ucRecvMsg[1024];
    unsigned char ucRspMac[8];
    unsigned char ucMac[8];
    ifield39 = 0;
    CUP8583_MSG cup8583Msg;
    //获取消费报文
    if(GetOfflineConsumeMsg(pTag,pi_chPan,pi_chPanLen, pi_chPanSn, i_iTransAmt, pi_ucIccData, i_iIccDataLen, ucSendMsg, &iSendMsgLen,T_5f24) != SLSY_TRANS_SUCCESS)
        return SLSY_TRANS_ERROR;

    //发送报文
    if (iRet = SendRecvMsg(ucSendMsg, iSendMsgLen, ucRecvMsg, &iRecvMsgLen), iRet == 0)
    {
        //成功
        Print8583Msg2(ucRecvMsg, iRecvMsgLen);
    }
    else if (iRet < -3)
    {
        //无收到响应
        printf("无响应,认为成功\n");
        close(socket_online);
        socket_online=-1;
        //return SLSY_TRANS_ERROR;
        return SLSY_TRANS_NOACK;
    }
    else
    {
        //未发送出报文
        printf("未发送出去\n");
        close(socket_online);
        socket_online = -1;
        return SLSY_TRANS_ERROR;
    }

    //解析报文
    memset(&cup8583Msg, 0, sizeof(cup8583Msg));
    if (CUP8583_GetMsg(&cup8583Msg, ucRecvMsg + 13, iRecvMsgLen - 13) != 0)
    {
        printf("解析失败\n");
        return SLSY_TRANS_ERROR;
    }

    //15域 清算日期 MMDD
    unsigned char settlementdata[8] = {0};
    ASSERT_FAIL(CUP8583_GetField_ASC(&cup8583Msg, 15, settlementdata, &iLen));
    memcpy(&pTag->settlementDate[4],settlementdata,4);

    //13域 受卡方所在地日期(MMDD)
    unsigned char cardCentertime[8] = {0};
    ASSERT_FAIL(CUP8583_GetField_ASC(&cup8583Msg, 13, cardCentertime, &iLen));
    memcpy(&pTag->cardCenterTime[4],cardCentertime,4);

    //12域 受卡方所在地时间(hhmmss)
    memset(cardCentertime,0,sizeof(cardCentertime));
    ASSERT_FAIL(CUP8583_GetField_ASC(&cup8583Msg, 12, cardCentertime, &iLen));
    memcpy(&pTag->cardCenterTime[8],cardCentertime,6);

    //14域
    ASSERT_FAIL(CUP8583_GetField_ASC(&cup8583Msg, 14, (unsigned char*)pTag->validDate, &iLen));

    //37域 POS中心系统流水号
    ASSERT_FAIL(CUP8583_GetField(&cup8583Msg, 37, (unsigned char*)pTag->cardCenterSerialNo, &iLen));

    //41域 终端编号
    ASSERT_FAIL(CUP8583_GetField(&cup8583Msg, 41, (unsigned char*)pTag->terminalId, &iLen));

    //42域 商户号
    ASSERT_FAIL(CUP8583_GetField(&cup8583Msg, 42, (unsigned char*)pTag->merchantId, &iLen));

    //49域 交易货币代码
    ASSERT_FAIL(CUP8583_GetField(&cup8583Msg, 49, (unsigned char*)pTag->currencyCode, &iLen));

    //获取39域应答码
    memset(ucRspCode, 0, sizeof(ucRspCode));
    ASSERT_FAIL(CUP8583_GetField(&cup8583Msg, 39, ucRspCode, &iLen));
    char str[100]={0};
    sprintf(str,"[39域应答码]%s\n", (char*)ucRspCode);
    memcpy(pTag->retCode,ucRspCode,2);
    if (memcmp(ucRspCode, "\x30\x30", 2) != 0)
    {
        printf("消费失败,应答 %s", (char*)ucRspCode);
        // 发送语音 代码 消费失败
        return SLSY_TRANS_ERROR;//tiaoshi
    }

    //获取64域MAC
    memset(ucRspMac, 0, sizeof(ucRspMac));
    ASSERT_FAIL(CUP8583_GetField(&cup8583Msg, 64, ucRspMac, &iLen));

    //检查MAC
#if 1
    CalMsgMac(g_ucMak, ucRecvMsg + 13, iRecvMsgLen - 13 - 8, ucMac);
    //DebugLog::
    // logprintf("[MAK]:", g_ucMak, 8);
    // logprintf("[MAC(POS 中心)]:", ucRspMac, 8);
    // logprintf("[MAC (本地计算)]:", ucMac, 8);
    if (memcmp(ucMac+1, ucRspMac+1, 7) != 0)
    {
        printf("MAC 校验值不正确\r\n");
        return SLSY_TRANS_ERROR;
    }
#endif
    // DebugLog::Log
    char str1[100]={0};
    sprintf(str1, "[消费成功]\n");
    // logprintf(str1, NULL, 0);
    // printf("消费成功\r\n");
    // 发送语音 代码 消费成功
    return SLSY_TRANS_SUCCESS;
}

int CUPTrans::ConsumeReversing(char* pi_chPan,char pi_chPanLen, char* pi_chPanSn, int i_iTransAmt, unsigned char* pi_ucIccData, int i_iIccDataLen,char *T_5f24)
{
    int iRet, iLen, iSendMsgLen, iRecvMsgLen;
    unsigned char ucRspCode[3];
    unsigned char ucSendMsg[1024];
    unsigned char ucRecvMsg[1024];
    unsigned char ucRspMac[8];
    unsigned char ucMac[8];
    CUP8583_MSG cup8583Msg;

    //获取消费报文
    if(GetConsumeMsgReversing(pi_chPan,pi_chPanLen, pi_chPanSn, i_iTransAmt, pi_ucIccData, i_iIccDataLen, ucSendMsg, &iSendMsgLen,T_5f24) != SLSY_TRANS_SUCCESS)
        return SLSY_TRANS_ERROR;

    //发送报文
    if (iRet = SendRecvMsg(ucSendMsg, iSendMsgLen, ucRecvMsg, &iRecvMsgLen), iRet == 0) {
        //成功
        //DebugLog::Log("收到响应 : ", ucRecvMsg, iRecvMsgLen);
        Print8583Msg2(ucRecvMsg, iRecvMsgLen);
    } else if (iRet < -3) {
        //无收到响应
        //DebugLog::Log("无收到响应\r\n");
        printf("无响应,认为成功\n");
        close(socket_online);
        socket_online=-1;
        //return SLSY_TRANS_ERROR;
        return SLSY_TRANS_SUCCESS;
    } else {
        //未发送出报文
        //DebugLog::Log("未发送出报文\r\n");
        printf("未发送出去\n");
        close(socket_online);
        socket_online=-1;
        return SLSY_TRANS_ERROR;
    }

    //解析报文
    memset(&cup8583Msg, 0, sizeof(cup8583Msg));
    if (CUP8583_GetMsg(&cup8583Msg, ucRecvMsg + 13, iRecvMsgLen - 13) != 0) {
       // DebugLog::Log("解析失败\r\n");
        printf("解析失败\n");
        return SLSY_TRANS_ERROR;
    }

    //获取39域应答码
    memset(ucRspCode, 0, sizeof(ucRspCode));
    ASSERT_FAIL(CUP8583_GetField(&cup8583Msg, 39, ucRspCode, &iLen));
    char str[100]={0};
    sprintf(str,"[39域应答码]%s\n", (char*)ucRspCode);
    //logprintf(str,NULL,0);
    if (memcmp(ucRspCode, "\x30\x30", 2) != 0) {
        // DebugLog::Log("消费失败,应答码:%s\r\n", (char*)ucRspCode);
        printf("消费失败,应答 %s", (char*)ucRspCode);
        // 发送语音 代码 消费失败
        return SLSY_TRANS_ERROR;//tiaoshi
    }

    //获取64域MAC
    memset(ucRspMac, 0, sizeof(ucRspMac));
    ASSERT_FAIL(CUP8583_GetField(&cup8583Msg, 64, ucRspMac, &iLen));

    //检查MAC
    #if 1
    CalMsgMac(g_ucMak, ucRecvMsg + 13, iRecvMsgLen - 13 - 8, ucMac);
    //DebugLog::
    //logprintf("[MAK]:", g_ucMak, 8);
    //logprintf("[MAC(POS 中心)]:", ucRspMac, 8);
    // logprintf("[MAC (本地计算)]:", ucMac, 8);
    if (memcmp(ucMac+1, ucRspMac+1, 7) != 0)
    {
        printf("MAC 校验值不正确\r\n");
        return SLSY_TRANS_ERROR;
    }
    #endif
    // DebugLog::Log
    char str1[100]={0};
    sprintf(str1, "[消费成功]\n");
    // logprintf(str1, NULL, 0);
    // printf("消费成功\r\n");
    // 发送语音 代码 消费成功
    return SLSY_TRANS_SUCCESS;
}

/*********************************************************************************
功	能	:	签退
参	数	:	空
返	回	:	见定义
**********************************************************************************/
int CUPTrans::LogOut()
{
    int iRet, iLen, iSendMsgLen, iRecvMsgLen;
    unsigned char ucRspCode[3];
    unsigned char ucSendMsg[1024];
    unsigned char ucRecvMsg[1024];
    CUP8583_MSG cup8583Msg;

    //获取签退报文
    if (GetLogOutMsg(ucSendMsg, &iSendMsgLen) != SLSY_TRANS_SUCCESS)
        return SLSY_TRANS_ERROR;

    //发送报文
    if (iRet = SendRecvMsg(ucSendMsg, iSendMsgLen, ucRecvMsg, &iRecvMsgLen), iRet == 0)
    {
        //成功
        //DebugLog::Log("收到响应 : ", ucRecvMsg, iRecvMsgLen);
        printf("收到回应\n");
        Print8583Msg2(ucRecvMsg, iRecvMsgLen);
    }
    else if (iRet < -3)
    {
        //无收到响应
        // DebugLog::Log("无收到响应\r\n");
        close(socket_online);
        socket_online=-1;
        printf("无收到回应\n");
        return SLSY_TRANS_ERROR;
    }
    else
    {
        //未发送出报文
        // DebugLog::Log("未发送出报文\r\n");
        close(socket_online);
        socket_online=-1;
        printf("发送失败\n");
        return SLSY_TRANS_ERROR;
    }

    //解析报文
    memset(&cup8583Msg, 0, sizeof(cup8583Msg));
    if (CUP8583_GetMsg(&cup8583Msg, ucRecvMsg + 13, iRecvMsgLen) != 0)
    {
        //DebugLog::Log("解析失败\r\n");
        printf("解析失败\n");
        return SLSY_TRANS_ERROR;
    }

    //获取39域应答码
    memset(ucRspCode, 0, sizeof(ucRspCode));
    ASSERT_FAIL(CUP8583_GetField(&cup8583Msg, 39, ucRspCode, &iLen));

    if (memcmp(ucRspCode, "\x30\x30", 2) != 0)
    {
        // DebugLog::Log
        printf("签退失败,应答码:%s\r\n", (char*)ucRspCode);
        return SLSY_TRANS_ERROR;
    }

    //DebugLog::Log
    printf("签退成功\r\n");

    return SLSY_TRANS_SUCCESS;
}
