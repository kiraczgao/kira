#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <sys/reboot.h>
#include <QFile>
#include <QByteArray>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QTimer>
#include <QEventLoop>
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

//FTP上传文件-重载1
//data 需要上传的数据
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

//FTP上传文件-重载2
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

//FTP下载文件
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

//槽定义
//FTP文件下载完成
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

//上传进度条
void ftpClient::dealuploadProgress(qint64 bytesSent, qint64 bytesTotal)
{
    quint8 nPercent = (quint8)(bytesSent/(double)bytesTotal * 100); //计算百分比
    if(bytesSent != 0)
    {
        printf("Uploading...progress: %d%%\n", nPercent);
        if(bytesSent == bytesTotal && m_pReply->error() == QNetworkReply::NoError)
        {
            //获取文件名
            QNetworkReply *pReply = (QNetworkReply*)sender();
            QString FileName = pReply->url().path();
            printf("File[%s]upload finished!\n", FileName.toLatin1().constData());
        }
    }
}

//下载进度条
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

//超时处理
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

//出错处理
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


/*********************
 *     appUpdater    *
 *********************/
const QString appUpdater::UPDATE_DIR = "/opt/kira/updata/";
const QString appUpdater::MAIN_FILE = "scanPos-Alipay";
const QString appUpdater::CARDREADER_FILE = "cardReader";
const QString appUpdater::UPDATE_SUCCESS_FILE = UPDATE_DIR + "newUpdate.notify";

//软件升级执行外部函数
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

enum appUpdaterError appUpdater::processUpdate(const posFtpInfo_t& ftpInfo)
{
    return __processUpdate(ftpInfo);
}

//软件升级执行内部函数
enum appUpdaterError appUpdater::__processUpdate(const posFtpInfo_t& ftpInfo)
{
    //检查参数正确性
    enum appUpdaterError ret = __checkFtpInfo(ftpInfo);
    if(noError != ret)
    {
        return ret;
    }

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
            printf("[system update]Update file: %s download success! rebooting system...\n", strlocalFile.toLatin1().constData());
            ret = noError;
            //重新启动
            __rebootSystem();
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

//校验FTP信息是否正确
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

//校验下载文件CRC是否正确
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

//删除文件
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

void appUpdater::__rebootSystem()
{
    //创建升级成功标志文件
    QFile file(UPDATE_SUCCESS_FILE);
    file.open(QIODevice::WriteOnly);
    file.close();
    //同步数据
    ::sync();
    //重启系统
    ::reboot(RB_AUTOBOOT);
}

//FTP完成响应函数
void appUpdater::dealFtpOperateComplete(bool result)
{
    m_bResult = result;
    emit quitloop();    //终止局部事件循环信号
}
