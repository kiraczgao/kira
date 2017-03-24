#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <sys/reboot.h>
#include <netinet/in.h>
#include <QFile>
#include <QByteArray>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QTimer>
#include <QEventLoop>
#include <QMutexLocker>
#include "stmserialtalk.h"
#include "appUpdater.h"
#include "crc32.h"

//静态成员初始化
quint32 ftpClient::MAX_WRITE_BYTES = 1024 * 1024;   //每次最大写入1MB
quint32 ftpClient::MAX_TIMEOUT = 1000 * 60 * 3;     //操作超时3分钟

ftpClient::ftpClient() : m_pReply(NULL)
{
    //分配内存
    m_pmanager = new QNetworkAccessManager(this);
    m_pRequest = new QNetworkRequest();
    m_ptimer = new QTimer();
    if(!m_pmanager || !m_pRequest || !m_ptimer)
    {
        printf("[CRITIAL ERROR]ftpClient bad alloc!!\n");
    }
    else
    {
        //连接信号与槽
        connect(m_pmanager,SIGNAL(finished(QNetworkReply*)),this,SLOT(dealFinish(QNetworkReply*)));
        connect(m_ptimer, SIGNAL(timeout()), this, SLOT(dealtimeOut()));
        //设定定时器超时时间
        m_ptimer->setInterval(MAX_TIMEOUT);
    }
}

ftpClient::~ftpClient()
{
    if(m_pRequest)
    {
        delete m_pRequest;
    }
    if(m_pReply)
    {
        m_pReply->deleteLater();
    }
    if(m_pmanager)
    {
        m_pmanager->deleteLater();
    }
    if(m_ptimer)
    {
        if(m_ptimer->isActive())
        {
            m_ptimer->stop();
        }
        m_ptimer->deleteLater();
    }
}

/** @function
********************************************************************************
<PRE>
函数名: put
功能:  上传功能函数-重载1
抛出异常: 否
--------------------------------------------------------------------------------
备注: ftpClient类成员函数
--------------------------------------------------------------------------------
</PRE>
*******************************************************************************/

bool ftpClient::put(const QUrl &url, const QByteArray &data)
{
    //有效性判断
    if(data.isEmpty())
    {
        printf("[ftp put]upload data CAN NOT be empty!\n");
        return false;
    }
    if(url.isEmpty() || url.scheme() != "ftp")
    {
        printf("[ftp put]upload url INCORRECT!\n");
        return false;
    }
    //启动定时器
    m_ptimer->start();
    m_pRequest->setUrl(url);
    m_pReply = m_pmanager->put(*m_pRequest, data);
    connect(m_pReply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(dealError(QNetworkReply::NetworkError)));
    connect(m_pReply, SIGNAL(uploadProgress(qint64,qint64)), this, SLOT(dealuploadProgress(qint64,qint64)));
    return true;
}

/** @function
********************************************************************************
<PRE>
函数名: put
功能:  上传功能函数-重载2
抛出异常: 否
--------------------------------------------------------------------------------
备注: ftpClient类成员函数
--------------------------------------------------------------------------------
</PRE>
*******************************************************************************/

bool ftpClient::put(const QUrl &url, QFile *pFile)
{
    //有效性判断
    if(!pFile)
    {
        printf("[ftp put]file pointer is illegal!\n");
        return false;
    }
    if(url.isEmpty() || url.scheme() != "ftp")
    {
        printf("[ftp put]upload url INCORRECT!\n");
        return false;
    }
    //检查文件合法性
    if(!pFile->isOpen())
    {
        if(!pFile->open(QIODevice::ReadOnly))
        {
            printf("[ftp put]file:%s open failed!\n", pFile->fileName().toLatin1().constData());
            return false;
        }
    }
    if(!pFile->isReadable())
    {
        if(!pFile->open(QIODevice::ReadOnly))
        {
            printf("[ftp put]file:%s is NOT Readable!\n", pFile->fileName().toLatin1().constData());
            return false;
        }
    }
    //启动定时器
    m_ptimer->start();
    m_pRequest->setUrl(url);
    m_pReply = m_pmanager->put(*m_pRequest, pFile);
    connect(m_pReply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(dealError(QNetworkReply::NetworkError)));
    connect(m_pReply, SIGNAL(uploadProgress(qint64,qint64)), this, SLOT(dealuploadProgress(qint64,qint64)));
    return true;
}

