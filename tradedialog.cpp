#include "tradedialog.h"
#include "ui_tradedialog.h"
#include <QTimer>

tradeDialog::tradeDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::tradeDialog)
{
    ui->setupUi(this);
    //新建一个QTimer对象
    myTimer = new QTimer();
    //设置定时器每个多少毫秒发送一个timeout()信号
    myTimer->setInterval(3*1000);
    myTimer->stop();
    connect(myTimer, SIGNAL(timeout()), this, SLOT(onTimerOut()));
}

tradeDialog::~tradeDialog()
{
    delete ui;
}

void tradeDialog::onTimerOut()
{
    emit backPosWidget();
}

void tradeDialog::setBalance(const QString &sbalance)
{
    balance = sbalance;
    ui->balanceLabel->setText(balance);
}

void tradeDialog::setDebit(const QString &sdebit)
{
    debit = sdebit;
    ui->debitLabel->setText(debit);
}
