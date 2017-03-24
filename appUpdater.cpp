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

/** @function
********************************************************************************
<PRE>
������: put
����:  �ϴ����ܺ���-����1
�׳��쳣: ��
--------------------------------------------------------------------------------
��ע: ftpClient���Ա����
--------------------------------------------------------------------------------
</PRE>
*******************************************************************************/

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

/** @function
********************************************************************************
<PRE>
������: put
����:  �ϴ����ܺ���-����2
�׳��쳣: ��
--------------------------------------------------------------------------------
��ע: ftpClient���Ա����
--------------------------------------------------------------------------------
</PRE>
*******************************************************************************/

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

/** @function
********************************************************************************
<PRE>
������: get
����:  ����FTP�ļ����ܺ���
�׳��쳣: ��
--------------------------------------------------------------------------------
��ע: ftpClient���Ա����
--------------------------------------------------------------------------------
</PRE>
*******************************************************************************/

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

/** @function
********************************************************************************
<PRE>
������: dealFinish
����:  ����FTP������������
�׳��쳣: ��
--------------------------------------------------------------------------------
��ע: ftpClient��ۺ���
--------------------------------------------------------------------------------
</PRE>
*******************************************************************************/

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

/** @function
********************************************************************************
<PRE>
������: dealuploadProgress
����:  �����ϴ�����
�׳��쳣: ��
--------------------------------------------------------------------------------
��ע: ftpClient��ۺ���
--------------------------------------------------------------------------------
</PRE>
*******************************************************************************/

void ftpClient::dealuploadProgress(qint64 bytesSent, qint64 bytesTotal)
{
    quint8 nPercent = (quint8)(bytesSent/(double)bytesTotal * 100); //����ٷֱ�
    if(bytesSent != 0)
    {
        printf("Uploading...progress: %d%%...\n", nPercent);
        if(bytesSent == bytesTotal && m_pReply->error() == QNetworkReply::NoError)
        {
            //��ȡ�ļ���
            QNetworkReply *pReply = (QNetworkReply*)sender();
            QString FileName = pReply->url().path();
            printf("File[%s]upload finished!\n", FileName.toLatin1().constData());
        }
    }
}

/** @function
********************************************************************************
<PRE>
������: dealdownloadProcess
����:  �������ؽ���
�׳��쳣: ��
--------------------------------------------------------------------------------
��ע: ftpClient��ۺ���
--------------------------------------------------------------------------------
</PRE>
*******************************************************************************/

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

/** @function
********************************************************************************
<PRE>
������: dealtimeOut
����:  ����ʱ���ܺ���
�׳��쳣: ��
--------------------------------------------------------------------------------
��ע: ftpClient��ۺ���
--------------------------------------------------------------------------------
</PRE>
*******************************************************************************/

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

/** @function
********************************************************************************
<PRE>
������: dealError
����:  ftp��������
�׳��쳣: ��
--------------------------------------------------------------------------------
��ע: ftpClient��ۺ���
--------------------------------------------------------------------------------
</PRE>
*******************************************************************************/

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


/***************************************************************
 *                          appUpdater                         *
 ***************************************************************/

//�궨��
#define MAIN_UPDATE_TYPE 1           //��������������
#define CARDREADER_UPDATE_TYPE 2     //������������������

//��̬��Ա��ʼ��
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

/** @function
********************************************************************************
<PRE>
������: processUpdate
����:  ִ�����������ⲿ�ӿ�
�׳��쳣: ��
--------------------------------------------------------------------------------
��ע: appUpdater���Ա����
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
������: __processUpdate
����:  ִ�����������ڲ��ӿ�
�׳��쳣: ��
--------------------------------------------------------------------------------
��ע: appUpdater���Ա����
--------------------------------------------------------------------------------
</PRE>
*******************************************************************************/

enum appUpdaterError appUpdater::__processUpdate(const posFtpInfo_t& ftpInfo, stmSerialTalk *pstmSerialTalk)
{
    //��������ȷ��
    enum appUpdaterError ret = __checkFtpInfo(ftpInfo);
    if(noError != ret)
    {
        return ret;
    }

