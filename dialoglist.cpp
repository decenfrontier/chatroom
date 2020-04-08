#include "dialoglist.h"
#include "ui_dialoglist.h"
#include "widget.h"
#include <QToolButton>
#include <QMessageBox>

DialogList::DialogList(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DialogList)
{
    ui->setupUi(this);

    // 设置图标
    setWindowIcon(QIcon(":/pic/images/qq.png"));


    // 准备名称
    QStringList nameList;
    nameList << "水票奇缘" << "忆梦如澜" <<"北京出版人"<<"Cherry"<<"淡然"
             <<"娇娇girl"<<"落水无痕"<<"青墨暖暖";
    // 准备图标
    QStringList iconNameList; //图标资源列表
    iconNameList << "spqy"<< "ymrl" <<"qq" <<"Cherry"<< "dr"
                     <<"jj"<<"lswh"<<"qmnn";


    QVector<QToolButton*> vToolBtn; // 添加容器记住这些button的指针

    // 添加用户
    for (int i = 0; i < 8; ++i) {
        QToolButton* btn = new QToolButton(this);
        // 设置文字
        btn->setText(nameList[i]);
        // 设置头像
        QString str = QString(":/pic/images/%1.png").arg(iconNameList.at(i));
        btn->setIcon(QIcon(str));

        btn->setIconSize(QSize(64,64)); // 设置头像大小
        btn->setAutoRaise(true);    // 设置按钮风格 透明
        btn->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);  // 设置文字在图片右边
        ui->vLayout->addWidget(btn);    // 添加到垂直布局中
        vToolBtn.push_back(btn);
        vIsShow.push_back(false);
    }

    // 对8个按钮添加信号槽
    for (int i = 0; i < vToolBtn.size(); ++i)
    {
        connect(vToolBtn[i],&QToolButton::clicked,[=](){
            if(vIsShow[i] == true)
            {
                QString str = QString("%1 窗口已经打开").arg(vToolBtn[i]->text());
                QMessageBox::warning(this,"警告",str);
                return;
            }
            vIsShow[i] = true;
            Widget* chatWnd = new Widget(0,vToolBtn[i]->text());    // 顶层弹出 窗口名字
            chatWnd->setWindowIcon(vToolBtn[i]->icon());
            chatWnd->show();

            connect(chatWnd,&Widget::closeWidget,[=](){ // 关闭窗口,显示置0
                vIsShow[i] = false;
            });
        });
    }
}

DialogList::~DialogList()
{
    delete ui;
}
