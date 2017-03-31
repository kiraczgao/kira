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

void tradeDialog::_initTradeDialogWidget()
{
    //����û�б�Ҫ�ı�ǩ
    ui->debitNameLabel->hide();
    ui->balanceNameLabel->hide();
    //�������ѽ��
    ui->debitLabel->setGeometry(160,75,190,60);
    ui->debitLabel->setStyleSheet("font: 54pt \"Arial\";\n"
                                  "color: rgb(255, 255, 255);");
    //�������
    ui->balanceLabel->setGeometry(160,155,190,60);
    ui->balanceLabel->setStyleSheet("font: 54pt \"Arial\";\n"
                                    "color: rgb(255, 255, 255);");
}
