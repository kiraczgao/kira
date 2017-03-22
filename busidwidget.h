#ifndef BUSIDWIDGET_H
#define BUSIDWIDGET_H

#include <QWidget>
#include <QString>
#include "datastructure.h"

namespace Ui {
class busidWidget;
}

class busidWidget : public QWidget
{
    Q_OBJECT

public:
    explicit busidWidget(QWidget *parent = 0);
    ~busidWidget();

public:
    void setBusID(QString sid);
    void getBusID(QString &gid) { gid = busid; }
    void keyboardLeftpress();
    void keyboardRightpress();
    void keyboardShortpress();
    void initLabelState();

signals:
    void checkConfirm();

private:
    Ui::busidWidget *ui;

public:
    QString busid;
    idLabelState myIDLabel;
    QString busidArray[8];
};

#endif // BUSIDWIDGET_H
