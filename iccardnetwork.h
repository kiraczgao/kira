#ifndef ICCARDNETWORK_H
#define ICCARDNETWORK_H

#include <QString>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include "../boost_1_58_0/boost_include.h"

typedef struct tagICCardTcpPack{
public:
    unsigned char head;//0x40 '@'
    unsigned char cmd;
    unsigned char datalen[2];
    unsigned char data[1020];//data+1校验+1结束符(0x23 '#')
public:
    void clear();
    bool Check();
    void MakeCheckSum(int sLen);
    void Transf(int size);//转义
    void UnTransf(unsigned char* pdata, int size);//解转义
}ICCardTcpPack;

class ICCardNetWork
{
public:
    ICCardNetWork();

public:
    void SetParams(QString ip,QString port);
    void Start();
    int ConnectServer();
    // 设置SOCKET接收超时时间
    bool SetRecvTimeout(int sockfd,int nRecvTimeoutMillisecond);

protected:
    void run(void* p);
    void NotifyReceivedPackage(char *pBuff,int nSize);
    // A Package min length
    int GetMinLength();
    // A Package max length
    int GetMaxLength();
    // A Package length
    int GetPackageLength(char *pBuff,int nSize);
    // 增加包头判断 返回0: 表示正常 -1: 表示该Buffer中没有包头数据
    int GetPackageHead(char *pBuff,int nSize);

private:
    QString m_IcCardServerIp;
    QString m_IcCardServerPort;
    boost::thread m_thr;
};

#endif // ICCARDNETWORK_H
