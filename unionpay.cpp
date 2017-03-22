#include "unionpay.h"
#include <QByteArray>

//unionPay::unionPay(QObject *parent) : QObject(parent)
unionPay::unionPay()
{
    //�½�һ��QTimer����
    myTimer = new QTimer();
    //���ö�ʱ��ÿ�����ٺ��뷢��һ��timeout()�ź�
    myTimer->setInterval(60*1000);
    //������ʱ��
    myTimer->start();
    connect(myTimer, SIGNAL(timeout()), this, SLOT(onTimerOut()));

    //�½�һ��QTimer����
    unionPayBeatTimer = new QTimer();
    //���ö�ʱ��ÿ�����ٺ��뷢��һ��timeout()�ź�
    unionPayBeatTimer->setInterval(120*1000);
    //��ͣ��ʱ��
    unionPayBeatTimer->stop();
    QObject::connect(unionPayBeatTimer, SIGNAL(timeout()), this, SLOT(beatTimerOut()));

    sleep(3);
    emit unionPayA1Talk();
}

void unionPay::initUnionPayConfig()
{
    myCUPTrans.unionPayConfigFile = "/opt/kira/unionPay/unionPaySetting.ini";
    myCUPTrans.unionPayGroup = "unionPay";

    myCUPTrans.readSetInfo(myCUPTrans.unionPayConfigFile, myCUPTrans.unionPayGroup, "serverIp", serverIpstr);
    myCUPTrans.readSetInfo(myCUPTrans.unionPayConfigFile, myCUPTrans.unionPayGroup, "serverPort", serverPortstr);
    myCUPTrans.readSetInfo(myCUPTrans.unionPayConfigFile, myCUPTrans.unionPayGroup, "merchId", merchIdstr);
    myCUPTrans.readSetInfo(myCUPTrans.unionPayConfigFile, myCUPTrans.unionPayGroup, "termId", termIdstr);
    myCUPTrans.readSetInfo(myCUPTrans.unionPayConfigFile, myCUPTrans.unionPayGroup, "batchId", batchIdstr);
    myCUPTrans.readSetInfo(myCUPTrans.unionPayConfigFile, myCUPTrans.unionPayGroup, "termMk", termMkstr);

    myCUPTrans.setServerIp(serverIpstr);
    myCUPTrans.setServerPort(serverPortstr);
    myCUPTrans.setMerchId(merchIdstr);
    myCUPTrans.setTermId(termIdstr);
    myCUPTrans.setBatchId(batchIdstr);
    myCUPTrans.setTermMk(termMkstr);
}

void unionPay::recordTermMk(QString stermMk)
{
    myCUPTrans.writeSetInfo(myCUPTrans.unionPayConfigFile, myCUPTrans.unionPayGroup, "termMk", stermMk);
    myCUPTrans.setTermMk(stermMk);
}

void unionPay::onTimerOut()
{
    emit unionPayA1Talk();
}

void unionPay::beatTimerOut()
{
    myCUPTrans.processBeatInfo();
}

void unionPay::stop()
{

}

void unionPay::run()
{
    while(1)
    {
        int ret = myCUPTrans.Login();
        if(0 == ret)
            break;
        sleep(5);
        printf("ǩ��ʧ�ܣ�����ǩ�� \n");
    }
    //unionPayBeatTimer->start();
    printf("ǩ���ɹ� \n");
}

int unionPay::processA2Talk(unionPayInfo_t* pTag,cardreceiveA2 unionPayProcA2, int tickets)
{
    int ret = myCUPTrans.OfflineConsume(pTag,unionPayProcA2.citiao2data, unionPayProcA2.citiao2data_l, unionPayProcA2.serialno, \
                                        tickets, unionPayProcA2.data55, (int)unionPayProcA2.len55, unionPayProcA2.validdate);
    printf("\n offlineconsume ret = %d \n", ret);
    if(0 == ret)
    {
        printf("���׳ɹ� \n");
        emit unionPayTradeSuccess();
        // kira --- test
        //myCUPTrans.ConsumeReversing(unionPayProcA2.citiao2data, unionPayProcA2.citiao2data_l, unionPayProcA2.serialno, \
        //                            tickets, unionPayProcA2.data55, (int)unionPayProcA2.len55, unionPayProcA2.validdate);
        return 0;
    }
    else
    {
        printf("����ʧ�� \n");
        if(0xff == ret)
            myCUPTrans.ConsumeReversing(unionPayProcA2.citiao2data, unionPayProcA2.citiao2data_l, unionPayProcA2.serialno, \
                                        tickets, unionPayProcA2.data55, (int)unionPayProcA2.len55, unionPayProcA2.validdate);
        emit unionPayTradeFailed();
        return -1;
    }
}

void unionPay::getUnionPayTradeRet(int *pret)
{
    myCUPTrans.getIfield39(pret);
}
