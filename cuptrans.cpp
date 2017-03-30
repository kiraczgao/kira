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
��	��	:	ǩ��
��	��	:	��
��	��	:	������
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
    //��ȡǩ������
    printf("into login\n");
    if (GetLoginMsg(ucLoginSendMsg, &iLoginSendMsgLen) != SLSY_TRANS_SUCCESS)
        return SLSY_TRANS_ERROR;

    printf("send login information... \n");
    //���ͱ���
    if (iRet = SendRecvMsg(ucLoginSendMsg, iLoginSendMsgLen, ucLoginRecvMsg, &iLoginRecvMsgLen), iRet == 0)
    {
        //�ɹ�
        //logprintf("[�յ���Ӧ]:", ucLoginRecvMsg, iLoginRecvMsgLen);
        printf("�յ���Ӧ \n");
        Print8583Msg2(ucLoginRecvMsg, iLoginRecvMsgLen);
    }
    else if (iRet < -3)
    {
        close(socket_online);
        socket_online = -1;
        //���յ���Ӧ
        //logprintf("[���յ���Ӧ]\n", NULL, 0);
        printf("����Ӧ \n");
        return SLSY_TRANS_ERROR;
    }
    else
    {
        //δ���ͳ�����
        close(socket_online);
        socket_online = -1;
        //logprintf("[δ���ͳɹ�]\n", NULL,0);
        printf("δ���ͳɹ� \n");
        return SLSY_TRANS_ERROR;
    }

    //��������
    memset(&cup8583Msg, 0, sizeof(cup8583Msg));
    if (CUP8583_GetMsg(&cup8583Msg, ucLoginRecvMsg + 13, iLoginRecvMsgLen) != 0)
    {
        printf("����ʧ��\n");
        return SLSY_TRANS_ERROR;
    }

    //��ȡ39��Ӧ����
    memset(ucRspCode, 0, sizeof(ucRspCode));
    ASSERT_FAIL(CUP8583_GetField(&cup8583Msg, 39, ucRspCode, &iLen));
    if (memcmp(ucRspCode, "\x30\x30", 2) != 0)
    {
        printf("ǩ��ʧ��,Ӧ����:%s\r\n", (char*)ucRspCode);
        return SLSY_TRANS_ERROR;
    }
    //��ȡ��ȡǩ��������ˮ��
    //   ASSERT_FAIL(CUP8583_GetField(&cup8583Msg, 11, chSeq, &iLen));
    //   WritePrivateProfileString("TermParam", "Seq", chSeq, LOCALCONFIG);
    //��ȡ60������
    ASSERT_FAIL(CUP8583_GetField(&cup8583Msg, 60, ucFiled60, &iLen));
    printf("60������ : %s\r\n", (char*)ucFiled60);
    memset(chNNewBatch, 0, sizeof(chNNewBatch));
    memcpy(chNNewBatch, ucFiled60 + 2, 6);
    SetBatch(chNNewBatch);

    //��ȡ������Կ
    ASSERT_FAIL(CUP8583_GetField(&cup8583Msg, 62, ucWorkKey, &iLen));
    //logprintf("[������Կ]: ", ucWorkKey, iLen);
    memcpy(g_keyMAK, ucWorkKey + 20, 16);
    //logprintf("[������ԿMAK����]: ", g_keyMAK, 16);
    //logprintf("[������ԿMAKУ��]: ", &ucWorkKey[36], 4);
    //���ܹ�����Կ
    memcpy(ucMkKeyA, TERM_MK, 8);
    memcpy(ucMkKeyB, TERM_MK+8, 8);
    //////////////////////////////////////////////wait
    DES(1, ucMkKeyA, ucWorkKey + 20, g_ucMaktemp1);
    DES(0, ucMkKeyB, g_ucMaktemp1,  g_ucMaktemp2);
    DES(1, ucMkKeyA, g_ucMaktemp2, g_ucMak);

    DES(0, g_ucMak, (unsigned char*)"\x00\x00\x00\x00\x00\x00\x00\x00", ucMakCk);
    //logprintf("[���ؼ����MAKУ��]:", ucMakCk, 4);
    if (memcmp(ucMakCk, ucWorkKey + 36, 4) != 0)
    {
        printf("===MAKУ�����\n====\r\n");
        return SLSY_TRANS_ERROR;
    }

    //����MAK
    memset(chMak, 0, sizeof(chMak));
    HextoAsc(g_keyMAK, 16, &chMak[0]);
    printf("strlenofmak = %d\n", strlen((char*)chMak));

    QString makstr = QByteArray::fromRawData((char*)chMak, strlen((char*)chMak));
    writeSetInfo(unionPayConfigFile, unionPayGroup, "MAK", makstr);

    printf("ǩ���ɹ�\r\n");
    // �������� ���� 99
    return SLSY_TRANS_SUCCESS;
}

