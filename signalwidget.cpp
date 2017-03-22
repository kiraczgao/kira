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
    //�½�һ��QTimer����
    myTimer = new QTimer();
    //���ö�ʱ��ÿ�����ٺ��뷢��һ��timeout()�ź�
    myTimer->setInterval(1000);
    //������ʱ��
    myTimer->start();
    connect(myTimer, SIGNAL(timeout()), this, SLOT(onTimerOut()));
}

signalWidget::~signalWidget()
{
    delete ui;
}

void signalWidget::onTimerOut()
{
    //��ȡϵͳ��ǰʱ��

    QDateTime datetime = QDateTime::currentDateTime();
    QDateTime ldatetime = datetime.addSecs(UTC_TIMEDIFF);

    ui->signalDateEdit->setDateTime(ldatetime);
    ui->signalTimeEdit->setDateTime(ldatetime);

}
