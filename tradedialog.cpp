#include "tradedialog.h"
#include "ui_tradedialog.h"
#include <QTimer>

tradeDialog::tradeDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::tradeDialog)
{
    ui->setupUi(this);
    //�½�һ��QTimer����
    myTimer = new QTimer();
    //���ö�ʱ��ÿ�����ٺ��뷢��һ��timeout()�ź�
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
