#ifndef FTPCLIENT_H
#define FTPCLIENT_H

#include <QObject>
#include <QString>
#include <QNetworkReply>

//前置声明
class QByteArray;
class QFile;
class QUrl;
class QNetworkAccessManager;
class QNetworkRequest;
class QTimer;

//错误类型枚举
enum appUpdaterError
{
    noError = 0,        //无错误
    hostError,          //服务器名错误
    usernameError,      //用户名错误
    passwordError,      //密码错误
    portError,          //服务器端口错误
    filepathError,      //文件路径错误
    macError,           //MAC错误
    downloadError,      //文件下载失败
    filetypeError,      //文件类型错误
    localfilechmodError,//本地文件权限更改错误
    localfileopenError  //本地文件打开错误
};

//传入Ftp信息结构
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

//简易Ftp客户端类，只支持文件上传下载
class ftpClient : public QObject
{
    Q_OBJECT

private:
    //不允许复制
    ftpClient(const ftpClient&);
    ftpClient& operator=(const ftpClient&);

public:
    ftpClient();
    ~ftpClient();

    //FTP上传文件-重载1
    //data 需要上传的数据
    bool put(const QUrl &url, const QByteArray &data);

    //FTP上传文件-重载2
    bool put(const QUrl &url, QFile *pFile);

    //FTP下载文件
    //pFile 下载文件本地文件指针
    bool get(const QUrl &url, QFile *pFile);

//声明信号
signals:
    //操作完成信号
    //result: ftp操作结果
    void processCompleted(bool result);

//声明槽
private slots:
    //网络数据完成调用槽
    void dealFinish(QNetworkReply *pReply);

    //上传进度槽
    void dealuploadProgress( qint64 bytesSent, qint64 bytesTotal );

    //下载进度槽
    void dealdownloadProcess( qint64 bytesReceived, qint64 bytesTotal );

    //处理超时
    void dealtimeOut();

    //处理错误
    void dealError(QNetworkReply::NetworkError code);

private:
    QNetworkAccessManager *m_pmanager;  //网络管理类
    QNetworkReply *m_pReply;            //网络回应
    QNetworkRequest *m_pRequest;        //网络请求
    QFile *m_pFile;                     //当前操作文件指针
    QTimer *m_ptimer;                   //计时器

private:
    static quint32 MAX_WRITE_BYTES;
    static quint32 MAX_TIMEOUT;     //操作超时3分钟
};

//程序文件更新类
class appUpdater : public QObject
{
    Q_OBJECT
private:
    //不允许复制
    appUpdater(const appUpdater&);
    appUpdater& operator=(const appUpdater&);

public:
    appUpdater();
    ~appUpdater();

    //执行软件程序更新API
    appUpdaterError processUpdate(const posFtpInfo_t &ftpInfo);

private:
    //软件更新内部API
    appUpdaterError __processUpdate(const posFtpInfo_t &ftpInfo);

    //校验ftp信息
    appUpdaterError __checkFtpInfo(const posFtpInfo_t &ftpInfo);

    //校验文件CRC
    bool __checkDownFileCRC(QFile *file, const QString &strCRC);

    //删除文件
    void __deleteFile(QFile *file);

    //重启系统
    void __rebootSystem();

//信号量
signals:
    void quitloop();

//槽函数
private slots:
    //FTP操作完成响应函数
    void dealFtpOperateComplete(bool result);

private:
    ftpClient *m_pFtpClient;   //FTP客户端
    bool m_bResult;            //下载结果

private:
    static const QString UPDATE_DIR;        //升级文件存放目录
    static const QString MAIN_FILE;         //主程序文件名
    static const QString CARDREADER_FILE;   //读卡器程序文件名
    static const QString UPDATE_SUCCESS_FILE;   //升级成功标志文件名
};

#endif // FTPCLIENT_H
