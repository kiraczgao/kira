#ifndef BLACKCLEARWIDGET_H
#define BLACKCLEARWIDGET_H

#include <QWidget>

namespace Ui {
class blackClearWidget;
}

class blackClearWidget : public QWidget
{
    Q_OBJECT

public:
    explicit blackClearWidget(QWidget *parent = 0);
    ~blackClearWidget();

private:
    Ui::blackClearWidget *ui;
};

#endif // BLACKCLEARWIDGET_H
