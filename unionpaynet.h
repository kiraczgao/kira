#ifndef UNIONPAYNET_H
#define UNIONPAYNET_H

#include <stdio.h>
#include <string.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <unistd.h>

#include <sys/types.h>
#include <string.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <sys/sysinfo.h>
#include <pthread.h>
#include <termios.h>
#include <QString>
#include "unionpaystructure.h"


class UnionPayNet
{

public:
    UnionPayNet();

public:
    struct sockaddr_in mysin_online;
    pthread_mutex_t socket_mutex;
    int socket_online;
    int g_rebootnet;
    int g_rebootModule;

    //服务器IP
    char g_chServerIp[30];

    //服务器端口
    int g_iServerPort;

public:
    void setServerIp(QString serverIpStr);
    void setServerPort(QString serverPortStr);
    void powerCtrl(int flag);
    void processBeatInfo();

    int socketConnect(char *ip, int port);
    int socketSend(unsigned char *data, int len, int *hadsend);

public:
    int SendRecvMsg(unsigned char *sendData, int lenth_send, unsigned char* pio_ucRecvMsg, int* pio_iRecvMsgLen);
    int connect2server_nob(int *err);
    int Pthread_mutex_init(pthread_mutex_t * mutex);
    int Pthread_mutex_lock(pthread_mutex_t *mutex);
    int Pthread_mutex_unlock(pthread_mutex_t *mutex);
    int powerfd;

};

#endif // UNIONPAYNET_H
