#include "blackclearwidget.h"
#include "ui_blackclearwidget.h"

blackClearWidget::blackClearWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::blackClearWidget)
{
    ui->setupUi(this);
}

blackClearWidget::~blackClearWidget()
{
    delete ui;
}
