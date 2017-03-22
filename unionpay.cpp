#include "unionpay.h"
#include <QByteArray>

//unionPay::unionPay(QObject *parent) : QObject(parent)
unionPay::unionPay()
{
    //新建一个QTimer对象
    myTimer = new QTimer();
    //设置定时器每个多少毫秒发送一个timeout()信号
    myTimer->setInterval(60*1000);
    //启动定时器
    myTimer->start();
    connect(myTimer, SIGNAL(timeout()), this, SLOT(onTimerOut()));

    //新建一个QTimer对象
    unionPayBeatTimer = new QTimer();
    //设置定时器每个多少毫秒发送一个timeout()信号
    unionPayBeatTimer->setInterval(120*1000);
    //暂停定时器
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
        printf("签到失败，重新签到 \n");
    }
    //unionPayBeatTimer->start();
    printf("签到成功 \n");
}

int unionPay::processA2Talk(unionPayInfo_t* pTag,cardreceiveA2 unionPayProcA2, int tickets)
{
    int ret = myCUPTrans.OfflineConsume(pTag,unionPayProcA2.citiao2data, unionPayProcA2.citiao2data_l, unionPayProcA2.serialno, \
                                        tickets, unionPayProcA2.data55, (int)unionPayProcA2.len55, unionPayProcA2.validdate);
    printf("\n offlineconsume ret = %d \n", ret);
    if(0 == ret)
    {
        printf("交易成功 \n");
        emit unionPayTradeSuccess();
        // kira --- test
        //myCUPTrans.ConsumeReversing(unionPayProcA2.citiao2data, unionPayProcA2.citiao2data_l, unionPayProcA2.serialno, \
        //                            tickets, unionPayProcA2.data55, (int)unionPayProcA2.len55, unionPayProcA2.validdate);
        return 0;
    }
    else
    {
        printf("交易失败 \n");
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
