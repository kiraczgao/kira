#include "posparamwidget.h"
#include "ui_posparamwidget.h"

posparamwidget::posparamwidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::posparamwidget)
{
    ui->setupUi(this);
}

posparamwidget::~posparamwidget()
{
    delete ui;
}

void posparamwidget::setPosID(const QString &strPosID)
{
    ui->labelPosID_2->setText(QString("<html><head/><body><p><span style=\" font-weight:600;\">%1</span></p></body></html>").arg(strPosID));
}

void posparamwidget::setBusID(const QString &strBusID)
{
    ui->labelBusID_2->setText(QString("<html><head/><body><p><span style=\" font-weight:600;\">%1</span></p></body></html>").arg(strBusID));
}

void posparamwidget::setLineID(const QString &strLineID)
{
    ui->labelLineID_2->setText(QString("<html><head/><body><p><span style=\" font-weight:600;\">%1</span></p></body></html>").arg(strLineID));
}

void posparamwidget::setTicket(const QString &strTicket)
{
    ui->labelTicket_2->setText(QString("<html><head/><body><p><span style=\" font-weight:600;\">%1</span></p></body></html>").arg(strTicket));
}
