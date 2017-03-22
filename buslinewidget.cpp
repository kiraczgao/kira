#include "buslinewidget.h"
#include "ui_buslinewidget.h"

const QString imageUncheck = "image: url(:/images/uncheck.png); \
                                color: rgb(255, 255, 255); \
                                font: 75 16pt \"Î¢ÈíÑÅºÚ\";";

const QString imageCheck = "image: url(:/images/check.png); \
                            color: rgb(255, 255, 255); \
                            font: 75 16pt \"Î¢ÈíÑÅºÚ\";";

buslineWidget::buslineWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::buslineWidget)
{
    ui->setupUi(this);

    myIDLabel = lineLabel_1;
}

buslineWidget::~buslineWidget()
{
    delete ui;
}

void buslineWidget::setBusline(QString sline)
{
    busline = sline;
    int len = (busline.length()>8)?8:busline.length();
    for(int i=0; i<8; i++)
    {
        if(i<len)
        {
            buslineArray[i] = busline.mid(i, 1);
        }
        else
        {
            buslineArray[i] = QString();
        }
        switch(i)
        {
            case 0:
                ui->IDlabel_1->setText(buslineArray[i]);
                break;
            case 1:
                ui->IDlabel_2->setText(buslineArray[i]);
                break;
            case 2:
                ui->IDlabel_3->setText(buslineArray[i]);
                break;
            case 3:
                ui->IDlabel_4->setText(buslineArray[i]);
                break;
            case 4:
                ui->IDlabel_5->setText(buslineArray[i]);
                break;
            case 5:
                ui->IDlabel_6->setText(buslineArray[i]);
                break;
            case 6:
                ui->IDlabel_7->setText(buslineArray[i]);
                break;
            case 7:
                ui->IDlabel_8->setText(buslineArray[i]);
            break;
        }
    }
}

void buslineWidget::setBusticket(QString sticket)
{
    ticket = sticket;
    //int len = (ticket.length()>4)?4:ticket.length();
    int index = ticket.indexOf(".");

    if(index == 1)
    {
        ticketArray[0] = QString();
        ticketArray[1] = ticket.mid(index-1, 1);
        ticketArray[2] = ticket.mid(index, 1);
        ticketArray[3] = ticket.mid(index+1, 1);
        ticketArray[4] = ticket.mid(index+2, 1);
    }
    else if(index == 2)
    {
        ticketArray[0] = ticket.mid(index-2, 1);
        ticketArray[1] = ticket.mid(index-1, 1);
        ticketArray[2] = ticket.mid(index, 1);
        ticketArray[3] = ticket.mid(index+1, 1);
        ticketArray[4] = ticket.mid(index+2, 1);
    }
    else
    {
        ticketArray[0] = "E";
        ticketArray[1] = "R";
        ticketArray[2] = "R";
        ticketArray[3] = "O";
        ticketArray[4] = "R";
    }

    for(int i=0; i<5; i++)
    {
        switch(i)
        {
            case 0:
                ui->IDlabel_9->setText(ticketArray[i]);
                break;
            case 1:
                ui->IDlabel_10->setText(ticketArray[i]);
                break;
            case 2:
                ui->IDlabel_11->setText(ticketArray[i]);
                break;
            case 3:
                ui->IDlabel_12->setText(ticketArray[i]);
                break;
            case 4:
                ui->IDlabel_13->setText(ticketArray[i]);
                break;
        }
    }
}

