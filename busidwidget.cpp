#include "busidwidget.h"
#include "ui_busidwidget.h"

const QString imageUncheck = "image: url(:/images/uncheck.png); \
                                color: rgb(255, 255, 255); \
                                font: 75 16pt \"Î¢ÈíÑÅºÚ\";";

const QString imageCheck = "image: url(:/images/check.png); \
                            color: rgb(255, 255, 255); \
                            font: 75 16pt \"Î¢ÈíÑÅºÚ\";";

busidWidget::busidWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::busidWidget)
{
    ui->setupUi(this);

    myIDLabel = idLabel_1;
}

busidWidget::~busidWidget()
{
    delete ui;
}

void busidWidget::setBusID(QString sid)
{
    busid = sid;
    int len = (busid.length()>8)?8:busid.length();
    for(int i=0; i<8; i++)
    {
        if(i<len)
        {
            busidArray[i] = busid.mid(i, 1);
        }
        else
        {
            busidArray[i] = QString();
        }
        switch(i)
        {
            case 0:
                ui->IDlabel_1->setText(busidArray[i]);
                break;
            case 1:
                ui->IDlabel_2->setText(busidArray[i]);
                break;
            case 2:
                ui->IDlabel_3->setText(busidArray[i]);
                break;
            case 3:
                ui->IDlabel_4->setText(busidArray[i]);
                break;
            case 4:
                ui->IDlabel_5->setText(busidArray[i]);
                break;
            case 5:
                ui->IDlabel_6->setText(busidArray[i]);
                break;
            case 6:
                ui->IDlabel_7->setText(busidArray[i]);
                break;
            case 7:
                ui->IDlabel_8->setText(busidArray[i]);
            break;
        }
    }
}

void busidWidget::keyboardLeftpress()
{
    qDebug("kira --- process bus id left key press...");
    int itemp = 0;
    switch(myIDLabel)
    {
        case idLabel_1:
            if(busidArray[0] == "9")
                busidArray[0] = "0";
            else if((busidArray[0] >= "0")&&(busidArray[0] < "9"))
            {
                itemp = busidArray[0].toInt();
                busidArray[0] = QString::number(itemp+1, 10);
            }
            else
                busidArray[0] = "0";
            ui->IDlabel_1->setText(busidArray[0]);
            break;
        case idLabel_2:
            if(busidArray[1] == "-")
                busidArray[1] = "0";
            else if(busidArray[1] == "9")
                busidArray[1] = "-";
            else if((busidArray[1] >= "0")&&(busidArray[1] < "9"))
            {
                itemp = busidArray[1].toInt();
                busidArray[1] = QString::number(itemp+1, 10);
            }
            else
                busidArray[1] = "0";
            ui->IDlabel_2->setText(busidArray[1]);
            break;
        case idLabel_3:
            if(busidArray[2] == "-")
                busidArray[2] = "0";
            else if(busidArray[2] == "9")
                busidArray[2] = "-";
            else if((busidArray[0] >= "0")&&(busidArray[2] < "9"))
            {
                itemp = busidArray[2].toInt();
                busidArray[2] = QString::number(itemp+1, 10);
            }
            else
                busidArray[2] = "0";
            ui->IDlabel_3->setText(busidArray[2]);
            break;
        case idLabel_4:
            if(busidArray[3] == "-")
                busidArray[3] = "0";
            else if(busidArray[3] == "9")
                busidArray[3] = "-";
            else if((busidArray[3] >= "0")&&(busidArray[3] < "9"))
            {
                itemp = busidArray[3].toInt();
                busidArray[3] = QString::number(itemp+1, 10);
            }
            else
                busidArray[3] = "0";
            ui->IDlabel_4->setText(busidArray[3]);
            break;
        case idLabel_5:
            if(busidArray[4] == "-")
                busidArray[4] = "0";
            else if(busidArray[4] == "9")
                busidArray[4] = "-";
            else if((busidArray[4] >= "0")&&(busidArray[4] < "9"))
            {
                itemp = busidArray[4].toInt();
                busidArray[4] = QString::number(itemp+1, 10);
            }
            else
                busidArray[4] = "0";
            ui->IDlabel_5->setText(busidArray[4]);
            break;
        case idLabel_6:
            if(busidArray[5] == "-")
                busidArray[5] = "0";
            else if(busidArray[5] == "9")
                busidArray[5] = "-";
            else if((busidArray[5] >= "0")&&(busidArray[5] < "9"))
            {
                itemp = busidArray[5].toInt();
                busidArray[5] = QString::number(itemp+1, 10);
            }
            else
                busidArray[5] = "0";
            ui->IDlabel_6->setText(busidArray[5]);
            break;
        case idLabel_7:
            if(busidArray[6] == QString())
                busidArray[6] = "0";
            else if(busidArray[6] == "-")
                busidArray[6] = QString();
            else if(busidArray[6] == "9")
                busidArray[6] = "-";
            else if((busidArray[6] >= "0")&&(busidArray[6] < "9"))
            {
                itemp = busidArray[6].toInt();
                busidArray[6] = QString::number(itemp+1, 10);
            }
            else
                busidArray[6] = "0";
            ui->IDlabel_7->setText(busidArray[6]);
                break;
        case idLabel_8:
            if(busidArray[7] == QString())
                busidArray[7] = "0";
            else if(busidArray[7] == "9")
                busidArray[7] = QString();
            else if((busidArray[7] >= "0")&&(busidArray[7] < "9"))
            {
                itemp = busidArray[7].toInt();
                busidArray[7] = QString::number(itemp+1, 10);
            }
            else
                busidArray[7] = "0";
            ui->IDlabel_8->setText(busidArray[7]);
            break;
    }
}

