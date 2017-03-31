#include "poswidget.h"
#include "ui_poswidget.h"
#include <QDateTimeEdit>
#include <QDateTime>
#include <QTimer>
#include <QFont>
#include <QFontDatabase>
#include <QTextCodec>
#include <QDebug>

posWidget::posWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::posWidget)
{
    ui->setupUi(this);
    _initPosWidget();
    //QFontDatabase::addApplicationFont(":/font/SWISSK.TTF");
    //QFontDatabase::addApplicationFont(":/font/SWISSK1.TTF");
    //ui->ticketsLabel->setFont(QFont("SWISSK", 2.00));
    isShowPosWidget = true;
    //tickets = "3.00";
    //ui->ticketsLabel->setText(tickets);
    ui->dateTimeEdit->setDisabled(true);
    //新建一个QTimer对象
    myTimer = new QTimer();
    //设置定时器每个多少毫秒发送一个timeout()信号
    myTimer->setInterval(1000);
    //启动定时器
    myTimer->start();
    connect(myTimer, SIGNAL(timeout()), this, SLOT(onTimerOut()));
}

posWidget::~posWidget()
{
    delete ui;
}

void posWidget::onTimerOut()
{
    //获取系统当前时间
    QDateTime date = QDateTime::currentDateTime();

    //double dDateTime = date.toString("yyyyMMddhhmmss") + UTC_TIMEDIFF;
    //QString sDateTime = QString::number((qulonglong)dDateTime, 10);
    //qDebug(qPrintable(sDateTime));
    QDateTime ldate = date.addSecs(UTC_TIMEDIFF);

    ui->dateTimeEdit->setDateTime(ldate);
}

void posWidget::setVersion(QString &ver)
{
    qDebug("qwy --- set version: ");
    qDebug(qPrintable(ver));
    ui->version->setText(ver);
}

void posWidget::setTickets(QString &stickets)
{
    tickets = stickets;
    qDebug("kira --- set tickets: ");
    qDebug(qPrintable(tickets));
    ui->ticketsLabel->setText(tickets);
}

void posWidget::setBusID(const QString &sbusID)
{
    busID = sbusID;
    QString nbusID = busID.replace("^0*", "");
    ui->busIDLabel->setText(nbusID);
}

void posWidget::setBuslineID(const QString &sbuslineID)
{
    buslineID = sbuslineID;
}

void posWidget::setposID(const QString &sposID)
{
    posID = sposID;
}

void posWidget::setdriverID(const QString &sdriverID)
{
    driverID = sdriverID;
    ui->signLabel->setText(driverID);
}

void posWidget::_initPosWidget()
{
    //隐藏没有必要的标签
    ui->ticketsNameLabel->hide();
    ui->ticketsNameLabel_2->hide();
    //放置NO:标签
    ui->label->setGeometry(290,10,50,35);
    ui->label->setText(QApplication::translate("posWidget",
                                           "<html><head/><body><p><span style=\" font-weight:600;\">NO:</span></p></body></html>",
                                           0,
                                           QApplication::UnicodeUTF8));
    //放置车辆编号lable
    ui->busIDLabel->setGeometry(340,10,130,35);
    //放置时间
    ui->dateTimeEdit->setGeometry(40,10,240,35);
    //放置票价信息
    ui->ticketsLabel->setGeometry(100,125,280,80);
    ui->ticketsLabel->setStyleSheet("color: rgb(255, 255, 255);\n"
                                    "font: 80pt \"Arial\";\n"
                                    "font-weight:600\n");
    ui->signLabel->setGeometry(120,240,240,20);
}
