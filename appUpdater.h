#ifndef FTPCLIENT_H
#define FTPCLIENT_H

#include <QObject>
#include <QString>
#include <QThread>
#include <QNetworkReply>
#include <QMutex>
#include <QWaitCondition>

//前置声明
class QByteArray;
class QFile;
class QUrl;
class QNetworkAccessManager;
class QNetworkRequest;
class QTimer;
class QMutex;
class stmSerialTalk;

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
    QString version;    //版本号
    QString ftpIp;      //ftp服务器地址
    QString ftpPort;    //ftp服务器端口号
    QString ftpPath;    //ftp文件路径
    QString ftpUser;    //ftp用户名
    QString ftpPassword;//ftp密码
    QString crc;        //文件crc32校验值
}posFtpInfo_t;

///@brief 简易Ftp客户端类，只支持文件上传下载
class ftpClient : public QObject
{
    Q_OBJECT

private:
    //不允许复制
    ftpClient(const ftpClient&);
    ftpClient& operator=(const ftpClient&);

public:
    //构造函数
    ftpClient();
    //析构函数
    ~ftpClient();

    ///FTP上传文件-重载1
    //参数说明：url:上传的地址  data 需要上传的数据
    bool put(const QUrl &url, const QByteArray &data);

    ///FTP上传文件-重载2
    //参数说明：url:上传的地址  data 需要上传的数据
    bool put(const QUrl &url, QFile *pFile);

    ///FTP下载文件
    //参数说明：url:上传的地址  pFile 下载文件本地文件指针
    bool get(const QUrl &url, QFile *pFile);

//声明信号
signals:
    ///ftp操作完成信号
    //result: ftp操作结果
    void processCompleted(bool result);

//声明槽
private slots:
    ///网络数据完成调用槽
    void dealFinish(QNetworkReply *pReply);

    ///上传进度槽
    void dealuploadProgress( qint64 bytesSent, qint64 bytesTotal );

    ///下载进度槽
    void dealdownloadProcess( qint64 bytesReceived, qint64 bytesTotal );

    ///处理超时
    void dealtimeOut();

    ///处理错误
    void dealError(QNetworkReply::NetworkError code);

private:
    ///网络管理类
    QNetworkAccessManager *m_pmanager;
    ///网络回应
    QNetworkReply *m_pReply;
    ///网络请求
    QNetworkRequest *m_pRequest;
    ///当前操作文件指针
    QFile *m_pFile;
    ///计时器
    QTimer *m_ptimer;

private:
    ///单次最大写入字节
    static quint32 MAX_WRITE_BYTES;
    ///操作超时时间（毫秒）
    static quint32 MAX_TIMEOUT;
};

///@brief 程序文件更新类
class appUpdater : public QObject
{
    Q_OBJECT
private:
    //不允许复制
    appUpdater(const appUpdater&);
    appUpdater& operator=(const appUpdater&);

public:
    //构造函数
    appUpdater();
    //析构函数
    ~appUpdater();

    /**
    * 执行软件程序更新API
    *  @param [in] ftpInfo ftp服务器的详细信息
    *  @param [in] pstmSerialTalk 单片机通讯串口指针
    *  @return 升级返回码，详见枚举类定义
    */
    appUpdaterError processUpdate(const posFtpInfo_t &ftpInfo, stmSerialTalk *pstmSerialTalk);

private:
    ///软件更新内部API
    appUpdaterError __processUpdate(const posFtpInfo_t &ftpInfo, stmSerialTalk *pstmSerialTalk);

    ///工控主程序更新
    void __processMainUpdate();

    ///读卡器程序更新
    void __processCardReaderUpdate(stmSerialTalk *pstmSerialTalk);

    ///校验ftp信息
    appUpdaterError __checkFtpInfo(const posFtpInfo_t &ftpInfo);

    ///校验文件CRC
    bool __checkDownFileCRC(QFile *file, const QString &strCRC);

    ///删除文件
    void __deleteFile(QFile *file);

//信号量
signals:
    /**
    * 发送退出内部消息循环信号
    *  @note	QT信号
    */
    void quitloop();

//槽函数
private slots:
    /**
    * ftp操作完成处理函数
    *  @param [in] result 操作结果
    *  @note	QT槽函数
 */
    void dealFtpOperateComplete(bool result);

private:
    ///FTP客户端
    ftpClient *m_pFtpClient;
    ///下载结果
    bool m_bResult;

public:
    ///升级文件存放目录
    static const QString UPDATE_DIR;
    ///主程序文件名
    static const QString MAIN_FILE;
    ///读卡器程序文件名
    static const QString CARDREADER_FILE;
    ///升级成功标志文件名
    static const QString UPDATE_SUCCESS_FILE;
};


///@brief 单片机程序升级线程类
class stmUpdaterThread : public QThread
{
    Q_OBJECT

public:
    //构造函数
    stmUpdaterThread(int *fd);
    //析构函数
    ~stmUpdaterThread();

public:
    ///成功接收重启指令标志
    volatile bool m_updateStm_restartOK;
    ///成功进入boot标志
    volatile bool m_updateStm_intoBootOK;
    ///成功删除app标志
    volatile bool m_updateStm_deleteOK;
    ///传输预备标志，此标志有效可传输下一包
    volatile bool m_updateStm_transferReady;
    ///升级完成标志
    volatile bool m_updateStm_update_ok;
    ///成功升级应答次数
    int m_updateStm_updateAnswer;
    ///单片机升级线程锁
    QMutex stmUpdateMutex;
    ///升级传输等待条件
    QWaitCondition stmUpdateCond;

protected:
    ///线程函数
    virtual void run();

private:
    ///串口设备描述符指针
    int *stmfd;

//内部函数
private:
    /**
    * 升级单片机应用
    *  @param [out] nRetryTimes 重试次数变量引用
    *  @return 函数执行状态
    *  @retval 0 当前状态执行完成
    *  @retval 1 所有操作执行完成
    */
    int updateStmApp(int &nRetryTimes);

    /**
    * 发送单片机程序文件函数
    *  @return 返回发送升级文件结果
    *  @retval 0 发送过程出现错误
    *  @retval -1 文件操作出错
    */
    int sendStmBinData();

    /**
    * 升级单片机指令发送函数
    *  @param [in] cmd 待发送命令码
    */
    void sendStmBoot(quint8 cmd);

    /**
    * 计算异或和
    *  @param [in] data 指向需要计算校验的数据首地址
    *  @param [in] count 需要计算数据的长度
    *  @return 计算得到的校验值
    */
    quint8 cal_xor(const quint8 *data,int count);

    /**
    * 写串口数据
    *  @param [in] data 指向需要写入数据的首地址
    *  @param [in] datalen 需要发送的数据长度
    *  @param [in] isDisplay 是否打印日志，默认true
    */
    void writeData(char *data, int datalen, bool isDisplay = true);

private:
    ///单片机升级状态
    char m_updateStmStatus;

signals:
    ///单片机升级失败信号
    void stmUpdateFail();
};

#endif // FTPCLIENT_H