void busidWidget::keyboardRightpress()
{
    qDebug("kira --- process bus id right key press...");
    switch(myIDLabel)
    {
        case idLabel_1:
            myIDLabel = idLabel_2;
            ui->IDlabel_1->setStyleSheet(imageUncheck);
            ui->IDlabel_2->setStyleSheet(imageCheck);
            break;
        case idLabel_2:
            myIDLabel = idLabel_3;
            ui->IDlabel_2->setStyleSheet(imageUncheck);
            ui->IDlabel_3->setStyleSheet(imageCheck);
            break;
        case idLabel_3:
            myIDLabel = idLabel_4;
            ui->IDlabel_3->setStyleSheet(imageUncheck);
            ui->IDlabel_4->setStyleSheet(imageCheck);
            break;
        case idLabel_4:
            myIDLabel = idLabel_5;
            ui->IDlabel_4->setStyleSheet(imageUncheck);
            ui->IDlabel_5->setStyleSheet(imageCheck);
            break;
        case idLabel_5:
            myIDLabel = idLabel_6;
            ui->IDlabel_5->setStyleSheet(imageUncheck);
            ui->IDlabel_6->setStyleSheet(imageCheck);
            break;
        case idLabel_6:
            myIDLabel = idLabel_7;
            ui->IDlabel_6->setStyleSheet(imageUncheck);
            ui->IDlabel_7->setStyleSheet(imageCheck);
            break;
        case idLabel_7:
            myIDLabel = idLabel_8;
            ui->IDlabel_7->setStyleSheet(imageUncheck);
            ui->IDlabel_8->setStyleSheet(imageCheck);
            break;
        case idLabel_8:
            myIDLabel = idLabel_1;
            ui->IDlabel_8->setStyleSheet(imageUncheck);
            ui->IDlabel_1->setStyleSheet(imageCheck);
            break;
    }
}

void busidWidget::keyboardShortpress()
{
    busid = QString();
    for(int i=0; i<8; i++)
        busid.append(busidArray[i]);
    initLabelState();
    qDebug(qPrintable(busid));
    emit checkConfirm();
}

void busidWidget::initLabelState()
{
    myIDLabel = idLabel_1;
    ui->IDlabel_1->setStyleSheet(imageCheck);
    ui->IDlabel_2->setStyleSheet(imageUncheck);
    ui->IDlabel_3->setStyleSheet(imageUncheck);
    ui->IDlabel_4->setStyleSheet(imageUncheck);
    ui->IDlabel_5->setStyleSheet(imageUncheck);
    ui->IDlabel_6->setStyleSheet(imageUncheck);
    ui->IDlabel_7->setStyleSheet(imageUncheck);
    ui->IDlabel_8->setStyleSheet(imageUncheck);
}