/** @function
********************************************************************************
<PRE>
函数名: get
功能:  下载FTP文件功能函数
抛出异常: 否
--------------------------------------------------------------------------------
备注: ftpClient类成员函数
--------------------------------------------------------------------------------
</PRE>
*******************************************************************************/

bool ftpClient::get(const QUrl &url, QFile *pFile)
{
    //有效性判断
    if(url.isEmpty() || url.scheme() != "ftp")
    {
        printf("[ftp put]download url INCORRECT!\n");
        return false;
    }
    if(!pFile)
    {
        printf("[ftp get]file pointer is illegal!\n");
        return false;
    }
    //打开文件
    if(!pFile->isOpen())
    {
        //尝试以方式打开文件
        if(!pFile->open(QIODevice::WriteOnly))
        {
            printf("[ftp get]open file failed!\n");
            return false;
        }
    }
    else
    {
        //文件已打开，检查是否可读
        if(!pFile->isReadable())
        {
            //关闭文件后，重新以只读方式打开
            pFile->close();
            if(!pFile->open(QIODevice::WriteOnly))
            {
                printf("[ftp get]open file failed!\n");
                return false;
            }
        }
    }
    //启动定时器
    m_ptimer->start();
    m_pFile = pFile;
    m_pRequest->setUrl(url);
    m_pReply = m_pmanager->get(*m_pRequest);
    connect(m_pReply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(dealError(QNetworkReply::NetworkError)));
    connect(m_pReply, SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(dealdownloadProcess(qint64,qint64)));
    return true;
}

/** @function
********************************************************************************
<PRE>
函数名: dealFinish
功能:  处理FTP操作结束工作
抛出异常: 否
--------------------------------------------------------------------------------
备注: ftpClient类槽函数
--------------------------------------------------------------------------------
</PRE>
*******************************************************************************/

void ftpClient::dealFinish(QNetworkReply *pReply)
{
    //停止定时器
    if(m_ptimer->isActive())
    {
        m_ptimer->stop();
    }
    //由于错误而发生的终止
    if(m_pReply->error() != QNetworkReply::NoError)
    {
        emit processCompleted(false);
        return;
    }
    //无需写入任何文件，不执行任何操作
    if(!m_pFile)
    {
        emit processCompleted(true);
        return;
    }
    //申请写入缓冲
    char *tempData = new char[MAX_WRITE_BYTES];
    if(!tempData)
    {
        printf("[ftp finish]write buffer alloc failed!\n");
        if(m_pFile->isOpen())
        {
           m_pFile->close();
           m_pFile = NULL;
        }
        emit processCompleted(false);
        return;
    }
    memset(tempData, 0, MAX_WRITE_BYTES);
    qint64 nReadBytes = 0;
    //循环写入文件，避免因为文件过大调用readAll带来的内存不足
    while(!pReply->atEnd())
    {
        nReadBytes = pReply->read(tempData, MAX_WRITE_BYTES);  //记录读取的字节数
        if(nReadBytes == 0)
        {
            continue;
        }
        if(nReadBytes != m_pFile->write(tempData ,nReadBytes))  //写入文件
        {
            printf("[ftp finish]write file:%s error: %s\n",
                   m_pFile->fileName().toLatin1().constData(), m_pFile->errorString().toLatin1().constData());
        }
    }
    delete[] tempData;
    if(m_pFile->isOpen())
    {
       m_pFile->close();
       m_pFile = NULL;
    }
    m_pReply->deleteLater();
    emit processCompleted(true);
    return;
}

/** @function
********************************************************************************
<PRE>
函数名: dealuploadProgress
功能:  处理上传进度
抛出异常: 否
--------------------------------------------------------------------------------
备注: ftpClient类槽函数
--------------------------------------------------------------------------------
</PRE>
*******************************************************************************/

