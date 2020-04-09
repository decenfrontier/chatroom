#include "widget.h"
#include "ui_widget.h"
#include <QDataStream>
#include <QMessageBox>
#include <QDateTime>

Widget::Widget(QWidget *parent,QString name)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    // 初始化操作
    udpSocket = new QUdpSocket(this);   // 套接字
    uName = name;   // 用户名
    port = 9999;    // 端口号
    /*
     * ShareAddress模式, 允许不同的服务连接到相同的地址和端口,常用于多客户端监听同一个服务器端口
     * ReuseAddressHint模式, 断线重新连接服务器
     */
    udpSocket->bind(port,QUdpSocket::ShareAddress|QUdpSocket::ReuseAddressHint);  // 绑定端口号
    //sndMsg(UsrEnter);   // 发送新用户进入
    // 点击发送按钮发送消息
    connect(ui->Btn_Send,&QPushButton::clicked,[=](){
        sndMsg(Msg);
    });
    // 监听别人发送的数据
    connect(udpSocket,&QUdpSocket::readyRead,this,&Widget::recvMsg);
}

Widget::~Widget()
{
    delete ui;
}

void Widget::closeEvent(QCloseEvent*)
{
    emit closeWidget();
}

void Widget::sndMsg(Widget::MsgType type)
{
    // 发送的消息分为3种类型
    // 发送的数据分3段处理. 第一段,类型;   第二段,用户名;    第三段,具体内容
    QByteArray array;
    QDataStream stream(&array,QIODevice::WriteOnly);
    stream << type << getUsrName(); // 类型 和 用户名, 添加到流中

    switch (type) {
    case Msg:       // 普通消息发送
        if(ui->msgTxtEdit->toPlainText() == "")
        {   // 如果用户没有输入内容,不发消息
            QMessageBox::warning(this,"警告","发送内容不能为空");
            return;
        }
        stream << getMsg(); // 具体内容, 添加到流中
        break;
    case UsrEnter:

        break;

    case UsrLeft:

        break;
    default:
        break;
    }
    // 书写报文
    udpSocket->writeDatagram(array,QHostAddress::Broadcast,port);
}

QString Widget::getUsrName()
{
    return uName;
}

QString Widget::getMsg()
{
    QString str = ui->msgTxtEdit->toHtml();
    ui->msgTxtEdit->clear();        // 清空输入框
    ui->msgTxtEdit->setFocus();
    return str;
}

void Widget::recvMsg()
{
    // 获取报文长度
    qint64 size = udpSocket->pendingDatagramSize();
    // 读取报文
    QByteArray array = QByteArray(size,0);
    udpSocket->readDatagram(array.data(),size);
    // 解析数据
    QDataStream stream(&array,QIODevice::ReadOnly); // 从array中读取数据到stream中
    // 定义接收三段数据的三个变量
    int msgType;
    QString usrName;
    QString msg;
    stream >> msgType;  // 从stream中取出 类型
    // 获取当前时间
    QString time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh-mm-ss");
    switch (msgType) {
    case Msg:
        stream >> usrName >> msg;   // 从stream中取出 用户名 和 具体内容
        // 追加聊天记录
        ui->msgBrowser->setTextColor(Qt::blue); // 设置聊天框文本颜色
        ui->msgBrowser->append("[" + usrName + "]" + time);
        ui->msgBrowser->append(msg);
        break;
    case UsrEnter:

        break;

    case UsrLeft:

        break;
    default:
        break;
    }
}
