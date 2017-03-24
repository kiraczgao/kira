#ifndef FTPCLIENT_H
#define FTPCLIENT_H

#include <QObject>
#include <QString>
#include <QThread>
#include <QNetworkReply>
#include <QMutex>
#include <QWaitCondition>

//ǰ������
class QByteArray;
class QFile;
class QUrl;
class QNetworkAccessManager;
class QNetworkRequest;
class QTimer;
class QMutex;
class stmSerialTalk;

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
    QString version;    //�汾��
    QString ftpIp;      //ftp��������ַ
    QString ftpPort;    //ftp�������˿ں�
    QString ftpPath;    //ftp�ļ�·��
    QString ftpUser;    //ftp�û���
    QString ftpPassword;//ftp����
    QString crc;        //�ļ�crc32У��ֵ
}posFtpInfo_t;

///@brief ����Ftp�ͻ����ֻ֧࣬���ļ��ϴ�����
class ftpClient : public QObject
{
    Q_OBJECT

private:
    //��������
    ftpClient(const ftpClient&);
    ftpClient& operator=(const ftpClient&);

public:
    //���캯��
    ftpClient();
    //��������
    ~ftpClient();

    ///FTP�ϴ��ļ�-����1
    //����˵����url:�ϴ��ĵ�ַ  data ��Ҫ�ϴ�������
    bool put(const QUrl &url, const QByteArray &data);

    ///FTP�ϴ��ļ�-����2
    //����˵����url:�ϴ��ĵ�ַ  data ��Ҫ�ϴ�������
    bool put(const QUrl &url, QFile *pFile);

    ///FTP�����ļ�
    //����˵����url:�ϴ��ĵ�ַ  pFile �����ļ������ļ�ָ��
    bool get(const QUrl &url, QFile *pFile);

//�����ź�
signals:
    ///ftp��������ź�
    //result: ftp�������
    void processCompleted(bool result);

//������
private slots:
    ///����������ɵ��ò�
    void dealFinish(QNetworkReply *pReply);

    ///�ϴ����Ȳ�
    void dealuploadProgress( qint64 bytesSent, qint64 bytesTotal );

    ///���ؽ��Ȳ�
    void dealdownloadProcess( qint64 bytesReceived, qint64 bytesTotal );

    ///����ʱ
    void dealtimeOut();

    ///�������
    void dealError(QNetworkReply::NetworkError code);

private:
    ///���������
    QNetworkAccessManager *m_pmanager;
    ///�����Ӧ
    QNetworkReply *m_pReply;
    ///��������
    QNetworkRequest *m_pRequest;
    ///��ǰ�����ļ�ָ��
    QFile *m_pFile;
    ///��ʱ��
    QTimer *m_ptimer;

private:
    ///�������д���ֽ�
    static quint32 MAX_WRITE_BYTES;
    ///������ʱʱ�䣨���룩
    static quint32 MAX_TIMEOUT;
};

///@brief �����ļ�������
class appUpdater : public QObject
{
    Q_OBJECT
private:
    //��������
    appUpdater(const appUpdater&);
    appUpdater& operator=(const appUpdater&);

public:
    //���캯��
    appUpdater();
    //��������
    ~appUpdater();

    /**
    * ִ������������API
    *  @param [in] ftpInfo ftp����������ϸ��Ϣ
    *  @param [in] pstmSerialTalk ��Ƭ��ͨѶ����ָ��
    *  @return ���������룬���ö���ඨ��
    */
    appUpdaterError processUpdate(const posFtpInfo_t &ftpInfo, stmSerialTalk *pstmSerialTalk);

private:
    ///��������ڲ�API
    appUpdaterError __processUpdate(const posFtpInfo_t &ftpInfo, stmSerialTalk *pstmSerialTalk);

    ///�������������
    void __processMainUpdate();

    ///�������������
    void __processCardReaderUpdate(stmSerialTalk *pstmSerialTalk);

    ///У��ftp��Ϣ
    appUpdaterError __checkFtpInfo(const posFtpInfo_t &ftpInfo);

    ///У���ļ�CRC
    bool __checkDownFileCRC(QFile *file, const QString &strCRC);

    ///ɾ���ļ�
    void __deleteFile(QFile *file);

//�ź���
signals:
    /**
    * �����˳��ڲ���Ϣѭ���ź�
    *  @note	QT�ź�
    */
    void quitloop();

//�ۺ���
private slots:
    /**
    * ftp������ɴ�����
    *  @param [in] result �������
    *  @note	QT�ۺ���
 */
    void dealFtpOperateComplete(bool result);

private:
    ///FTP�ͻ���
    ftpClient *m_pFtpClient;
    ///���ؽ��
    bool m_bResult;

public:
    ///�����ļ����Ŀ¼
    static const QString UPDATE_DIR;
    ///�������ļ���
    static const QString MAIN_FILE;
    ///�����������ļ���
    static const QString CARDREADER_FILE;
    ///�����ɹ���־�ļ���
    static const QString UPDATE_SUCCESS_FILE;
};


///@brief ��Ƭ�����������߳���
class stmUpdaterThread : public QThread
{
    Q_OBJECT

public:
    //���캯��
    stmUpdaterThread(int *fd);
    //��������
    ~stmUpdaterThread();

public:
    ///�ɹ���������ָ���־
    volatile bool m_updateStm_restartOK;
    ///�ɹ�����boot��־
    volatile bool m_updateStm_intoBootOK;
    ///�ɹ�ɾ��app��־
    volatile bool m_updateStm_deleteOK;
    ///����Ԥ����־���˱�־��Ч�ɴ�����һ��
    volatile bool m_updateStm_transferReady;
    ///������ɱ�־
    volatile bool m_updateStm_update_ok;
    ///�ɹ�����Ӧ�����
    int m_updateStm_updateAnswer;
    ///��Ƭ�������߳���
    QMutex stmUpdateMutex;
    ///��������ȴ�����
    QWaitCondition stmUpdateCond;

protected:
    ///�̺߳���
    virtual void run();

private:
    ///�����豸������ָ��
    int *stmfd;

//�ڲ�����
private:
    /**
    * ������Ƭ��Ӧ��
    *  @param [out] nRetryTimes ���Դ�����������
    *  @return ����ִ��״̬
    *  @retval 0 ��ǰ״ִ̬�����
    *  @retval 1 ���в���ִ�����
    */
    int updateStmApp(int &nRetryTimes);

    /**
    * ���͵�Ƭ�������ļ�����
    *  @return ���ط��������ļ����
    *  @retval 0 ���͹��̳��ִ���
    *  @retval -1 �ļ���������
    */
    int sendStmBinData();

    /**
    * ������Ƭ��ָ��ͺ���
    *  @param [in] cmd ������������
    */
    void sendStmBoot(quint8 cmd);

    /**
    * ��������
    *  @param [in] data ָ����Ҫ����У��������׵�ַ
    *  @param [in] count ��Ҫ�������ݵĳ���
    *  @return ����õ���У��ֵ
    */
    quint8 cal_xor(const quint8 *data,int count);

    /**
    * д��������
    *  @param [in] data ָ����Ҫд�����ݵ��׵�ַ
    *  @param [in] datalen ��Ҫ���͵����ݳ���
    *  @param [in] isDisplay �Ƿ��ӡ��־��Ĭ��true
    */
    void writeData(char *data, int datalen, bool isDisplay = true);

private:
    ///��Ƭ������״̬
    char m_updateStmStatus;

signals:
    ///��Ƭ������ʧ���ź�
    void stmUpdateFail();
};

#endif // FTPCLIENT_H