void ftpClient::dealuploadProgress(qint64 bytesSent, qint64 bytesTotal)
{
    quint8 nPercent = (quint8)(bytesSent/(double)bytesTotal * 100); //计算百分比
    if(bytesSent != 0)
    {
        printf("Uploading...progress: %d%%...\n", nPercent);
        if(bytesSent == bytesTotal && m_pReply->error() == QNetworkReply::NoError)
        {
            //获取文件名
            QNetworkReply *pReply = (QNetworkReply*)sender();
            QString FileName = pReply->url().path();
            printf("File[%s]upload finished!\n", FileName.toLatin1().constData());
        }
    }
}

/** @function
********************************************************************************
<PRE>
函数名: dealdownloadProcess
功能:  处理下载进度
抛出异常: 否
--------------------------------------------------------------------------------
备注: ftpClient类槽函数
--------------------------------------------------------------------------------
</PRE>
*******************************************************************************/

void ftpClient::dealdownloadProcess(qint64 bytesReceived, qint64 bytesTotal)
{
    quint8 nPercent = (quint8)(bytesReceived/(double)bytesTotal * 100); //计算百分比
    if(bytesReceived != 0 && m_pReply->error() == QNetworkReply::NoError)
    {

        printf("Downloading... progress: %d%%...\n", nPercent);
        if(bytesReceived == bytesTotal)
        {
            //获取文件名
            QNetworkReply *pReply = (QNetworkReply*)sender();
            QString FileName = pReply->url().path();
            printf("File[%s]download finished!\n", FileName.toLatin1().constData());
        }
    }
}

/** @function
********************************************************************************
<PRE>
函数名: dealtimeOut
功能:  处理超时功能函数
抛出异常: 否
--------------------------------------------------------------------------------
备注: ftpClient类槽函数
--------------------------------------------------------------------------------
</PRE>
*******************************************************************************/

void ftpClient::dealtimeOut()
{
    printf("FTP operation time out,the operation will be canceled.\n");
    //停止定时器
    if(m_ptimer->isActive())
    {
       m_ptimer->stop();
    }
    //取消操作
    m_pReply->abort();
}

/** @function
********************************************************************************
<PRE>
函数名: dealError
功能:  ftp错误处理函数
抛出异常: 否
--------------------------------------------------------------------------------
备注: ftpClient类槽函数
--------------------------------------------------------------------------------
</PRE>
*******************************************************************************/

void ftpClient::dealError(QNetworkReply::NetworkError code)
{
    //停止定时器
    if(m_ptimer->isActive())
    {
       m_ptimer->stop();
    }
    //关闭打开的文件
    if(m_pFile->isOpen())
    {
        m_pFile->close();
        m_pFile = NULL;
    }
    printf("FTP operation failed! Error code: %d\n", code);
    m_pReply->deleteLater();
}


/***************************************************************
 *                          appUpdater                         *
 ***************************************************************/

//宏定义
#define MAIN_UPDATE_TYPE 1           //主程序升级类型
#define CARDREADER_UPDATE_TYPE 2     //读卡器程序升级类型

//静态成员初始化
const QString appUpdater::UPDATE_DIR = "/opt/kira/updata/";
const QString appUpdater::MAIN_FILE = "scanPos-Alipay";
const QString appUpdater::CARDREADER_FILE = "cardreader.bin";
const QString appUpdater::UPDATE_SUCCESS_FILE = UPDATE_DIR + "newUpdate.notify";

appUpdater::appUpdater() : m_bResult(false)
{
    m_pFtpClient = new ftpClient();
    if(!m_pFtpClient)
    {
        printf("[CRITIAL ERROR]system updater bad alloc!\n");
    }
    else
    {
        //连接信号，处理FTP操作完成步骤
        connect(m_pFtpClient, SIGNAL(processCompleted(bool)), this, SLOT(dealFtpOperateComplete(bool)));
    }
}

appUpdater::~appUpdater()
{
    if(m_pFtpClient)
    {
        delete m_pFtpClient;
    }
}

/** @function
********************************************************************************
<PRE>
函数名: processUpdate
功能:  执行升级操作外部接口
抛出异常: 否
--------------------------------------------------------------------------------
备注: appUpdater类成员函数
--------------------------------------------------------------------------------
</PRE>
*******************************************************************************/

enum appUpdaterError appUpdater::processUpdate(const posFtpInfo_t& ftpInfo, stmSerialTalk *pstmSerialTalk)
{
    return __processUpdate(ftpInfo, pstmSerialTalk);
}

