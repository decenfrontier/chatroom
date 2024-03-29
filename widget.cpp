#include "widget.h"
#include "ui_widget.h"
#include <QDataStream>
#include <QMessageBox>
#include <QDateTime>
#include <QColorDialog>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>

Widget::Widget(QWidget *parent,QString name)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    // 初始化操作
    udpSocket = new QUdpSocket(this);   // 套接字
    uName = name;   // 用户名
    port = 9999;    // 端口号,所有聊天窗口使用同一端口
    /*
     * ShareAddress模式, 允许不同的服务连接到相同的地址和端口,常用于多客户端监听同一个服务器端口
     * ReuseAddressHint模式, 断线重新连接服务器
     */
    udpSocket->bind(port,QUdpSocket::ShareAddress|QUdpSocket::ReuseAddressHint);  // 绑定端口号
    // 发送新用户进入
    sndMsg(UsrEnter);
    // 点击发送按钮发送消息
    connect(ui->Btn_Send,&QPushButton::clicked,[=](){
        sndMsg(Msg);
        ui->msgTxtEdit->setFocus();
    });
    // 监听别人发送的数据
    connect(udpSocket,&QUdpSocket::readyRead,this,&Widget::recvMsg);
    // 点击退出按钮关闭群聊窗口
    connect(ui->Btn_Exit,&QPushButton::clicked,[=](){
        close();
    });

    //----------------------辅助功能------------------------//
    // 字体
    connect(ui->Cmb_font,&QFontComboBox::currentFontChanged,[=](const QFont &font){
        ui->msgTxtEdit->setCurrentFont(font);
        ui->msgTxtEdit->setFocus();
    });
    // 字号
    void (QComboBox::*pF)(const QString &) = &QComboBox::currentIndexChanged;
    connect(ui->Cmb_Size,pF,[=](const QString &text){
        ui->msgTxtEdit->setFontPointSize(text.toDouble());
        ui->msgTxtEdit->setFocus();
    });
    // 加粗
    connect(ui->TBtn_Bold,&QToolButton::clicked,[=](bool isCheck){
        if(isCheck)
            ui->msgTxtEdit->setFontWeight(QFont::Bold);
        else
            ui->msgTxtEdit->setFontWeight(QFont::Normal);
    });
    // 倾斜
    connect(ui->TBtn_italic,&QToolButton::clicked,[=](bool isCheck){
        ui->msgTxtEdit->setFontItalic(isCheck);
    });
    // 下划线
    connect(ui->TBtn_UnderLine,&QToolButton::clicked,[=](bool isCheck){
        ui->msgTxtEdit->setFontUnderline(isCheck);
    });
    // 字体颜色
    connect(ui->TBtn_Color,&QToolButton::clicked,[=](){
        QColor color = QColorDialog::getColor(Qt::black); // 设置默认为黑色
        ui->msgTxtEdit->setTextColor(color);
    });
    // 清空聊天记录
    connect(ui->TBtn_Clear,&QToolButton::clicked,[=](){
        ui->msgBrowser->clear();
    });
    // 保存聊天记录
    connect(ui->TBtn_Save,&QToolButton::clicked,[=](){
        if(ui->msgBrowser->document()->isEmpty())
        {
            QMessageBox::warning(this,"警告","内容不能为空");
            return;
        }
        QString path = QFileDialog::getSaveFileName(this,"请选择保存聊天记录的位置","保存的文件名","*.txt");
        if(path.isEmpty())
        {
            QMessageBox::warning(this,"警告","路径不能为空");
            return;
        }
        QFile file(path);
        file.open(QIODevice::WriteOnly | QIODevice::Text);  // 打开模式设置为 只写 和 换行
        QTextStream stream(&file);
        stream << ui->msgBrowser->toPlainText();
        file.close();
    });
}

Widget::~Widget()
{
    delete ui;
}

void Widget::closeEvent(QCloseEvent*)
{
    emit closeWidget();
    sndMsg(UsrLeft);
    // 断开套接字
    udpSocket->close();
    udpSocket->destroyed();
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

void Widget::usrEnter(QString userName)
{
    bool isEmpty = ui->usrTblWidget->findItems(userName,Qt::MatchExactly).isEmpty();
    if(isEmpty)
    {
        // 更新右侧tableWidget
        QTableWidgetItem* usr = new QTableWidgetItem(userName);
        ui->usrTblWidget->insertRow(0); // 插入项
        ui->usrTblWidget->setItem(0,0,usr);
        // 追加聊天记录
        ui->msgBrowser->setTextColor(Qt::gray);
        ui->msgBrowser->append(QString("%1 上线了").arg(userName));
        // 在线人数更新
        ui->Lbl_usrNum->setText(QString("在线人数: %1人").arg(ui->usrTblWidget->rowCount()));
        // 把自身信息广播出去
        sndMsg(UsrEnter);
    }
}

void Widget::usrLeft(QString userName,QString time)
{
    bool isEmpty = ui->usrTblWidget->findItems(userName,Qt::MatchExactly).isEmpty();
    if(!isEmpty)
    {
        // 更新右侧tableWidget
        int row = ui->usrTblWidget->findItems(userName,Qt::MatchExactly).first()->row();
        ui->usrTblWidget->removeRow(row);
        // 追加聊天记录
        ui->msgBrowser->setTextColor(Qt::gray);
        ui->msgBrowser->append(userName + "于" + time + "退出了群聊");
        // 在线人数更新
        ui->Lbl_usrNum->setText(QString("在线人数:%1人").arg(ui->usrTblWidget->rowCount()));
    }
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
    QString time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    switch (msgType) {
    case Msg:   // 普通聊天
    {
        stream >> usrName >> msg;   // 从stream中取出 用户名 和 具体内容
        // 追加聊天记录
        ui->msgBrowser->setTextColor(Qt::blue); // 设置聊天框文本颜色
        ui->msgBrowser->append("[" + usrName + "]" + time);
        ui->msgBrowser->append(msg);
    }
        break;

    case UsrEnter:  // 用户进入
    {
        stream >> usrName;  // 从stream中取出 用户名
        usrEnter(usrName);
    }
        break;

    case UsrLeft:   // 用户离开
    {
        stream >> usrName;  // 从stream中取出 用户名
        usrLeft(usrName,time);
    }
        break;
    default:
        break;
    }
}
