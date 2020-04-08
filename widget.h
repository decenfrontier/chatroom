#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent,QString name);
    ~Widget();
    void closeEvent(QCloseEvent *);

private:
    Ui::Widget *ui;

signals:
    void closeWidget();   // 关闭窗口发送信号

};
#endif // WIDGET_H