/** @function
********************************************************************************
<PRE>
函数名: __processUpdate
功能:  执行升级操作内部接口
抛出异常: 否
--------------------------------------------------------------------------------
备注: appUpdater类成员函数
--------------------------------------------------------------------------------
</PRE>
*******************************************************************************/

enum appUpdaterError appUpdater::__processUpdate(const posFtpInfo_t& ftpInfo, stmSerialTalk *pstmSerialTalk)
{
    //检查参数正确性
    enum appUpdaterError ret = __checkFtpInfo(ftpInfo);
    if(noError != ret)
    {
        return ret;
    }

    qint8 nUpdateType = 0;   //升级类型
    //设置URL
    QUrl url;
    url.setScheme("ftp");
    url.setHost(ftpInfo.ftpIp);
    url.setUserName(ftpInfo.ftpUser);
    url.setPassword(ftpInfo.ftpPassword);
    url.setPort(ftpInfo.ftpPort.toInt());

    //生成本地下载文件路径与网络路径
    QString strlocalFile(UPDATE_DIR), strFtpFile(ftpInfo.ftpPath);
    //将ftp路径中的反斜杠换成斜杠
    strFtpFile.replace(QChar('\\'), QChar('/'));
    if(ftpInfo.version.toLower()[0] == QChar('p'))
    {
        //主程序开头为p
        strlocalFile += MAIN_FILE;
        //处理网络路径
        if(ftpInfo.ftpPath.right(1) != "/")
        {
            strFtpFile += "/";

        }
        strFtpFile += MAIN_FILE;
        nUpdateType = MAIN_UPDATE_TYPE;
    }
    else if(ftpInfo.version.toLower()[0] == QChar('s'))
    {
        //单片机程序开头为s
        strlocalFile += CARDREADER_FILE;
        //处理网络路径
        if(ftpInfo.ftpPath.right(1) != "/")
        {
            strFtpFile += "/";

        }
        strFtpFile += CARDREADER_FILE;
        nUpdateType = CARDREADER_UPDATE_TYPE;
    }
    else
    {
        printf("[system update]Unknown file type, the file version is: %s!\n",
               ftpInfo.version.toLatin1().constData());
        return filetypeError;
    }
    //设置远程路径到URL
    url.setPath(strFtpFile);

    //尝试打开本地文件
    QFile file(strlocalFile);
    if(!file.open(QIODevice::WriteOnly))
    {
        printf("[system update]Open local file: %s failed!\n", strlocalFile.toLatin1().constData());
        return localfileopenError;
    }
    //赋予最大权限
    if(!file.setPermissions((QFile::Permissions)0x7777))
    {
        printf("[system update]file %s chmod failed!\n", strlocalFile.toLatin1().constData());
        file.close();
        return localfilechmodError;
    }

    //提交下载申请
    if(!m_pFtpClient->get(url, &file))
    {
        printf("[system update]ftp download failed!\n");
        //删除文件
        __deleteFile(&file);
        return downloadError;
    }

    //本地事件循环，等待下载完成
    QEventLoop loop;
    connect(this, SIGNAL(quitloop()), &loop, SLOT(quit()));
    loop.exec();

    printf("[system update]ftp operation completed!\n");
    if(m_bResult)
    {
        if(__checkDownFileCRC(&file, ftpInfo.crc))
        {
            printf("[system update]Update file: %s download success!\n", strlocalFile.toLatin1().constData());
            ret = noError;
            //判断升级类型
            if(nUpdateType == MAIN_UPDATE_TYPE)
            {
                //工控机升级
                __processMainUpdate();
            }
            else if(nUpdateType == CARDREADER_UPDATE_TYPE)
            {
                //读卡器升级
                __processCardReaderUpdate(pstmSerialTalk);
            }
            else
            {
                ret = filetypeError;
            }
        }
        else
        {
            printf("[system update]Update file: %s CRC incorrect, deleting file...\n", strlocalFile.toLatin1().constData());
            //删除文件
            __deleteFile(&file);
            ret = macError;
        }
    }
    else
    {
        printf("[system update]Update file: %s download failed!\n", strlocalFile.toLatin1().constData());
        //删除文件
        __deleteFile(&file);
        ret = downloadError;
    }
    //关闭文件
    if(file.isOpen())
    {
        file.close();
    }
    return ret;
}

