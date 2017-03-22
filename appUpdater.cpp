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

//��̬��Ա��ʼ��
quint32 ftpClient::MAX_WRITE_BYTES = 1024 * 1024;   //ÿ�����д��1MB
quint32 ftpClient::MAX_TIMEOUT = 1000 * 60 * 3;     //������ʱ3����

ftpClient::ftpClient() : m_pReply(NULL)
{
    //�����ڴ�
    m_pmanager = new QNetworkAccessManager(this);
    m_pRequest = new QNetworkRequest();
    m_ptimer = new QTimer();
    if(!m_pmanager || !m_pRequest || !m_ptimer)
    {
        printf("[CRITIAL ERROR]ftpClient bad alloc!!\n");
    }
    else
    {
        //�����ź����
        connect(m_pmanager,SIGNAL(finished(QNetworkReply*)),this,SLOT(dealFinish(QNetworkReply*)));
        connect(m_ptimer, SIGNAL(timeout()), this, SLOT(dealtimeOut()));
        //�趨��ʱ����ʱʱ��
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

//FTP�ϴ��ļ�-����1
//data ��Ҫ�ϴ�������
bool ftpClient::put(const QUrl &url, const QByteArray &data)
{
    //��Ч���ж�
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
    //������ʱ��
    m_ptimer->start();
    m_pRequest->setUrl(url);
    m_pReply = m_pmanager->put(*m_pRequest, data);
    connect(m_pReply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(dealError(QNetworkReply::NetworkError)));
    connect(m_pReply, SIGNAL(uploadProgress(qint64,qint64)), this, SLOT(dealuploadProgress(qint64,qint64)));
    return true;
}

//FTP�ϴ��ļ�-����2
bool ftpClient::put(const QUrl &url, QFile *pFile)
{
    //��Ч���ж�
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
    //����ļ��Ϸ���
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
    //������ʱ��
    m_ptimer->start();
    m_pRequest->setUrl(url);
    m_pReply = m_pmanager->put(*m_pRequest, pFile);
    connect(m_pReply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(dealError(QNetworkReply::NetworkError)));
    connect(m_pReply, SIGNAL(uploadProgress(qint64,qint64)), this, SLOT(dealuploadProgress(qint64,qint64)));
    return true;
}

//FTP�����ļ�
bool ftpClient::get(const QUrl &url, QFile *pFile)
{
    //��Ч���ж�
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
    //���ļ�
    if(!pFile->isOpen())
    {
        //�����Է�ʽ���ļ�
        if(!pFile->open(QIODevice::WriteOnly))
        {
            printf("[ftp get]open file failed!\n");
            return false;
        }
    }
    else
    {
        //�ļ��Ѵ򿪣�����Ƿ�ɶ�
        if(!pFile->isReadable())
        {
            //�ر��ļ���������ֻ����ʽ��
            pFile->close();
            if(!pFile->open(QIODevice::WriteOnly))
            {
                printf("[ftp get]open file failed!\n");
                return false;
            }
        }
    }
    //������ʱ��
    m_ptimer->start();
    m_pFile = pFile;
    m_pRequest->setUrl(url);
    m_pReply = m_pmanager->get(*m_pRequest);
    connect(m_pReply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(dealError(QNetworkReply::NetworkError)));
    connect(m_pReply, SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(dealdownloadProcess(qint64,qint64)));
    return true;
}

//�۶���
//FTP�ļ��������
void ftpClient::dealFinish(QNetworkReply *pReply)
{
    //ֹͣ��ʱ��
    if(m_ptimer->isActive())
    {
        m_ptimer->stop();
    }
    //���ڴ������������ֹ
    if(m_pReply->error() != QNetworkReply::NoError)
    {
        emit processCompleted(false);
        return;
    }
    //����д���κ��ļ�����ִ���κβ���
    if(!m_pFile)
    {
        emit processCompleted(true);
        return;
    }
    //����д�뻺��
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
    //ѭ��д���ļ���������Ϊ�ļ��������readAll�������ڴ治��
    while(!pReply->atEnd())
    {
        nReadBytes = pReply->read(tempData, MAX_WRITE_BYTES);  //��¼��ȡ���ֽ���
        if(nReadBytes == 0)
        {
            continue;
        }
        if(nReadBytes != m_pFile->write(tempData ,nReadBytes))  //д���ļ�
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

//�ϴ�������
void ftpClient::dealuploadProgress(qint64 bytesSent, qint64 bytesTotal)
{
    quint8 nPercent = (quint8)(bytesSent/(double)bytesTotal * 100); //����ٷֱ�
    if(bytesSent != 0)
    {
        printf("Uploading...progress: %d%%\n", nPercent);
        if(bytesSent == bytesTotal && m_pReply->error() == QNetworkReply::NoError)
        {
            //��ȡ�ļ���
            QNetworkReply *pReply = (QNetworkReply*)sender();
            QString FileName = pReply->url().path();
            printf("File[%s]upload finished!\n", FileName.toLatin1().constData());
        }
    }
}

//���ؽ�����
void ftpClient::dealdownloadProcess(qint64 bytesReceived, qint64 bytesTotal)
{
    quint8 nPercent = (quint8)(bytesReceived/(double)bytesTotal * 100); //����ٷֱ�
    if(bytesReceived != 0 && m_pReply->error() == QNetworkReply::NoError)
    {

        printf("Downloading... progress: %d%%...\n", nPercent);
        if(bytesReceived == bytesTotal)
        {
            //��ȡ�ļ���
            QNetworkReply *pReply = (QNetworkReply*)sender();
            QString FileName = pReply->url().path();
            printf("File[%s]download finished!\n", FileName.toLatin1().constData());
        }
    }
}

//��ʱ����
void ftpClient::dealtimeOut()
{
    printf("FTP operation time out,the operation will be canceled.\n");
    //ֹͣ��ʱ��
    if(m_ptimer->isActive())
    {
       m_ptimer->stop();
    }
    //ȡ������
    m_pReply->abort();
}

//������
void ftpClient::dealError(QNetworkReply::NetworkError code)
{
    //ֹͣ��ʱ��
    if(m_ptimer->isActive())
    {
       m_ptimer->stop();
    }
    //�رմ򿪵��ļ�
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

//�������ִ���ⲿ����
appUpdater::appUpdater() : m_bResult(false)
{
    m_pFtpClient = new ftpClient();
    if(!m_pFtpClient)
    {
        printf("[CRITIAL ERROR]system updater bad alloc!\n");
    }
    else
    {
        //�����źţ�����FTP������ɲ���
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

//�������ִ���ڲ�����
enum appUpdaterError appUpdater::__processUpdate(const posFtpInfo_t& ftpInfo)
{
    //��������ȷ��
    enum appUpdaterError ret = __checkFtpInfo(ftpInfo);
    if(noError != ret)
    {
        return ret;
    }

    //����URL
    QUrl url;
    url.setScheme("ftp");
    url.setHost(ftpInfo.ftpIp);
    url.setUserName(ftpInfo.ftpUser);
    url.setPassword(ftpInfo.ftpPassword);
    url.setPort(ftpInfo.ftpPort.toInt());

    //���ɱ��������ļ�·��������·��
    QString strlocalFile(UPDATE_DIR), strFtpFile(ftpInfo.ftpPath);
    //��ftp·���еķ�б�ܻ���б��
    strFtpFile.replace(QChar('\\'), QChar('/'));
    if(ftpInfo.version.toLower()[0] == QChar('p'))
    {
        //������ͷΪp
        strlocalFile += MAIN_FILE;
        //��������·��
        if(ftpInfo.ftpPath.right(1) != "/")
        {
            strFtpFile += "/";

        }
        strFtpFile += MAIN_FILE;
    }
    else if(ftpInfo.version.toLower()[0] == QChar('s'))
    {
        //��Ƭ������ͷΪs
        strlocalFile += CARDREADER_FILE;
        //��������·��
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
    //����Զ��·����URL
    url.setPath(strFtpFile);

    //���Դ򿪱����ļ�
    QFile file(strlocalFile);
    if(!file.open(QIODevice::WriteOnly))
    {
        printf("[system update]Open local file: %s failed!\n", strlocalFile.toLatin1().constData());
        return localfileopenError;
    }
    //�������Ȩ��
    if(!file.setPermissions((QFile::Permissions)0x7777))
    {
        printf("[system update]file %s chmod failed!\n", strlocalFile.toLatin1().constData());
        file.close();
        return localfilechmodError;
    }

    //�ύ��������
    if(!m_pFtpClient->get(url, &file))
    {
        printf("[system update]ftp download failed!\n");
        //ɾ���ļ�
        __deleteFile(&file);
        return downloadError;
    }

    //�����¼�ѭ�����ȴ��������
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
            //��������
            __rebootSystem();
        }
        else
        {
            printf("[system update]Update file: %s CRC incorrect, deleting file...\n", strlocalFile.toLatin1().constData());
            //ɾ���ļ�
            __deleteFile(&file);
            ret = macError;
        }
    }
    else
    {
        printf("[system update]Update file: %s download failed!\n", strlocalFile.toLatin1().constData());
        //ɾ���ļ�
        __deleteFile(&file);
        ret = downloadError;
    }
    //�ر��ļ�
    if(file.isOpen())
    {
        file.close();
    }
    return ret;
}

//У��FTP��Ϣ�Ƿ���ȷ
enum appUpdaterError appUpdater::__checkFtpInfo(const posFtpInfo_t &ftpInfo)
{
    //���ftp������ȷ��
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

//У�������ļ�CRC�Ƿ���ȷ
bool appUpdater::__checkDownFileCRC(QFile *file, const QString &strCRC)
{
    //����У�������ļ���CRC��������ṩ��CRCֵ�Ƿ�ƥ��
    //����CRC
    quint32 crc = 0xFFFFFFFF;   //ָ��CRC��ֵ
    if(CRC32::calc_img_crc(file, crc, &crc) != 0)
    {
        //����CRC����
        return false;
    }
    else
    {
        //�ɹ����CRCУ�飬������ת����HEX�ַ���
        QString fileCRC = QString("%1").arg(crc, 8, 16, QChar('0'));
        printf("File CRC is: %s, Server report CRC is: %s\n",
               fileCRC.toLatin1().constData(), strCRC.toLatin1().constData());
        if(strCRC.compare(fileCRC, Qt::CaseInsensitive) != 0)
        {
            //CRC��ƥ��
            printf("CRC mismatching!\n");
            return false;
        }
    }
    return true;
}

//ɾ���ļ�
void appUpdater::__deleteFile(QFile *file)
{
    //�ر��ļ�
    if(file->isOpen())
    {
        file->close();
    }
    //ɾ���ļ�
    if(unlink(file->fileName().toLatin1().constData()) != 0)
    {
        printf("[system update]delete file: %s error!\n", file->fileName().toLatin1().constData());
    }
}

void appUpdater::__rebootSystem()
{
    //���������ɹ���־�ļ�
    QFile file(UPDATE_SUCCESS_FILE);
    file.open(QIODevice::WriteOnly);
    file.close();
    //ͬ������
    ::sync();
    //����ϵͳ
    ::reboot(RB_AUTOBOOT);
}

//FTP�����Ӧ����
void appUpdater::dealFtpOperateComplete(bool result)
{
    m_bResult = result;
    emit quitloop();    //��ֹ�ֲ��¼�ѭ���ź�
}
