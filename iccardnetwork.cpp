#include "iccardnetwork.h"

ICCardNetWork::ICCardNetWork()
{
    m_IcCardServerIp.clear();
    m_IcCardServerPort.clear();
}

void ICCardNetWork::SetParams(QString ip, QString port)
{
    m_IcCardServerIp = ip;
    m_IcCardServerPort = port;
}

void ICCardNetWork::Start()
{
    boost::function<void()> f = boost::bind(&ICCardNetWork::run,this,(void*)0);
    m_thr = boost::thread(f);
}

int ICCardNetWork::ConnectServer()
{
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd == -1){
        printf("qwy --- [ICCardNetWork] creat tcp socket is fail\n");
        return -1;
    }
    struct in_addr stIpAddress;
    struct sockaddr_in stDestSockaddr;
    struct hostent *pHE;
    memset(&stDestSockaddr, 0, sizeof(stDestSockaddr));
    stDestSockaddr.sin_family = AF_INET;
    if ((pHE = gethostbyname((const char*)m_IcCardServerIp.toLatin1().data())) == NULL){
        return -1;
    }
    memcpy(&stIpAddress, pHE->h_addr_list[0], pHE->h_length);
    stDestSockaddr.sin_addr.s_addr = stIpAddress.s_addr;

    if ((long)stDestSockaddr.sin_addr.s_addr == -1){
        return -1;
    }

    stDestSockaddr.sin_port = htons(m_IcCardServerPort.toInt());

    //------------------------------------------------------------------
    // Connect to address "xxx.xxx.xxx.xxx"	(IPv4) address only.
    //------------------------------------------------------------------
    if (connect(sockfd, (struct sockaddr*)&stDestSockaddr, sizeof(stDestSockaddr)) == -1){
        return -1;
    }
    return sockfd;
}

bool ICCardNetWork::SetRecvTimeout(int sockfd, int nRecvTimeoutMillisecond)
{
    bool bRetVal = true;

    struct timeval stRecvTimeout;
    memset(&stRecvTimeout, 0, sizeof(struct timeval));
    stRecvTimeout.tv_sec = nRecvTimeoutMillisecond/1000;
    stRecvTimeout.tv_usec = (nRecvTimeoutMillisecond%1000)*1000;

    //--------------------------------------------------------------------------
    // 检查确定参数是否支持!
    //--------------------------------------------------------------------------
    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&stRecvTimeout,
        sizeof(struct timeval)) == -1)
    {
        bRetVal = false;
    }

    return bRetVal;
}

void ICCardNetWork::run(void* p)
{
    char *cBuffer = new char[GetMaxLength()+1];
    while (1)
    {
        int client = ConnectServer();
        if(client == -1){
            usleep(3000*1000);//3秒
            continue;
        }
        int nRecvBuffer = GetMaxLength()*100;
        if (setsockopt(client, SOL_SOCKET, SO_RCVBUF, (const char*)&nRecvBuffer,sizeof(int)) == -1){
            printf("qwy --- set recvbuffer size is fail,will use default recvbuffer size\n");
        }
        if(!SetRecvTimeout(client,1000)){
            printf("qwy --- set recv timeout is fail\n");
        }
        bool bConnect = true;
        memset(cBuffer,0,GetMaxLength()+1);
        long lRecv = 0;
        while (bConnect)
        {
            int ret = recv(client,&cBuffer[lRecv],GetMaxLength()-lRecv,0);
            if(ret == -1)
            {
                int iError = errno;
                if(iError == ECONNRESET || iError == ECONNABORTED)
                {
                    printf("SOCKET错误，准备重新连接...\n");
                    bConnect = false;
                    break;
                }
                continue;
            }
            lRecv += ret;
            while(lRecv >= GetMinLength())
            {
                int iHead = GetPackageHead(cBuffer,lRecv);
                if(iHead < 0)
                {
                    memset(cBuffer,0,GetMaxLength()+1);
                    lRecv = 0;
                    printf("数据错误，未找到包头...\n");
                    break;
                }
                else
                {
                    char* pDeal = cBuffer + iHead;
                    int iLen = GetPackageLength(pDeal,lRecv-iHead);
                    if(iLen>0 && iLen<=(lRecv-iHead))
                    {
                        NotifyReceivedPackage(pDeal,iLen);
                        lRecv = lRecv-iLen-iHead;
                        memcpy(cBuffer,pDeal+iLen,lRecv);
                    }
                    else if(iLen>GetMaxLength())
                    {
                        printf("数据包长度大于最大包长度.\n");
                        bConnect = false;
                        break;
                    }
                    else
                    {
                        memset(cBuffer,0,GetMaxLength()+1);
                        lRecv = 0;
                        printf("数据包长度错误,长度[%d]\n",iLen);
                        break;
                    }
                }
            }
        }
        usleep(1000*1000);//1秒
    }
    delete[] cBuffer;
}

void ICCardNetWork::NotifyReceivedPackage(char *pBuff, int nSize)
{
    ICCardTcpPack *tagPack = (ICCardTcpPack*)pBuff;
    if(!tagPack->Check()){
        printf("ICCradNetWork 校验错误\n");
        return;
    }
    switch (tagPack->cmd) {
    case value:

        break;
    default:
        break;
    }
}

int ICCardNetWork::GetMinLength()
{
    return 6;
}

int ICCardNetWork::GetMaxLength()
{
    return 1024;
}

int ICCardNetWork::GetPackageLength(char *pBuff, int nSize)
{
   for(int i=nSize-1; i>=0; --i){
       unsigned char tmp = (unsigned char)pBuff[i];
       if(tmp == 0x23)//'#'
           return (i+1);
   }
   return -1;
}

// 增加包头判断 返回0: 表示正常 -1: 表示该Buffer中没有包头数据
int ICCardNetWork::GetPackageHead(char *pBuff, int nSize)
{
    for(int i=0; i<nSize; ++i){
        unsigned char tmp = (unsigned char)pBuff[i];
        if(tmp == 0x40)//'@'
            return i;
    }
    return -1;
}

void tagICCardTcpPack::clear()
{
    head = 0x40;
}

bool tagICCardTcpPack::Check()
{
    unsigned short sLen = 0;
    memcpy(&sLen,datalen,2);
    sLen += 4;
    unsigned char bSum = 0;
    unsigned char* pBuffer = head;
    for(int i = 0; i < sLen; i ++)
        bSum ^= *pBuffer++;
    if(bSum == *pBuffer)
        return true;
    return false;
}

void tagICCardTcpPack::MakeCheckSum(int sLen)
{

}

void tagICCardTcpPack::Transf(int size)
{

}

void tagICCardTcpPack::UnTransf(unsigned char* pdata, int size)
{
    unsigned char* pbuf = &head;
    for(int i=0;i<size;++i)
    {
        if(pdata[i]==0x5E)
        {
            switch(pdata[i+1])
            {
            case 0x01:
                *pbuf = 0x24;
                break;
            case 0x02:
                *pbuf = 0x23;
                break;
            case 0x03:
                *pbuf = 0x40;
                break;
            case 0x04:
                *pbuf = 0x5E;
                break;
            }
        }
        else{
            *pbuf = pdata[i];
        }
    }
}
