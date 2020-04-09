#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QUdpSocket>

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

public:
    enum MsgType {Msg,UsrEnter,UsrLeft};
    void sndMsg(MsgType type); // 广播UDP消息
    void usrEnter(QString userName);    // 处理新用户加入
    void usrLeft(QString userName);     // 处理用户离开
    QString getUsrName();   // 获取用户名
    QString getMsg();       // 获取聊天信息
private:
    QUdpSocket* udpSocket;  // Udp套接字
    qint16 port;            // 端口
    QString uName;          // 用户名
    void recvMsg();         // 接收Udp消息
};
#endif // WIDGET_H
