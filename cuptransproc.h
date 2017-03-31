#ifndef CUPTRANSPROC_H
#define CUPTRANSPROC_H

#include "cup8583.h"
#include "unionpaystructure.h"
#include "setconfig.h"
#include <QDebug>
#include <QString>
#include "datastructure.h"

class CUPTransProc : public CUP8583, public setConfig
{
public:
    CUPTransProc();

public:
    QString unionPayConfigFile;
    QString unionPayGroup;

    //�̻�ID
    char g_chMerchId[16];

    //�ն�ID
    char g_chTermId[9];

    //��ǰ���κ�
    int g_iBatchId;

    //��ǰMAK
    unsigned char g_ucMak[8];

    //����MAK
    unsigned char g_keyMAK[16];

    //�ն�����Կ
    unsigned char TERM_MK[16];

public:
    void setMerchId(QString merchIdStr);
    void setTermId(QString termIdStr);
    void setBatchId(QString batchIdStr);
    void setTermMk(QString termMkStr);

public:
    //��ȡǩ������
    int GetLoginMsg(unsigned char* pio_ucMsg,int* pio_iMsgLen);

    //��ȡǩ�˱���
    int GetLogOutMsg(unsigned char* pio_ucMsg,int* pio_iMsgLen);

    //��ȡ���ѱ���
    int GetConsumeMsg(char* pi_chPan,int i_iTransAmt,unsigned char* pio_ucMsg,int* pio_iMsgLen);

    //��ȡ�ѻ����ѱ���
    int GetOfflineConsumeMsg(unionPayInfo_t* pTag, char* chFiled37, char chFiled37Len, char* pi_chPansn, int i_iTransAmt, unsigned char* pi_ucIccData, int i_iIccDataLen, unsigned char* pio_ucMsg, int* pio_iMsgLen,char *T_5f24);
    int GetConsumeMsgReversing(char* chFiled37, char chFiled37Len,char* pi_chPansn, int i_iTransAmt, unsigned char* pi_ucIccData, int i_iIccDataLen, unsigned char* pio_ucMsg, int* pio_iMsgLen,char *T_5f24);

    int GetbanlanceMsg(unsigned char* pio_ucMsg, int* pio_iMsgLen);

    //������ˮ
    int ResetSeq();

    //��������ˮ
    char* GenSeq(char* pio_chSeq);
    char* NowGenSeq(char* pio_chSeq);

    //����������
    int GenNewBatch();

    //����������
    int SetBatch(char* pi_chBatchId);

    //��ӡ8583����
    int Print8583Msg(CUP8583_MSG i_cup8583Msg);

    //��ӡ8583����,ͨ������
    int Print8583Msg2(unsigned char* pi_ucMsg,int i_iMsgLen);

    unsigned char HexToAsc(unsigned char aHex);
    unsigned char AscToHex(unsigned char aChar);
};

#endif // CUPTRANSPROC_H