/** @function
********************************************************************************
<PRE>
函数名: __checkFtpInfo
功能:  检查FTP信息正确性
抛出异常: 否
--------------------------------------------------------------------------------
备注: appUpdater类成员函数
--------------------------------------------------------------------------------
</PRE>
*******************************************************************************/

enum appUpdaterError appUpdater::__checkFtpInfo(const posFtpInfo_t &ftpInfo)
{
    //检查ftp参数正确性
    if(ftpInfo.ftpIp.isNull())
    {
        printf("[system update]Ftp host is null!\n");
        return hostError;
    }
    if(ftpInfo.ftpUser.isNull())
    {
        printf("[system update]Ftp username is null!\n");
        return usernameError;
    }
    if(ftpInfo.ftpPassword.isNull())
    {
        printf("[system update]Ftp password is null!\n");
        return passwordError;
    }
    if(ftpInfo.ftpPath.isNull())
    {
        printf("[system update]Ftp path is null!\n");
        return filepathError;
    }
    if(ftpInfo.ftpPort.isNull())
    {
        printf("[system update]Ftp Port is null!\n");
        return portError;
    }
    if(ftpInfo.crc.size() > 8 || ftpInfo.crc.isNull())
    {
        printf("[system update]File CRC inCorrect!\n");
        return macError;
    }
    return noError;
}

/** @function
********************************************************************************
<PRE>
函数名: __checkDownFileCRC
功能:  CRC32校验
抛出异常: 否
--------------------------------------------------------------------------------
备注: appUpdater类成员函数
--------------------------------------------------------------------------------
</PRE>
*******************************************************************************/

bool appUpdater::__checkDownFileCRC(QFile *file, const QString &strCRC)
{
    //这里校验下载文件的CRC与服务器提供的CRC值是否匹配
    //计算CRC
    quint32 crc = 0xFFFFFFFF;   //指定CRC初值
    if(CRC32::calc_img_crc(file, crc, &crc) != 0)
    {
        //计算CRC出错
        return false;
    }
    else
    {
        //成功完成CRC校验，将数字转换成HEX字符串
        QString fileCRC = QString("%1").arg(crc, 8, 16, QChar('0'));
        printf("File CRC is: %s, Server report CRC is: %s\n",
               fileCRC.toLatin1().constData(), strCRC.toLatin1().constData());
        if(strCRC.compare(fileCRC, Qt::CaseInsensitive) != 0)
        {
            //CRC不匹配
            printf("CRC mismatching!\n");
            return false;
        }
    }
    return true;
}

/** @function
********************************************************************************
<PRE>
函数名: __deleteFile
功能:  删除对应QFile打开的磁盘文件
抛出异常: 否
--------------------------------------------------------------------------------
备注: appUpdater类成员函数
--------------------------------------------------------------------------------
</PRE>
*******************************************************************************/

void appUpdater::__deleteFile(QFile *file)
{
    //关闭文件
    if(file->isOpen())
    {
        file->close();
    }
    //删除文件
    if(unlink(file->fileName().toLatin1().constData()) != 0)
    {
        printf("[system update]delete file: %s error!\n", file->fileName().toLatin1().constData());
    }
}

/** @function
********************************************************************************
<PRE>
函数名: __processMainUpdate
功能:  处理主程序升级函数
抛出异常: 否
--------------------------------------------------------------------------------
备注: appUpdater类成员函数
--------------------------------------------------------------------------------
</PRE>
*******************************************************************************/

void appUpdater::__processMainUpdate()
{
    printf("update main application, rebooting system now...\n");
    //创建升级成功标志文件
    QFile file(UPDATE_SUCCESS_FILE);
    file.open(QIODevice::WriteOnly);
    file.close();
    //同步数据
    ::sync();
    //重启系统
    ::reboot(RB_AUTOBOOT);
}

/** @function
********************************************************************************
<PRE>
函数名: __processCardReaderUpdate
功能:  处理读卡器单片机程序升级函数
抛出异常: 否
--------------------------------------------------------------------------------
备注: appUpdater类成员函数
--------------------------------------------------------------------------------
</PRE>
*******************************************************************************/