    qint8 nUpdateType = 0;   //��������
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
        nUpdateType = MAIN_UPDATE_TYPE;
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
        nUpdateType = CARDREADER_UPDATE_TYPE;
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
            printf("[system update]Update file: %s download success!\n", strlocalFile.toLatin1().constData());
            ret = noError;
            //�ж���������
            if(nUpdateType == MAIN_UPDATE_TYPE)
            {
                //���ػ�����
                __processMainUpdate();
            }
            else if(nUpdateType == CARDREADER_UPDATE_TYPE)
            {
                //����������
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

/** @function
********************************************************************************
<PRE>
������: __checkFtpInfo
����:  ���FTP��Ϣ��ȷ��
�׳��쳣: ��
--------------------------------------------------------------------------------
��ע: appUpdater���Ա����
--------------------------------------------------------------------------------
</PRE>
*******************************************************************************/

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

/** @function
********************************************************************************
<PRE>
������: __checkDownFileCRC
����:  CRC32У��
�׳��쳣: ��
--------------------------------------------------------------------------------
��ע: appUpdater���Ա����
--------------------------------------------------------------------------------
</PRE>
*******************************************************************************/

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

/** @function
********************************************************************************
<PRE>
������: __deleteFile
����:  ɾ����ӦQFile�򿪵Ĵ����ļ�
�׳��쳣: ��
--------------------------------------------------------------------------------
��ע: appUpdater���Ա����
--------------------------------------------------------------------------------
</PRE>
*******************************************************************************/

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

/** @function
********************************************************************************
<PRE>
������: __processMainUpdate
����:  ������������������
�׳��쳣: ��
--------------------------------------------------------------------------------
��ע: appUpdater���Ա����
--------------------------------------------------------------------------------
</PRE>
*******************************************************************************/

void appUpdater::__processMainUpdate()
{
    printf("update main application, rebooting system now...\n");
    //���������ɹ���־�ļ�
    QFile file(UPDATE_SUCCESS_FILE);
    file.open(QIODevice::WriteOnly);
    file.close();
    //ͬ������
    ::sync();
    //����ϵͳ
    ::reboot(RB_AUTOBOOT);
}

/** @function
********************************************************************************
<PRE>
������: __processCardReaderUpdate
����:  �����������Ƭ��������������
�׳��쳣: ��
--------------------------------------------------------------------------------
��ע: appUpdater���Ա����
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
        //��������ָ��
        char resetCommand[8] = { 0 };
        //������׼������ָ�� 1:���Ͷ�������λָ��,��ʱ1s,�޷���ֵ
        //����ʵ����55 7A 21 01 00 00 00 0F
        resetCommand[0] = 0x55;
        resetCommand[1] = 0x7A; //HEAD
        resetCommand[2] = 0x21; //COMMAND
        resetCommand[3] = 0x01; //LEN
        resetCommand[7] = 0x0F; //VERIFY
        for(int i=0;i<5;i++)
        {
            pstmSerialTalk->writeData(resetCommand, 8);     //��������ָ��
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
������: dealFtpOperateComplete
����:  ����ftp������ɶ��������ͽ���ѭ���ź�
�׳��쳣: ��
--------------------------------------------------------------------------------
��ע: appUpdater��ۺ���
--------------------------------------------------------------------------------
</PRE>
*******************************************************************************/

void appUpdater::dealFtpOperateComplete(bool result)
{
    m_bResult = result;
    emit quitloop();    //��ֹ�ֲ��¼�ѭ���ź�
}


/***************************************************************
 *                        stmUpdaterThread                     *
 ***************************************************************/
//��غ궨��
#define MAX_RETRY_TIMES 5     //����ʧ��������Դ���

stmUpdaterThread::stmUpdaterThread(int *fd) :
    stmfd(fd)
{
    //��λ��־
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
������: run
����:  �̺߳���
�׳��쳣: ��
--------------------------------------------------------------------------------
��ע: stmUpdaterThread���Ա����
--------------------------------------------------------------------------------
</PRE>
*******************************************************************************/

void stmUpdaterThread::run()
{
    qDebug("Begin update stmApp...");
    int nFlag = 0;          //����״̬��־
    int nRetryTimes = 0;    //���Դ���
    m_updateStmStatus = 0;  //��λ״̬��
    m_updateStm_restartOK = false; //��λ��־
    m_updateStm_deleteOK = false;
    m_updateStm_intoBootOK = false;
    m_updateStm_update_ok = false;
    m_updateStm_transferReady = true;

    //����ļ��Ƿ����
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
        //�������״̬����Ϊ0��һֱִ�У�ֱ���������
        nFlag = updateStmApp(nRetryTimes);
        if(nFlag < 0)
        {
            qDebug("updateStmApp failed, return code: %d, retry!", nFlag);
            ++nRetryTimes;
        }
    }
    //�жϳɹ�����ʧ�ܴ���
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
������: updateStmApp
����:  ��Ƭ��������������
�׳��쳣: ��
--------------------------------------------------------------------------------
��ע: stmUpdaterThread���Ա����
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
                ++nRetryTimes;    //�������Դ���
                m_updateStm_intoBootOK = false;
                for(i=0;i<10;i++)
                {
                    //2:���ͼ��������Ƿ����BOOT״ָ̬����ʱ100ms
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
                //������һ��ָ��,����ɾ������ָ��
                m_updateStmStatus=1;
                break;
            case 1:
                sleep(2);
                m_updateStm_deleteOK = false;
                for(i=0;i<10;i++)
                {
                    //3:����ɾ��������Ӧ�ó���ָ����ʱ
                    // 4in1һ�����ղ�������ָ������������
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
                //������һ��ָ��,���ͳ�������
                m_updateStmStatus=2;
                break;
            case 2:
                //4:���ͳ����ļ�18ָ��ȴ�ʱ��
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
                    // 5:�������������Ӧ��16ָ��ȴ�ʱ��
                    //���		00 00 AA 02 16 18 CC
                    //��ȷ���أ�	00 00 bb 16 00 cc aa 03 16 00 15 cc
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
                return 1;//�ɹ�
                break;
        }
        return 0;
}

/** @function
********************************************************************************
<PRE>
������: sendStmBinData
����:  ���͵�Ƭ�����������ļ�
�׳��쳣: ��
--------------------------------------------------------------------------------
��ע: stmUpdaterThread���Ա����
--------------------------------------------------------------------------------
</PRE>
*******************************************************************************/

int stmUpdaterThread::sendStmBinData()
{
    quint8 packet[137] = { 0 };    //���Ͱ�������
    int ret_flag = -1, nSendCount = 0;  //����ֵ,���ͼ���
    volatile int nReadByte = 0;  //�ļ���ȡ����
    quint16 nPackOrder = 0;      //�����

    //д��ͷ��β�̶�����
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

    //��λӦ��
    stmUpdateMutex.lock();
    m_updateStm_updateAnswer = 0;
    stmUpdateMutex.unlock();

    qDebug("Start transfer stm bin file, total size: %lld", file.size());
    nReadByte = file.read((char*)&packet[7], 128);   //ÿ�ζ�ȡ128���ֽڵ����Ͱ�������
    while(nReadByte > 0)
    {
        ++nSendCount;
        //�������ת���������ֽ���
        quint16 netPackOrder = htons(nPackOrder);
        //д�����������
        memcpy(&packet[5], &netPackOrder, 2);
        //����ż�1
        ++nPackOrder;
        //����У��
        quint8 verify = cal_xor(&packet[3], 132);
        packet[135] = verify;
        writeData((char*)packet, 137, false);   //����ʾ��־
        //�ȴ�Ӧ��
        stmUpdateMutex.lock();
        if(!stmUpdateCond.wait(&stmUpdateMutex, 1000 * 2))  //�ȴ���ʱ2��
        {
            //�ȵ���Ӧ����ʱ,�ص�0x19
            stmUpdateMutex.unlock();
            file.close();
            return 0;
        }
        stmUpdateMutex.unlock();
        nReadByte = file.read((char*)&packet[7], 128);
    }
    file.atEnd() ? ret_flag = 1 : ret_flag = 0;
    qDebug("Send Stm update bin data finished, ret_flag is %d\n",ret_flag);
    if(ret_flag)//�ж��ǲ����Ѿ������ļ�β��
    {
        QMutexLocker locker(&stmUpdateMutex);
        qDebug("Total send count: [%d], total receive count: [%d]\n",
               nSendCount, m_updateStm_updateAnswer);
        ret_flag = 1;
    }
    else//�ļ���ȡ��������
    {
        QMutexLocker locker(&stmUpdateMutex);
        qDebug("error occured!! Total send count:[%d]��!, total receive count: [%d]\n",
               nSendCount,m_updateStm_updateAnswer);
        ret_flag = -1;
    }
    file.close();
    sleep(1);

    QMutexLocker locker(&stmUpdateMutex);
    if(	m_updateStm_updateAnswer < nSendCount ) //�ж�������
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
������: sendStmBoot
����: ����ָ�������ֵ���Ƭ��
�׳��쳣: ��
--------------------------------------------------------------------------------
��ע: stmUpdaterThread���Ա����
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
    /*LEN: COMMAND+DATA+VERIFY(һ���ֽ�)*/
    sendBuf[index++] = cmd;
    /*VERIFY= LEN XOR COMMAND XOR DATA*/
    sendBuf[index++] = cal_xor(&sendBuf[3],2);
    sendBuf[index++] = 0xCC;

    writeData((char*)sendBuf, index);
}

/** @function
********************************************************************************
<PRE>
������: cal_xor
����:  ����У���
�׳��쳣: ��
--------------------------------------------------------------------------------
��ע: stmUpdaterThread���Ա����
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
������: writeData
����: д��������
�׳��쳣: ��
--------------------------------------------------------------------------------
��ע:stmUpdaterThread���Ա����
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
