#include "posidwidget.h"
#include "ui_posidwidget.h"

const QString imageUncheck = "image: url(:/images/uncheck.png); \
                                color: rgb(255, 255, 255); \
                                font: 75 16pt \"Î¢ÈíÑÅºÚ\";";

const QString imageCheck = "image: url(:/images/check.png); \
                            color: rgb(255, 255, 255); \
                            font: 75 16pt \"Î¢ÈíÑÅºÚ\";";

posidWidget::posidWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::posidWidget)
{
    ui->setupUi(this);
    myIDLabel = idLabel_1;
}

posidWidget::~posidWidget()
{
    delete ui;
}

void posidWidget::setPosID(QString sid)
{
    posid = sid;
    int len = (posid.length()>8)?8:posid.length();
    for(int i=0; i<8; i++)
    {
        if(i<len)
        {
            posidArray[i] = posid.mid(i, 1);
        }
        else
        {
            posidArray[i] = QString();
        }
        switch(i)
        {
            case 0:
                ui->IDlabel_1->setText(posidArray[i]);
                break;
            case 1:
                ui->IDlabel_2->setText(posidArray[i]);
                break;
            case 2:
                ui->IDlabel_3->setText(posidArray[i]);
                break;
            case 3:
                ui->IDlabel_4->setText(posidArray[i]);
                break;
            case 4:
                ui->IDlabel_5->setText(posidArray[i]);
                break;
            case 5:
                ui->IDlabel_6->setText(posidArray[i]);
                break;
            case 6:
                ui->IDlabel_7->setText(posidArray[i]);
                break;
            case 7:
                ui->IDlabel_8->setText(posidArray[i]);
            break;
        }
    }
}

void posidWidget::keyboardLeftpress()
{
    qDebug("kira --- process pos id left key press...");
    int itemp = 0;
    switch(myIDLabel)
    {
        case idLabel_1:
            if(posidArray[0] == "9")
                posidArray[0] = "0";
            else if((posidArray[0] >= "0")&&(posidArray[0] < "9"))
            {
                itemp = posidArray[0].toInt();
                posidArray[0] = QString::number(itemp+1, 10);
            }
            else
                posidArray[0] = "0";
            ui->IDlabel_1->setText(posidArray[0]);
            break;
        case idLabel_2:
            if(posidArray[1] == "9")
                posidArray[1] = "0";
            else if((posidArray[1] >= "0")&&(posidArray[1] < "9"))
            {
                itemp = posidArray[1].toInt();
                posidArray[1] = QString::number(itemp+1, 10);
            }
            else
                posidArray[1] = "0";
            ui->IDlabel_2->setText(posidArray[1]);
            break;
        case idLabel_3:
            if(posidArray[2] == "9")
                posidArray[2] = "0";
            else if((posidArray[0] >= "0")&&(posidArray[2] < "9"))
            {
                itemp = posidArray[2].toInt();
                posidArray[2] = QString::number(itemp+1, 10);
            }
            else
                posidArray[2] = "0";
            ui->IDlabel_3->setText(posidArray[2]);
            break;
        case idLabel_4:
            if(posidArray[3] == "9")
                posidArray[3] = "0";
            else if((posidArray[3] >= "0")&&(posidArray[3] < "9"))
            {
                itemp = posidArray[3].toInt();
                posidArray[3] = QString::number(itemp+1, 10);
            }
            else
                posidArray[3] = "0";
            ui->IDlabel_4->setText(posidArray[3]);
            break;
        case idLabel_5:
            if(posidArray[4] == "9")
                posidArray[4] = "0";
            else if((posidArray[4] >= "0")&&(posidArray[4] < "9"))
            {
                itemp = posidArray[4].toInt();
                posidArray[4] = QString::number(itemp+1, 10);
            }
            else
                posidArray[4] = "0";
            ui->IDlabel_5->setText(posidArray[4]);
            break;
        case idLabel_6:
            if(posidArray[5] == "9")
                posidArray[5] = "0";
            else if((posidArray[5] >= "0")&&(posidArray[5] < "9"))
            {
                itemp = posidArray[5].toInt();
                posidArray[5] = QString::number(itemp+1, 10);
            }
            else
                posidArray[5] = "0";
            ui->IDlabel_6->setText(posidArray[5]);
            break;
        case idLabel_7:
            if(posidArray[6] == QString())
                posidArray[6] = "0";
            else if(posidArray[6] == "9")
                posidArray[6] = QString();
            else if((posidArray[6] >= "0")&&(posidArray[6] < "9"))
            {
                itemp = posidArray[6].toInt();
                posidArray[6] = QString::number(itemp+1, 10);
            }
            else
                posidArray[6] = "0";
            ui->IDlabel_7->setText(posidArray[6]);
                break;
        case idLabel_8:
            if(posidArray[7] == QString())
                posidArray[7] = "0";
            else if(posidArray[7] == "9")
                posidArray[7] = QString();
            else if((posidArray[7] >= "0")&&(posidArray[7] < "9"))
            {
                itemp = posidArray[7].toInt();
                posidArray[7] = QString::number(itemp+1, 10);
            }
            else
                posidArray[7] = "0";
            ui->IDlabel_8->setText(posidArray[7]);
            break;
    }
}

void posidWidget::keyboardRightpress()
{
    qDebug("kira --- process pos id right key press...");
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

void posidWidget::keyboardShortpress()
{
    posid = QString();
    for(int i=0; i<8; i++)
        posid.append(posidArray[i]);
    initLabelState();
    qDebug(qPrintable(posid));
    emit checkConfirm();
}

void posidWidget::initLabelState()
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
