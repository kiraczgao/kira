#include "unionpaynet.h"
#include <QTimer>

UnionPayNet::UnionPayNet()
{
    socket_online = -1;
    g_rebootnet = 0;
    g_rebootModule = 0;
    Pthread_mutex_init(&socket_mutex);

    powerfd = open("/sys/class/gpio/gpio122/value", O_RDWR);
    if(powerfd < 0)
    {
        qDebug("kira --- open 4G power failed: %d", powerfd);
    }
}

void UnionPayNet::setServerIp(QString serverIpStr)
{
    memset(g_chServerIp, 0, sizeof(g_chServerIp));
    memcpy(g_chServerIp, serverIpStr.toLatin1().data(), serverIpStr.length());
}

void UnionPayNet::setServerPort(QString serverPortStr)
{
    g_iServerPort = serverPortStr.toInt();
}

void UnionPayNet::processBeatInfo()
{
    unsigned  char beat[2] = {0x30, 0x30};
    int iRecvMsgLen;
    unsigned char ucRecvMsg[1024];

    int ret = SendRecvMsg(beat, 2, ucRecvMsg, &iRecvMsgLen);

#if 0
    printf("kira --- unionPay beat info: \n");
    for(int i=0; i<iRecvMsgLen; i++)
        printf("%2x ", ucRecvMsg[i]);
    printf("\n");
#endif
}

int UnionPayNet::connect2server_nob(int *err)
{
    int ret;
    int flags;
    int error=0;
    fd_set rd_set, wr_set;
    struct timeval tv_val;
    socklen_t len;

    printf("serverip = %s \n port = %d \n", g_chServerIp, g_iServerPort);
    bzero(&mysin_online,sizeof(mysin_online));
    mysin_online.sin_family = AF_INET;
    mysin_online.sin_addr.s_addr = inet_addr(g_chServerIp);
    mysin_online.sin_port = htons(g_iServerPort);

    socket_online = socket(AF_INET, SOCK_STREAM, 0);
    //printf("socket_online = %d \n", socket_online);
    if (socket_online < 0)
    {
        return -1;
    }

    flags = fcntl(socket_online, F_GETFL, 0);
    fcntl (socket_online, F_SETFL, flags|O_NONBLOCK);
    if ((ret = connect(socket_online, (struct sockaddr*)&mysin_online, sizeof(mysin_online))) < 0)
    {
        if (errno != EINPROGRESS)
        {
            *err = errno;
            return -4;
        }
    }
    if (ret == 0)
    {
        goto done;
    }
    FD_ZERO(&rd_set);
    FD_SET(socket_online, &rd_set);
    wr_set = rd_set;

    tv_val.tv_sec = 2;
    tv_val.tv_usec = 0;

    if ((ret=select(socket_online+1, &rd_set, &wr_set, NULL, &tv_val)) == 0)
    {
        *err= -5;
        return -5;
    }

    if (FD_ISSET(socket_online, &rd_set) || FD_ISSET(socket_online, &wr_set))
    {
        len = sizeof(error);
        if (getsockopt(socket_online, SOL_SOCKET, SO_ERROR, &error, &len) < 0)
        {
            *err = -6;
            return -6;
        }
    }
    else
    {
        *err = -7;
        return -7;
    }

done:
    fcntl(socket_online, F_SETFL, flags);
    if (error)
    {
        errno = error;
        *err = error;
        return -8;
    }
    return 1;
}

