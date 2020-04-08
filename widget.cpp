#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent,QString name)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    setWindowTitle(name);
}

Widget::~Widget()
{
    delete ui;
}

void Widget::closeEvent(QCloseEvent*)
{
    emit closeWidget();
}
