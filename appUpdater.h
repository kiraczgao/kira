#ifndef FTPCLIENT_H
#define FTPCLIENT_H

#include <QObject>
#include <QString>
#include <QNetworkReply>

//ǰ������
class QByteArray;
class QFile;
class QUrl;
class QNetworkAccessManager;
class QNetworkRequest;
class QTimer;

//��������ö��
enum appUpdaterError
{
    noError = 0,        //�޴���
    hostError,          //������������
    usernameError,      //�û�������
    passwordError,      //�������
    portError,          //�������˿ڴ���
    filepathError,      //�ļ�·������
    macError,           //MAC����
    downloadError,      //�ļ�����ʧ��
    filetypeError,      //�ļ����ʹ���
    localfilechmodError,//�����ļ�Ȩ�޸��Ĵ���
    localfileopenError  //�����ļ��򿪴���
};

//����Ftp��Ϣ�ṹ
typedef struct posFtpInfo_s
{
    QString version;
    QString ftpIp;
    QString ftpPort;
    QString ftpPath;
    QString ftpUser;
    QString ftpPassword;
    QString crc;
}posFtpInfo_t;

//����Ftp�ͻ����ֻ֧࣬���ļ��ϴ�����
class ftpClient : public QObject
{
    Q_OBJECT

private:
    //��������
    ftpClient(const ftpClient&);
    ftpClient& operator=(const ftpClient&);

public:
    ftpClient();
    ~ftpClient();

    //FTP�ϴ��ļ�-����1
    //data ��Ҫ�ϴ�������
    bool put(const QUrl &url, const QByteArray &data);

    //FTP�ϴ��ļ�-����2
    bool put(const QUrl &url, QFile *pFile);

    //FTP�����ļ�
    //pFile �����ļ������ļ�ָ��
    bool get(const QUrl &url, QFile *pFile);

//�����ź�
signals:
    //��������ź�
    //result: ftp�������
    void processCompleted(bool result);

//������
private slots:
    //����������ɵ��ò�
    void dealFinish(QNetworkReply *pReply);

    //�ϴ����Ȳ�
    void dealuploadProgress( qint64 bytesSent, qint64 bytesTotal );

    //���ؽ��Ȳ�
    void dealdownloadProcess( qint64 bytesReceived, qint64 bytesTotal );

    //����ʱ
    void dealtimeOut();

    //�������
    void dealError(QNetworkReply::NetworkError code);

private:
    QNetworkAccessManager *m_pmanager;  //���������
    QNetworkReply *m_pReply;            //�����Ӧ
    QNetworkRequest *m_pRequest;        //��������
    QFile *m_pFile;                     //��ǰ�����ļ�ָ��
    QTimer *m_ptimer;                   //��ʱ��

private:
    static quint32 MAX_WRITE_BYTES;
    static quint32 MAX_TIMEOUT;     //������ʱ3����
};

//�����ļ�������
class appUpdater : public QObject
{
    Q_OBJECT
private:
    //��������
    appUpdater(const appUpdater&);
    appUpdater& operator=(const appUpdater&);

public:
    appUpdater();
    ~appUpdater();

    //ִ������������API
    appUpdaterError processUpdate(const posFtpInfo_t &ftpInfo);

private:
    //��������ڲ�API
    appUpdaterError __processUpdate(const posFtpInfo_t &ftpInfo);

    //У��ftp��Ϣ
    appUpdaterError __checkFtpInfo(const posFtpInfo_t &ftpInfo);

    //У���ļ�CRC
    bool __checkDownFileCRC(QFile *file, const QString &strCRC);

    //ɾ���ļ�
    void __deleteFile(QFile *file);

    //����ϵͳ
    void __rebootSystem();

//�ź���
signals:
    void quitloop();

//�ۺ���
private slots:
    //FTP���������Ӧ����
    void dealFtpOperateComplete(bool result);

private:
    ftpClient *m_pFtpClient;   //FTP�ͻ���
    bool m_bResult;            //���ؽ��

private:
    static const QString UPDATE_DIR;        //�����ļ����Ŀ¼
    static const QString MAIN_FILE;         //�������ļ���
    static const QString CARDREADER_FILE;   //�����������ļ���
    static const QString UPDATE_SUCCESS_FILE;   //�����ɹ���־�ļ���
};

#endif // FTPCLIENT_H
