#include "dialoglist.h"
#include "ui_dialoglist.h"
#include <QToolButton>

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

    // 添加容器记住这些button的指针
    QVector<QToolButton*> vToolBtn;
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
    }
}

DialogList::~DialogList()
{
    delete ui;
}