void buslineWidget::keyboardLeftpress()
{
    qDebug("kira --- process bus line and ticket left key press...");
    int itemp = 0;
    switch(myIDLabel)
    {
        case lineLabel_1:
            if(buslineArray[0] == "9")
                buslineArray[0] = "0";
            else if((buslineArray[0] >= "0")&&(buslineArray[0] < "9"))
            {
                itemp = buslineArray[0].toInt();
                buslineArray[0] = QString::number(itemp+1, 10);
            }
            else
                buslineArray[0] = "0";
            ui->IDlabel_1->setText(buslineArray[0]);
            break;
        case lineLabel_2:
            if(buslineArray[1] == QString())
                buslineArray[1] = "0";
            else if(buslineArray[1] == "-")
                buslineArray[1] = QString();
            else if(buslineArray[1] == "9")
                buslineArray[1] = "-";
            else if((buslineArray[1] >= "0")&&(buslineArray[1] < "9"))
            {
                itemp = buslineArray[1].toInt();
                buslineArray[1] = QString::number(itemp+1, 10);
            }
            else
                buslineArray[1] = "0";
            ui->IDlabel_2->setText(buslineArray[1]);
            break;
        case lineLabel_3:
            if(buslineArray[2] == QString())
                buslineArray[2] = "0";
            else if(buslineArray[2] == "-")
                buslineArray[2] = QString();
            else if(buslineArray[2] == "9")
                buslineArray[2] = "-";
            else if((buslineArray[0] >= "0")&&(buslineArray[2] < "9"))
            {
                itemp = buslineArray[2].toInt();
                buslineArray[2] = QString::number(itemp+1, 10);
            }
            else
                buslineArray[2] = "0";
            ui->IDlabel_3->setText(buslineArray[2]);
            break;
        case lineLabel_4:
            if(buslineArray[3] == QString())
                buslineArray[3] = "0";
            else if(buslineArray[3] == "-")
                buslineArray[3] = QString();
            else if(buslineArray[3] == "9")
                buslineArray[3] = "-";
            else if((buslineArray[3] >= "0")&&(buslineArray[3] < "9"))
            {
                itemp = buslineArray[3].toInt();
                buslineArray[3] = QString::number(itemp+1, 10);
            }
            else
                buslineArray[3] = "0";
            ui->IDlabel_4->setText(buslineArray[3]);
            break;
        case lineLabel_5:
            if(buslineArray[4] == QString())
                buslineArray[4] = "0";
            else if(buslineArray[4] == "-")
                buslineArray[4] = QString();
            else if(buslineArray[4] == "9")
                buslineArray[4] = "-";
            else if((buslineArray[4] >= "0")&&(buslineArray[4] < "9"))
            {
                itemp = buslineArray[4].toInt();
                buslineArray[4] = QString::number(itemp+1, 10);
            }
            else
                buslineArray[4] = "0";
            ui->IDlabel_5->setText(buslineArray[4]);
            break;
        case lineLabel_6:
            if(buslineArray[5] == QString())
                buslineArray[5] = "0";
            else if(buslineArray[5] == "-")
                buslineArray[5] = QString();
            else if(buslineArray[5] == "9")
                buslineArray[5] = "-";
            else if((buslineArray[5] >= "0")&&(buslineArray[5] < "9"))
            {
                itemp = buslineArray[5].toInt();
                buslineArray[5] = QString::number(itemp+1, 10);
            }
            else
                buslineArray[5] = "0";
            ui->IDlabel_6->setText(buslineArray[5]);
            break;
        case lineLabel_7:
            if(buslineArray[6] == QString())
                buslineArray[6] = "0";
            else if(buslineArray[6] == "-")
                buslineArray[6] = QString();
            else if(buslineArray[6] == "9")
                buslineArray[6] = "-";
            else if((buslineArray[6] >= "0")&&(buslineArray[6] < "9"))
            {
                itemp = buslineArray[6].toInt();
                buslineArray[6] = QString::number(itemp+1, 10);
            }
            else
                buslineArray[6] = "0";
            ui->IDlabel_7->setText(buslineArray[6]);
                break;
        case lineLabel_8:
            if(buslineArray[7] == QString())
                buslineArray[7] = "0";
            else if(buslineArray[7] == "9")
                buslineArray[7] = QString();
            else if((buslineArray[7] >= "0")&&(buslineArray[7] < "9"))
            {
                itemp = buslineArray[7].toInt();
                buslineArray[7] = QString::number(itemp+1, 10);
            }
            else
                buslineArray[7] = "0";
            ui->IDlabel_8->setText(buslineArray[7]);
            break;
        case ticketLabel_1:
            if(ticketArray[0] == QString())
                ticketArray[0] = "0";
            else if(ticketArray[0] == "9")
                    ticketArray[0] = QString();
            else if((ticketArray[0] >= "0")&&(ticketArray[0] < "9"))
            {
                itemp = ticketArray[0].toInt();
                ticketArray[0] = QString::number(itemp+1, 10);
            }
            else
                ticketArray[0] = "0";
            ui->IDlabel_9->setText(ticketArray[0]);
            break;
        case ticketLabel_2:
            if(ticketArray[1] == "9")
                ticketArray[1] = "0";
            else if((ticketArray[1] >= "0")&&(ticketArray[1] < "9"))
            {
                itemp = ticketArray[1].toInt();
                ticketArray[1] = QString::number(itemp+1, 10);
            }
            else
                ticketArray[1] = "0";
            ui->IDlabel_10->setText(ticketArray[1]);
            break;
        case ticketLabel_3:
            if(ticketArray[2] != ".")
                ticketArray[2] = ".";
            ui->IDlabel_11->setText(ticketArray[2]);
            break;
        case ticketLabel_4:
            if(ticketArray[3] == "9")
                ticketArray[3] = "0";
            else if((ticketArray[3] >= "0")&&(ticketArray[3] < "9"))
            {
                itemp = ticketArray[3].toInt();
                ticketArray[3] = QString::number(itemp+1, 10);
            }
            else
                ticketArray[3] = "0";
            ui->IDlabel_12->setText(ticketArray[3]);
            break;
        case ticketLabel_5:
            if(ticketArray[4] == "9")
                ticketArray[4] = "0";
            else if((ticketArray[4] >= "0")&&(ticketArray[4] < "9"))
            {
                itemp = ticketArray[4].toInt();
                ticketArray[4] = QString::number(itemp+1, 10);
            }
            else
                ticketArray[4] = "0";
            ui->IDlabel_13->setText(ticketArray[4]);
            break;
    }
}

