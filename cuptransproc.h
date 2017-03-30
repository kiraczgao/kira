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

    //商户ID
    char g_chMerchId[16];

    //终端ID
    char g_chTermId[9];

    //当前批次号
    int g_iBatchId;

    //当前MAK
    unsigned char g_ucMak[8];

    //密文MAK
    unsigned char g_keyMAK[16];

    //终端主密钥
    unsigned char TERM_MK[16];

public:
    void setMerchId(QString merchIdStr);
    void setTermId(QString termIdStr);
    void setBatchId(QString batchIdStr);
    void setTermMk(QString termMkStr);

public:
    //获取签到报文
    int GetLoginMsg(unsigned char* pio_ucMsg,int* pio_iMsgLen);

    //获取签退报文
    int GetLogOutMsg(unsigned char* pio_ucMsg,int* pio_iMsgLen);

    //获取消费报文
    int GetConsumeMsg(char* pi_chPan,int i_iTransAmt,unsigned char* pio_ucMsg,int* pio_iMsgLen);

    //获取脱机消费报文
    int GetOfflineConsumeMsg(unionPayInfo_t* pTag, char* chFiled37, char chFiled37Len, char* pi_chPansn, int i_iTransAmt, unsigned char* pi_ucIccData, int i_iIccDataLen, unsigned char* pio_ucMsg, int* pio_iMsgLen,char *T_5f24);
    int GetConsumeMsgReversing(char* chFiled37, char chFiled37Len,char* pi_chPansn, int i_iTransAmt, unsigned char* pi_ucIccData, int i_iIccDataLen, unsigned char* pio_ucMsg, int* pio_iMsgLen,char *T_5f24);

    int GetbanlanceMsg(unsigned char* pio_ucMsg, int* pio_iMsgLen);

    //重置流水
    int ResetSeq();

    //生成新流水
    char* GenSeq(char* pio_chSeq);
    char* NowGenSeq(char* pio_chSeq);

    //生成新批次
    int GenNewBatch();

    //设置新批次
    int SetBatch(char* pi_chBatchId);

    //打印8583报文
    int Print8583Msg(CUP8583_MSG i_cup8583Msg);

    //打印8583报文,通过报文
    int Print8583Msg2(unsigned char* pi_ucMsg,int i_iMsgLen);

    unsigned char HexToAsc(unsigned char aHex);
    unsigned char AscToHex(unsigned char aChar);
};

#endif // CUPTRANSPROC_H