void appUpdater::__processCardReaderUpdate(stmSerialTalk *pstmSerialTalk)
{
    printf("update cardreader, start stmUpdater thread...\n");
    if(!pstmSerialTalk->stmUpdater)
    {
        printf("stmUpdater not found, update failed!");
    }
    else
    {
        //发送重启指令
        char resetCommand[8] = { 0 };
        //读卡器准备下载指令 1:发送读卡器复位指令,延时1s,无返回值
        //命令实例：55 7A 21 01 00 00 00 0F
        resetCommand[0] = 0x55;
        resetCommand[1] = 0x7A; //HEAD
        resetCommand[2] = 0x21; //COMMAND
        resetCommand[3] = 0x01; //LEN
        resetCommand[7] = 0x0F; //VERIFY
        for(int i=0;i<5;i++)
        {
            pstmSerialTalk->writeData(resetCommand, 8);     //发送重启指令
            usleep(500*1000);
            if(pstmSerialTalk->stmUpdater->m_updateStm_restartOK)
            {
                break;
            }
        }
    }
}

/** @function
********************************************************************************
<PRE>
函数名: dealFtpOperateComplete
功能:  处理ftp操作完成动作，发送结束循环信号
抛出异常: 否
--------------------------------------------------------------------------------
备注: appUpdater类槽函数
--------------------------------------------------------------------------------
</PRE>
*******************************************************************************/

void appUpdater::dealFtpOperateComplete(bool result)
{
    m_bResult = result;
    emit quitloop();    //终止局部事件循环信号
}


/***************************************************************
 *                        stmUpdaterThread                     *
 ***************************************************************/
//相关宏定义
#define MAX_RETRY_TIMES 5     //升级失败最大重试次数

stmUpdaterThread::stmUpdaterThread(int *fd) :
    stmfd(fd)
{
    //复位标志
    m_updateStm_restartOK = false;
    m_updateStm_deleteOK = false;
    m_updateStm_intoBootOK = false;
    m_updateStm_update_ok = false;
    m_updateStm_transferReady = true;
}

stmUpdaterThread::~stmUpdaterThread()
{

}

/** @function
********************************************************************************
<PRE>
函数名: run
功能:  线程函数
抛出异常: 否
--------------------------------------------------------------------------------
备注: stmUpdaterThread类成员函数
--------------------------------------------------------------------------------
</PRE>
*******************************************************************************/

void stmUpdaterThread::run()
{
    qDebug("Begin update stmApp...");
    int nFlag = 0;          //升级状态标志
    int nRetryTimes = 0;    //重试次数
    m_updateStmStatus = 0;  //复位状态机
    m_updateStm_restartOK = false; //复位标志
    m_updateStm_deleteOK = false;
    m_updateStm_intoBootOK = false;
    m_updateStm_update_ok = false;
    m_updateStm_transferReady = true;

    //检测文件是否存在
    QString strBinFile = appUpdater::UPDATE_DIR + appUpdater::CARDREADER_FILE;
    QFile file(strBinFile);
    if(!file.exists())
    {
        qDebug("update file doesn't exists!\n");
        emit stmUpdateFail();
        return;
    }

    while(nFlag <= 0 && nRetryTimes < MAX_RETRY_TIMES)
    {
        //如果升级状态返回为0则一直执行，直至升级完成
        nFlag = updateStmApp(nRetryTimes);
        if(nFlag < 0)
        {
            qDebug("updateStmApp failed, return code: %d, retry!", nFlag);
            ++nRetryTimes;
        }
    }
    //判断成功或者失败次数
    if(nFlag > 0)
    {
       qDebug("Update stmApp finished,wait stm restart...");
    }
    else
    {
        if(nRetryTimes >= MAX_RETRY_TIMES)
        {
            qDebug("Update stmApp failed %d times.", nRetryTimes);
            qDebug("Failed excueed max retry times! Give up update!");
        }
        emit stmUpdateFail();
    }
}

