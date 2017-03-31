#include "tradedialog.h"
#include "ui_tradedialog.h"
#include <QTimer>
#include "setconfig.h"

tradeDialog::tradeDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::tradeDialog)
{
    ui->setupUi(this);
    _initTradeDialogWidget();
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

void tradeDialog::_initTradeDialogWidget()
{
    //隐藏没有必要的标签
    ui->debitNameLabel->hide();
    ui->balanceNameLabel->hide();
    //放置消费金额
    ui->debitLabel->setGeometry(160,75,190,60);
    ui->debitLabel->setStyleSheet("font: 54pt \"Arial\";\n"
                                  "color: rgb(255, 255, 255);");
    //放置余额
    ui->balanceLabel->setGeometry(160,155,190,60);
    ui->balanceLabel->setStyleSheet("font: 54pt \"Arial\";\n"
                                    "color: rgb(255, 255, 255);");
}