void buslineWidget::keyboardRightpress()
{
    qDebug("kira --- process bus line and ticket right key press...");
    switch(myIDLabel)
    {
        case lineLabel_1:
            myIDLabel = lineLabel_2;
            ui->IDlabel_1->setStyleSheet(imageUncheck);
            ui->IDlabel_2->setStyleSheet(imageCheck);
            break;
        case lineLabel_2:
            myIDLabel = lineLabel_3;
            ui->IDlabel_2->setStyleSheet(imageUncheck);
            ui->IDlabel_3->setStyleSheet(imageCheck);
            break;
        case lineLabel_3:
            myIDLabel = lineLabel_4;
            ui->IDlabel_3->setStyleSheet(imageUncheck);
            ui->IDlabel_4->setStyleSheet(imageCheck);
            break;
        case lineLabel_4:
            myIDLabel = lineLabel_5;
            ui->IDlabel_4->setStyleSheet(imageUncheck);
            ui->IDlabel_5->setStyleSheet(imageCheck);
            break;
        case lineLabel_5:
            myIDLabel = lineLabel_6;
            ui->IDlabel_5->setStyleSheet(imageUncheck);
            ui->IDlabel_6->setStyleSheet(imageCheck);
            break;
        case lineLabel_6:
            myIDLabel = lineLabel_7;
            ui->IDlabel_6->setStyleSheet(imageUncheck);
            ui->IDlabel_7->setStyleSheet(imageCheck);
            break;
        case lineLabel_7:
            myIDLabel = lineLabel_8;
            ui->IDlabel_7->setStyleSheet(imageUncheck);
            ui->IDlabel_8->setStyleSheet(imageCheck);
            break;
        case lineLabel_8:
            myIDLabel = ticketLabel_1;
            ui->IDlabel_8->setStyleSheet(imageUncheck);
            ui->IDlabel_9->setStyleSheet(imageCheck);
            break;
        case ticketLabel_1:
            myIDLabel = ticketLabel_2;
            ui->IDlabel_9->setStyleSheet(imageUncheck);
            ui->IDlabel_10->setStyleSheet(imageCheck);
            break;
        case ticketLabel_2:
            myIDLabel = ticketLabel_3;
            ui->IDlabel_10->setStyleSheet(imageUncheck);
            ui->IDlabel_11->setStyleSheet(imageCheck);
            break;
        case ticketLabel_3:
            myIDLabel = ticketLabel_4;
            ui->IDlabel_11->setStyleSheet(imageUncheck);
            ui->IDlabel_12->setStyleSheet(imageCheck);
            break;
        case ticketLabel_4:
            myIDLabel = ticketLabel_5;
            ui->IDlabel_12->setStyleSheet(imageUncheck);
            ui->IDlabel_13->setStyleSheet(imageCheck);
            break;
        case ticketLabel_5:
            myIDLabel = lineLabel_1;
            ui->IDlabel_13->setStyleSheet(imageUncheck);
            ui->IDlabel_1->setStyleSheet(imageCheck);
            break;
    }
}

void buslineWidget::keyboardShortpress()
{
    busline = QString();
    for(int i=0; i<8; i++)
        busline.append(buslineArray[i]);
    ticket = QString();
    for(int i=0; i<5; i++)
        ticket.append(ticketArray[i]);
    initLabelState();
    qDebug(qPrintable(busline));
    qDebug(qPrintable(ticket));
    emit checkConfirm();
}

void buslineWidget::initLabelState()
{
    myIDLabel = lineLabel_1;
    ui->IDlabel_1->setStyleSheet(imageCheck);
    ui->IDlabel_2->setStyleSheet(imageUncheck);
    ui->IDlabel_3->setStyleSheet(imageUncheck);
    ui->IDlabel_4->setStyleSheet(imageUncheck);
    ui->IDlabel_5->setStyleSheet(imageUncheck);
    ui->IDlabel_6->setStyleSheet(imageUncheck);
    ui->IDlabel_7->setStyleSheet(imageUncheck);
    ui->IDlabel_8->setStyleSheet(imageUncheck);

    ui->IDlabel_9->setStyleSheet(imageUncheck);
    ui->IDlabel_10->setStyleSheet(imageUncheck);
    ui->IDlabel_11->setStyleSheet(imageUncheck);
    ui->IDlabel_12->setStyleSheet(imageUncheck);
    ui->IDlabel_13->setStyleSheet(imageUncheck);
}
