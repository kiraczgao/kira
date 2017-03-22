#ifndef CUPTRANS_H
#define CUPTRANS_H

#include "cuptransproc.h"
#include "unionpaystructure.h"
#include "unionpaynet.h"
#include "datastructure.h"

class CUPTrans : public CUPTransProc, public UnionPayNet
{
public:
    CUPTrans();

public:
    int ifield39;
    void getIfield39(int* pfield39) { *pfield39 = ifield39; }
    void BCDToMMDD(unsigned char* bcd,char* ret);
    void BCDTohhmmss(unsigned char* bcd,char* ret);

public:
    int Login();
    int Consume(char* pi_chPan, int i_iTransAmt);
    int OfflineConsume(unionPayInfo_t* pTag,char* pi_chPan, char pi_chPanLen, char* pi_chPanSn, int i_iTransAmt, unsigned char* pi_ucIccData, int i_iIccDataLen,char *T_5f24);
    int ConsumeReversing(char* pi_chPan,char pi_chPanLen, char* pi_chPanSn, int i_iTransAmt, unsigned char* pi_ucIccData, int i_iIccDataLen,char *T_5f24);
    int LogOut();
};

#endif // CUPTRANS_H
