#ifndef SIGNALWIDGET_H
#define SIGNALWIDGET_H

#include <QWidget>

class QTimer;
namespace Ui {
class signalWidget;
}

class signalWidget : public QWidget
{
    Q_OBJECT

public:
    explicit signalWidget(QWidget *parent = 0);
    ~signalWidget();

private:
    Ui::signalWidget *ui;

public slots:
    void onTimerOut();

public:
    QTimer *myTimer;
};

#endif // SIGNALWIDGET_H
