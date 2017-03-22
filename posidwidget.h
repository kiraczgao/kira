#ifndef POSIDWIDGET_H
#define POSIDWIDGET_H

#include <QWidget>
#include <QString>
#include "datastructure.h"

namespace Ui {
class posidWidget;
}

class posidWidget : public QWidget
{
    Q_OBJECT

public:
    explicit posidWidget(QWidget *parent = 0);
    ~posidWidget();

private:
    Ui::posidWidget *ui;

public:
    void setPosID(QString sid);
    void getPosID(QString &gid) { gid = posid; }
    void keyboardLeftpress();
    void keyboardRightpress();
    void keyboardShortpress();
    void initLabelState();

signals:
    void checkConfirm();

public:
    QString posid;
    idLabelState myIDLabel;
    QString posidArray[8];
};

#endif // POSIDWIDGET_H