/*********************************************************************************
��	��	:	����
��	��	:	pi_chPan - ����
��	��	:	i_iTransAmt - ���׽��Է�Ϊ��λ
��	��	:	������
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

    //��ȡ���ѱ���
    if (GetConsumeMsg(pi_chPan, i_iTransAmt, ucSendMsg, &iSendMsgLen) != SLSY_TRANS_SUCCESS)
        return SLSY_TRANS_ERROR;

    //���ͱ���
    if (iRet = SendRecvMsg(ucSendMsg, iSendMsgLen, ucRecvMsg, &iRecvMsgLen), iRet == 0)
    {
        //�ɹ�
        //DebugLog::Log("�յ���Ӧ : ", ucRecvMsg, iRecvMsgLen);
        printf("�յ���Ӧ\n");
        Print8583Msg2(ucRecvMsg, iRecvMsgLen);
    }
    else if (iRet < -3)
    {
        //���յ���Ӧ
        // DebugLog::Log
        close(socket_online);
        socket_online = -1;
        printf("���յ���Ӧ\r\n");
        return SLSY_TRANS_ERROR;
    }
    else
    {
        //δ���ͳ�����
        // DebugLog::Log
        close(socket_online);
        socket_online = -1;
        printf("δ���ͳ�����\r\n");
        return SLSY_TRANS_ERROR;
    }

    //��������
    memset(&cup8583Msg, 0, sizeof(cup8583Msg));
    if (CUP8583_GetMsg(&cup8583Msg, ucRecvMsg + 13, iRecvMsgLen - 13) != 0)
    {
        //DebugLog::Log("����ʧ��\r\n");
        printf("����ʧ��\n");
        return SLSY_TRANS_ERROR;
    }

    //��ȡ39��Ӧ����
    memset(ucRspCode, 0, sizeof(ucRspCode));
    ASSERT_FAIL(CUP8583_GetField(&cup8583Msg, 39, ucRspCode, &iLen));
    if (memcmp(ucRspCode, "\x30\x30", 2) != 0)
    {
        //DebugLog::Log("����ʧ��,Ӧ����:%s\r\n", (char*)ucRspCode);
        printf("����Ӧ�����\n");
        return SLSY_TRANS_ERROR;
    }

    //��ȡ64��MAC
    memset(ucRspMac, 0, sizeof(ucRspMac));
    ASSERT_FAIL(CUP8583_GetField(&cup8583Msg, 64, ucRspMac, &iLen));

    //���MAC
    CalMsgMac(g_ucMak, ucRecvMsg + 13, iRecvMsgLen - 13 - 8, ucMac);
    // DebugLog::Log("MAK : ", g_ucMak, 8);
    // DebugLog::Log("MAC (POS ����) : ", ucRspMac, 8);
    // DebugLog::Log("MAC (���ؼ���) : ", ucMac, 8);
    if (memcmp(ucMac, ucRspMac, 8) != 0)
    {
        // DebugLog::Log("MAC У��ֵ����ȷ\r\n");
        printf("MAC У�鲻��\n");
        return SLSY_TRANS_ERROR;
    }

    //DebugLog::Log("���ѳɹ�\r\n");
    printf("���ѳɹ�\n");

    return SLSY_TRANS_SUCCESS;
}

/*********************************************************************************
��	��	:	�ѻ�����
��	��	:	pi_chPan - ��5A
��	��	:	i_iTransAmt - ���׽��Է�Ϊ��λ
��	��	:	������
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
    //��ȡ���ѱ���
    if(GetOfflineConsumeMsg(pTag,pi_chPan,pi_chPanLen, pi_chPanSn, i_iTransAmt, pi_ucIccData, i_iIccDataLen, ucSendMsg, &iSendMsgLen,T_5f24) != SLSY_TRANS_SUCCESS)
        return SLSY_TRANS_ERROR;

    //���ͱ���
    if (iRet = SendRecvMsg(ucSendMsg, iSendMsgLen, ucRecvMsg, &iRecvMsgLen), iRet == 0)
    {
        //�ɹ�
        Print8583Msg2(ucRecvMsg, iRecvMsgLen);
    }
    else if (iRet < -3)
    {
        //���յ���Ӧ
        printf("����Ӧ,��Ϊ�ɹ�\n");
        close(socket_online);
        socket_online=-1;
        //return SLSY_TRANS_ERROR;
        return SLSY_TRANS_NOACK;
    }
    else
    {
        //δ���ͳ�����
        printf("δ���ͳ�ȥ\n");
        close(socket_online);
        socket_online = -1;
        return SLSY_TRANS_ERROR;
    }

    //��������
    memset(&cup8583Msg, 0, sizeof(cup8583Msg));
    if (CUP8583_GetMsg(&cup8583Msg, ucRecvMsg + 13, iRecvMsgLen - 13) != 0)
    {
        printf("����ʧ��\n");
        return SLSY_TRANS_ERROR;
    }

    //15�� �������� MMDD
    unsigned char settlementdata[8] = {0};
    ASSERT_FAIL(CUP8583_GetField_ASC(&cup8583Msg, 15, settlementdata, &iLen));
    memcpy(&pTag->settlementDate[4],settlementdata,4);

    //13�� �ܿ������ڵ�����(MMDD)
    unsigned char cardCentertime[8] = {0};
    ASSERT_FAIL(CUP8583_GetField_ASC(&cup8583Msg, 13, cardCentertime, &iLen));
    memcpy(&pTag->cardCenterTime[4],cardCentertime,4);

    //12�� �ܿ������ڵ�ʱ��(hhmmss)
    memset(cardCentertime,0,sizeof(cardCentertime));
    ASSERT_FAIL(CUP8583_GetField_ASC(&cup8583Msg, 12, cardCentertime, &iLen));
    memcpy(&pTag->cardCenterTime[8],cardCentertime,6);

    //14��
    ASSERT_FAIL(CUP8583_GetField_ASC(&cup8583Msg, 14, (unsigned char*)pTag->validDate, &iLen));

    //37�� POS����ϵͳ��ˮ��
    ASSERT_FAIL(CUP8583_GetField(&cup8583Msg, 37, (unsigned char*)pTag->cardCenterSerialNo, &iLen));

    //41�� �ն˱��
    ASSERT_FAIL(CUP8583_GetField(&cup8583Msg, 41, (unsigned char*)pTag->terminalId, &iLen));

    //42�� �̻���
    ASSERT_FAIL(CUP8583_GetField(&cup8583Msg, 42, (unsigned char*)pTag->merchantId, &iLen));

    //49�� ���׻��Ҵ���
    ASSERT_FAIL(CUP8583_GetField(&cup8583Msg, 49, (unsigned char*)pTag->currencyCode, &iLen));

    //��ȡ39��Ӧ����
    memset(ucRspCode, 0, sizeof(ucRspCode));
    ASSERT_FAIL(CUP8583_GetField(&cup8583Msg, 39, ucRspCode, &iLen));
    char str[100]={0};
    sprintf(str,"[39��Ӧ����]%s\n", (char*)ucRspCode);
    memcpy(pTag->retCode,ucRspCode,2);
    if (memcmp(ucRspCode, "\x30\x30", 2) != 0)
    {
        printf("����ʧ��,Ӧ�� %s", (char*)ucRspCode);
        // �������� ���� ����ʧ��
        return SLSY_TRANS_ERROR;//tiaoshi
    }

    //��ȡ64��MAC
    memset(ucRspMac, 0, sizeof(ucRspMac));
    ASSERT_FAIL(CUP8583_GetField(&cup8583Msg, 64, ucRspMac, &iLen));

    //���MAC
#if 1
    CalMsgMac(g_ucMak, ucRecvMsg + 13, iRecvMsgLen - 13 - 8, ucMac);
    //DebugLog::
    // logprintf("[MAK]:", g_ucMak, 8);
    // logprintf("[MAC(POS ����)]:", ucRspMac, 8);
    // logprintf("[MAC (���ؼ���)]:", ucMac, 8);
    if (memcmp(ucMac+1, ucRspMac+1, 7) != 0)
    {
        printf("MAC У��ֵ����ȷ\r\n");
        return SLSY_TRANS_ERROR;
    }
#endif
    // DebugLog::Log
    char str1[100]={0};
    sprintf(str1, "[���ѳɹ�]\n");
    // logprintf(str1, NULL, 0);
    // printf("���ѳɹ�\r\n");
    // �������� ���� ���ѳɹ�
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

    //��ȡ���ѱ���
    if(GetConsumeMsgReversing(pi_chPan,pi_chPanLen, pi_chPanSn, i_iTransAmt, pi_ucIccData, i_iIccDataLen, ucSendMsg, &iSendMsgLen,T_5f24) != SLSY_TRANS_SUCCESS)
        return SLSY_TRANS_ERROR;

    //���ͱ���
    if (iRet = SendRecvMsg(ucSendMsg, iSendMsgLen, ucRecvMsg, &iRecvMsgLen), iRet == 0) {
        //�ɹ�
        //DebugLog::Log("�յ���Ӧ : ", ucRecvMsg, iRecvMsgLen);
        Print8583Msg2(ucRecvMsg, iRecvMsgLen);
    } else if (iRet < -3) {
        //���յ���Ӧ
        //DebugLog::Log("���յ���Ӧ\r\n");
        printf("����Ӧ,��Ϊ�ɹ�\n");
        close(socket_online);
        socket_online=-1;
        //return SLSY_TRANS_ERROR;
        return SLSY_TRANS_SUCCESS;
    } else {
        //δ���ͳ�����
        //DebugLog::Log("δ���ͳ�����\r\n");
        printf("δ���ͳ�ȥ\n");
        close(socket_online);
        socket_online=-1;
        return SLSY_TRANS_ERROR;
    }

    //��������
    memset(&cup8583Msg, 0, sizeof(cup8583Msg));
    if (CUP8583_GetMsg(&cup8583Msg, ucRecvMsg + 13, iRecvMsgLen - 13) != 0) {
       // DebugLog::Log("����ʧ��\r\n");
        printf("����ʧ��\n");
        return SLSY_TRANS_ERROR;
    }

    //��ȡ39��Ӧ����
    memset(ucRspCode, 0, sizeof(ucRspCode));
    ASSERT_FAIL(CUP8583_GetField(&cup8583Msg, 39, ucRspCode, &iLen));
    char str[100]={0};
    sprintf(str,"[39��Ӧ����]%s\n", (char*)ucRspCode);
    //logprintf(str,NULL,0);
    if (memcmp(ucRspCode, "\x30\x30", 2) != 0) {
        // DebugLog::Log("����ʧ��,Ӧ����:%s\r\n", (char*)ucRspCode);
        printf("����ʧ��,Ӧ�� %s", (char*)ucRspCode);
        // �������� ���� ����ʧ��
        return SLSY_TRANS_ERROR;//tiaoshi
    }

    //��ȡ64��MAC
    memset(ucRspMac, 0, sizeof(ucRspMac));
    ASSERT_FAIL(CUP8583_GetField(&cup8583Msg, 64, ucRspMac, &iLen));

    //���MAC
    #if 1
    CalMsgMac(g_ucMak, ucRecvMsg + 13, iRecvMsgLen - 13 - 8, ucMac);
    //DebugLog::
    //logprintf("[MAK]:", g_ucMak, 8);
    //logprintf("[MAC(POS ����)]:", ucRspMac, 8);
    // logprintf("[MAC (���ؼ���)]:", ucMac, 8);
    if (memcmp(ucMac+1, ucRspMac+1, 7) != 0)
    {
        printf("MAC У��ֵ����ȷ\r\n");
        return SLSY_TRANS_ERROR;
    }
    #endif
    // DebugLog::Log
    char str1[100]={0};
    sprintf(str1, "[���ѳɹ�]\n");
    // logprintf(str1, NULL, 0);
    // printf("���ѳɹ�\r\n");
    // �������� ���� ���ѳɹ�
    return SLSY_TRANS_SUCCESS;
}

/*********************************************************************************
��	��	:	ǩ��
��	��	:	��
��	��	:	������
**********************************************************************************/
int CUPTrans::LogOut()
{
    int iRet, iLen, iSendMsgLen, iRecvMsgLen;
    unsigned char ucRspCode[3];
    unsigned char ucSendMsg[1024];
    unsigned char ucRecvMsg[1024];
    CUP8583_MSG cup8583Msg;

    //��ȡǩ�˱���
    if (GetLogOutMsg(ucSendMsg, &iSendMsgLen) != SLSY_TRANS_SUCCESS)
        return SLSY_TRANS_ERROR;

    //���ͱ���
    if (iRet = SendRecvMsg(ucSendMsg, iSendMsgLen, ucRecvMsg, &iRecvMsgLen), iRet == 0)
    {
        //�ɹ�
        //DebugLog::Log("�յ���Ӧ : ", ucRecvMsg, iRecvMsgLen);
        printf("�յ���Ӧ\n");
        Print8583Msg2(ucRecvMsg, iRecvMsgLen);
    }
    else if (iRet < -3)
    {
        //���յ���Ӧ
        // DebugLog::Log("���յ���Ӧ\r\n");
        close(socket_online);
        socket_online=-1;
        printf("���յ���Ӧ\n");
        return SLSY_TRANS_ERROR;
    }
    else
    {
        //δ���ͳ�����
        // DebugLog::Log("δ���ͳ�����\r\n");
        close(socket_online);
        socket_online=-1;
        printf("����ʧ��\n");
        return SLSY_TRANS_ERROR;
    }

    //��������
    memset(&cup8583Msg, 0, sizeof(cup8583Msg));
    if (CUP8583_GetMsg(&cup8583Msg, ucRecvMsg + 13, iRecvMsgLen) != 0)
    {
        //DebugLog::Log("����ʧ��\r\n");
        printf("����ʧ��\n");
        return SLSY_TRANS_ERROR;
    }

    //��ȡ39��Ӧ����
    memset(ucRspCode, 0, sizeof(ucRspCode));
    ASSERT_FAIL(CUP8583_GetField(&cup8583Msg, 39, ucRspCode, &iLen));

    if (memcmp(ucRspCode, "\x30\x30", 2) != 0)
    {
        // DebugLog::Log
        printf("ǩ��ʧ��,Ӧ����:%s\r\n", (char*)ucRspCode);
        return SLSY_TRANS_ERROR;
    }

    //DebugLog::Log
    printf("ǩ�˳ɹ�\r\n");

    return SLSY_TRANS_SUCCESS;
}