/** @function
********************************************************************************
<PRE>
函数名: updateStmApp
功能:  单片机程序升级函数
抛出异常: 否
--------------------------------------------------------------------------------
备注: stmUpdaterThread类成员函数
--------------------------------------------------------------------------------
</PRE>
*******************************************************************************/

int stmUpdaterThread::updateStmApp(int &nRetryTimes)
{
    qDebug("stm_update_state:%d;",m_updateStmStatus);

        int i;
        switch(m_updateStmStatus)
        {
            case 0:
                sleep(1);
                ++nRetryTimes;    //增加重试次数
                m_updateStm_intoBootOK = false;
                for(i=0;i<10;i++)
                {
                    //2:发送检测读卡器是否进入BOOT状态指令延时100ms
                    //send:00 00 aa 02 19 1b cc
                    //answer:00 00 bb 19 00 cc aa 03 19 00 1a cc
                    sendStmBoot(0x19);
                    usleep(200*1000);
                    if(m_updateStm_intoBootOK)
                    {
                        break;
                    }
                }
                if(!m_updateStm_intoBootOK)
                {
                    return -19;
                }
                //进入下一个指令,发送删除程序指令
                m_updateStmStatus=1;
                break;
            case 1:
                sleep(2);
                m_updateStm_deleteOK = false;
                for(i=0;i<10;i++)
                {
                    //3:发送删除读卡器应用程序指令延时
                    // 4in1一分钟收不到这条指令则会放弃升级
                    sendStmBoot(0x17);
                    usleep(500*1000);
                    if(m_updateStm_deleteOK)
                    {
                        break;
                    }
                }
                if(!m_updateStm_deleteOK)
                {
                    return -17;
                }
                //进入下一个指令,发送程序数据
                m_updateStmStatus=2;
                break;
            case 2:
                //4:发送程序文件18指令等待时间
                i = sendStmBinData();
                if( i < 0)
                {
                    return -18;
                }
                else if(i == 0)
                {
                    m_updateStmStatus = 0;
                    break;
                }
                else
                {
                    m_updateStmStatus = 3;
                    qDebug("StmUpdate: binary BIN file transfered successfully!\n");
                }
                break;
            case 3:
                m_updateStm_update_ok = false;
                for(i=0;i<3;i++)
                {
                    // 5:读卡器完成下载应答16指令等待时间
                    //命令：		00 00 AA 02 16 18 CC
                    //正确返回：	00 00 bb 16 00 cc aa 03 16 00 15 cc
                    sendStmBoot(0x16);
                    sleep(1);
                    if(m_updateStm_update_ok)
                    {
                        break;
                    }
                }
                if(!m_updateStm_update_ok)
                {
                    return -16;
                }
                return 1;//成功
                break;
        }
        return 0;
}

/** @function
********************************************************************************
<PRE>
函数名: sendStmBinData
功能:  发送单片机升级程序文件
抛出异常: 否
--------------------------------------------------------------------------------
备注: stmUpdaterThread类成员函数
--------------------------------------------------------------------------------
</PRE>
*******************************************************************************/