//*****SOCKET发送函数****************////
int UnionPayNet::SendRecvMsg(unsigned char *sendData,int lenth_send,unsigned char* pio_ucRecvMsg,int* pio_iRecvMsgLen)
{
    //int i, j;
    int ret=0;
    int ret1=0;
    fd_set wrSet;
    fd_set rdSet;
    int ret_write ;
    struct timeval timeOut;
    int retlen;
    int bufferlen=0;
    //int packetlen;
    //char datalen[5];
    //unsigned char recvbuf[MAX_SIZE];
    //int value=0;
    int connect_ok=0;
    int err;
    int count=0;
    unsigned char str[200] = {0};
    unsigned char cmd[100] = {0};
    Pthread_mutex_lock(&socket_mutex);
    printf("socket = %d \n", socket_online);

begin:
    count++;
    if(socket_online < 0)
    {
        connect_ok = connect2server_nob(&err);
        if(connect_ok > 0)
        {
            printf("connect okok\n");
        }
        else //若连接失败，重启4G 模块
        {
            ///etc/ppp/PPPd call cmnet
            g_rebootnet++;
            if(g_rebootnet>3)
            {
                if(++g_rebootModule > 2)
                {
                    g_rebootModule = 0;
#if 0
                    powerCtrl(1);
                    sleep(5);
                    powerCtrl(0);
                    sleep(5);
#endif
                }

                sprintf((char*)cmd, "/opt/kira/netmodul.pri");
                system((char*)cmd);
                printf("%s \n", cmd);
                printf("连接失败 \n");
                //sprintf(str,"[3次连接失败重启4G模块]\n");
                //logprintf(str, NULL, 0);
                g_rebootnet = 0;
                sleep(2);
            }

#if 0
            connect_ok=connect2server_nob(&err);
            if(connect_ok>0)
            {
                printf("second connect  okok\n");
            }
            else
            {
                printf("second connect fail\n");
                Pthread_mutex_unlock(&socket_mutex);
                return -1;
            }
#else
            Pthread_mutex_unlock(&socket_mutex);
            return -1;
#endif
        }

    }
    else
    {
        printf("网络连接中,不用重连\n");
    }

#if 1
    FD_ZERO(&wrSet);
    FD_SET(socket_online, &wrSet);
    timeOut.tv_sec = 1;
    timeOut.tv_usec = 500000;
    printf("\n ***servtcpsocket_online is = %d\n", socket_online);
    ret1 = select(socket_online+1, NULL, &wrSet, NULL, &timeOut);
    while(ret1 <= 0)
    {
        printf("select 失败\n");
        Pthread_mutex_unlock(&socket_mutex);
        return -1;
    }
#endif

    printf("socket_online的返回值is=%d\n", ret);

    if (FD_ISSET(socket_online, &wrSet))
    {
        ret_write = write(socket_online, sendData, lenth_send);
        //ret_write = send(socket_online, sendData, lenth_send, 0);
        printf("ret_write的长度 = %d\n", ret_write);

        if(ret_write < 0)
        {
            printf("此时网络已经断开,重连 \n");

            if(count < 2)
            {
                close(socket_online);
                socket_online = -1;
                goto begin;
            }
            else
                return -2;
        }

        if(ret_write != lenth_send)
        {
            printf("写socket错误 \n");
            Pthread_mutex_unlock(&socket_mutex);
            return -2;
        }
        else
            printf("写socket成功! \n");

        /////////加入日志文件////////////////
        memset(str,0,200);
        //sprintf(str, "[TCP send len %d]:", lenth_send);
        //sprintf(str, "[TCP send]:");
        //logprintf(str, sendData, lenth_send);
#if 0
        for(i=0;i<lenth_send;i++)
        {
            printf("%.2x ",sendData[i]);
        }
        printf("\n");
#endif
    }
    else
    {
        printf("写数据失败=== \n");
        Pthread_mutex_unlock(&socket_mutex);

        return -1;
    }

    //===========接收判断回应====================
    printf("开始等待接收回应数据==== \n");
    timeOut.tv_sec = 2;
    timeOut.tv_usec = 0;
    FD_ZERO(&rdSet);
    FD_SET(socket_online, &rdSet);
again:
    ret = select(socket_online+1, &rdSet, NULL, NULL, &timeOut);
    printf("ret = %d\n", ret);
    if(ret > 0)
    {
        if (FD_ISSET(socket_online, &rdSet))
        {
            retlen = read(socket_online, pio_ucRecvMsg, MAX_SIZE);
            if(retlen == 0)
            {
                printf("服务端已关闭连接 \n");
                // sprintf(pi_chErrMsg,"服务端已关闭连接");
                printf("====exit send_socket_fd==== \n");
                Pthread_mutex_unlock(&socket_mutex);
                return -6;
            }

            if(retlen < 0)
            {
                printf("接收时出错===\n");
                if(errno == EINTR)
                {
                    printf("errno=EINTR \n");
                    //sprintf(pi_chErrMsg,"errno=EINTR");
                    goto again;
                }
                Pthread_mutex_unlock(&socket_mutex);
                return -6;
            }
            bufferlen += retlen;
            memset(str, 0, 200);

            //sprintf(str, "[TCP receive]:");
            //logprintf(str, pio_ucRecvMsg, bufferlen);
#if 0
            printf("[TCP_online]receive len is %d:\n",bufferlen);
            for(j=0;j<bufferlen;j++)
            {
                printf("%.2x ",pio_ucRecvMsg[j]);
            }
            printf("\n");
#endif
            //赋值
            //pio_ucRecvMsg = recvbuf;
            *pio_iRecvMsgLen = bufferlen;
            Pthread_mutex_unlock(&socket_mutex);
            return 0;
        }
    }
    else
    {
        printf("接收失败\n");
        Pthread_mutex_unlock(&socket_mutex);
        return -6;
    }

    Pthread_mutex_unlock(&socket_mutex);
    return -6;
}

int UnionPayNet::Pthread_mutex_init(pthread_mutex_t * mutex)
{
   if (pthread_mutex_init(&(*mutex),NULL) != 0)
   {
       perror("pthread_mutex_init");
       return 0;
   }
   return 1;
}

int UnionPayNet::Pthread_mutex_lock(pthread_mutex_t *mutex)
{
   if (pthread_mutex_lock(&(*mutex)) != 0)
   {
       perror("pthread_mutex_lock");
       return 0;
   }
   return 1;
}


int UnionPayNet::Pthread_mutex_unlock(pthread_mutex_t *mutex)
{
   if (pthread_mutex_unlock(&(*mutex)) != 0)
   {
       perror("pthread_mutex_unlock");
       return 0;
   }
   return 1;
}

void UnionPayNet::powerCtrl(int flag)
{
    if(powerfd > 0)
    {
        if(flag)
            write(powerfd, "1", 1);
        else
            write(powerfd, "0", 1);
    }
}


int UnionPayNet::socketConnect(char *ip, int port)
{
    int temp;
    int ret;
    struct sockaddr_in server;
    socklen_t len;

    temp = socket(AF_INET, SOCK_STREAM, 0);
    if(temp == -1)
        return -1;

#if 1
    struct timeval connectTime;
    len = sizeof(connectTime);
    connectTime.tv_sec = 5;
    connectTime.tv_usec = 0;
    setsockopt(temp, SOL_SOCKET, SO_SNDTIMEO, &connectTime, len);
#endif

    memset((char *)&server, 0, sizeof(server));
    server.sin_addr.s_addr = inet_addr(ip);
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    printf("kira --- connect ip: %s  port: %d \n", ip, port);
    ret = connect(temp, (struct sockaddr *)&server, sizeof(server));
    printf("kira --- connect ret = %d  errno = %d \n", ret, errno);
    if(ret == -1)
    {
        close(temp);
        return -1;
    }
    fcntl(temp, F_SETFL, O_NONBLOCK);

    return temp;
}
