#ifndef TRADEDIALOG_H
#define TRADEDIALOG_H

#include <QDialog>
#include <QString>

class QTimer;
namespace Ui {
class tradeDialog;
}

class tradeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit tradeDialog(QWidget *parent = 0);
    ~tradeDialog();
    void setBalance(const QString& sbalance);
    void getBalance(QString& gbalance) { gbalance = balance; }
    void setDebit(const QString& sdebit);
    void getDebit(QString& gdebit) { gdebit = debit; }

private:
    //初始化交易界面UI
    void _initTradeDialogWidget();

private:
    Ui::tradeDialog *ui;

signals:
    void backPosWidget();

public slots:
    void onTimerOut();

public:
    QTimer* myTimer;
    QString balance;
    QString debit;
};

#endif // TRADEDIALOG_H