int stmUpdaterThread::sendStmBinData()
{
    quint8 packet[137] = { 0 };    //发送包缓冲区
    int ret_flag = -1, nSendCount = 0;  //返回值,发送计数
    volatile int nReadByte = 0;  //文件读取块数
    quint16 nPackOrder = 0;      //包序号

    //写包头包尾固定部分
    packet[0] = 0x00;
    packet[1] = 0x00;
    packet[2] = 0xAA;
    packet[3] = 0x84;
    packet[4] = 0x18;
    packet[136] = 0xCC;

    QString strBinFile = appUpdater::UPDATE_DIR + appUpdater::CARDREADER_FILE;
    QFile file(strBinFile);
    if(!file.open(QIODevice::ReadOnly))
    {
        qDebug("open Stm update file error!\n");
        return ret_flag;
    }

    //复位应答
    stmUpdateMutex.lock();
    m_updateStm_updateAnswer = 0;
    stmUpdateMutex.unlock();

    qDebug("Start transfer stm bin file, total size: %lld", file.size());
    nReadByte = file.read((char*)&packet[7], 128);   //每次读取128个字节到发送包缓冲区
    while(nReadByte > 0)
    {
        ++nSendCount;
        //将包序号转换成网络字节序
        quint16 netPackOrder = htons(nPackOrder);
        //写入包的序号序号
        memcpy(&packet[5], &netPackOrder, 2);
        //包序号加1
        ++nPackOrder;
        //计算校验
        quint8 verify = cal_xor(&packet[3], 132);
        packet[135] = verify;
        writeData((char*)packet, 137, false);   //不显示日志
        //等待应答
        stmUpdateMutex.lock();
        if(!stmUpdateCond.wait(&stmUpdateMutex, 1000 * 2))  //等待超时2秒
        {
            //等到回应包超时,回到0x19
            stmUpdateMutex.unlock();
            file.close();
            return 0;
        }
        stmUpdateMutex.unlock();
        nReadByte = file.read((char*)&packet[7], 128);
    }
    file.atEnd() ? ret_flag = 1 : ret_flag = 0;
    qDebug("Send Stm update bin data finished, ret_flag is %d\n",ret_flag);
    if(ret_flag)//判断是不是已经到了文件尾部
    {
        QMutexLocker locker(&stmUpdateMutex);
        qDebug("Total send count: [%d], total receive count: [%d]\n",
               nSendCount, m_updateStm_updateAnswer);
        ret_flag = 1;
    }
    else//文件读取发生错误
    {
        QMutexLocker locker(&stmUpdateMutex);
        qDebug("error occured!! Total send count:[%d]包!, total receive count: [%d]\n",
               nSendCount,m_updateStm_updateAnswer);
        ret_flag = -1;
    }
    file.close();
    sleep(1);

    QMutexLocker locker(&stmUpdateMutex);
    if(	m_updateStm_updateAnswer < nSendCount ) //有丢包发生
    {
        qDebug("Bin file transfer pack loss found, send[%d]->reply[%d]\n",
               nSendCount,m_updateStm_updateAnswer);
        ret_flag = 0;
    }
    return ret_flag;
}

/** @function
********************************************************************************
<PRE>
函数名: sendStmBoot
功能: 发送指定命令字到单片机
抛出异常: 否
--------------------------------------------------------------------------------
备注: stmUpdaterThread类成员函数
--------------------------------------------------------------------------------
</PRE>
*******************************************************************************/

void stmUpdaterThread::sendStmBoot(quint8 cmd)
{
    quint8 sendBuf[7]= { 0 };
    quint8 index=0;
    qDebug("[stmUpdaterThread] send command 0x%02X to Stm.", cmd);

    index=0;
    ///////////////////
    sendBuf[index++] = 0x00;    //CAN_SOURCEID
    sendBuf[index++] = 0x00;    //CAN_DESTID
    sendBuf[index++] = 0xAA;
    sendBuf[index++] = 0x02;
    /*LEN: COMMAND+DATA+VERIFY(一个字节)*/
    sendBuf[index++] = cmd;
    /*VERIFY= LEN XOR COMMAND XOR DATA*/
    sendBuf[index++] = cal_xor(&sendBuf[3],2);
    sendBuf[index++] = 0xCC;

    writeData((char*)sendBuf, index);
}

/** @function
********************************************************************************
<PRE>
函数名: cal_xor
功能:  计算校验和
抛出异常: 否
--------------------------------------------------------------------------------
备注: stmUpdaterThread类成员函数
--------------------------------------------------------------------------------
</PRE>
*******************************************************************************/

quint8 stmUpdaterThread::cal_xor(const quint8 *data, int count)
{
    quint8 verify = 0;
    for(int i=0;i<count;i++)
    {
        verify ^= data[i];
    }
    return verify;
}

/** @function
********************************************************************************
<PRE>
函数名: writeData
功能: 写串口数据
抛出异常: 否
--------------------------------------------------------------------------------
备注:stmUpdaterThread类成员函数
--------------------------------------------------------------------------------
</PRE>
*******************************************************************************/
void stmUpdaterThread::writeData(char *data, int datalen, bool isDisplay)
{
    int wlen = write(*stmfd, data, datalen);
    if(isDisplay)
    {
        printf("Send %d bytes data to stm:", wlen);
        for(int i = 0; i != datalen; i++)
        {
            printf(" %02X", data[i]);
        }
        printf("\n");
    }
}
