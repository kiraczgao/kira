#include "signalwidget.h"
#include "ui_signalwidget.h"
#include <QDateTimeEdit>
#include <QDateTime>
#include <QDate>
#include <QTime>
#include <QTimer>
#include <QDebug>
#include "datastructure.h"

signalWidget::signalWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::signalWidget)
{
    ui->setupUi(this);
    ui->signalDateEdit->setDisabled(true);
    ui->signalTimeEdit->setDisabled(true);
    //新建一个QTimer对象
    myTimer = new QTimer();
    //设置定时器每个多少毫秒发送一个timeout()信号
    myTimer->setInterval(1000);
    //启动定时器
    myTimer->start();
    connect(myTimer, SIGNAL(timeout()), this, SLOT(onTimerOut()));
}

signalWidget::~signalWidget()
{
    delete ui;
}

void signalWidget::onTimerOut()
{
    //获取系统当前时间

    QDateTime datetime = QDateTime::currentDateTime();
    QDateTime ldatetime = datetime.addSecs(UTC_TIMEDIFF);

    ui->signalDateEdit->setDateTime(ldatetime);
    ui->signalTimeEdit->setDateTime(ldatetime);

}
