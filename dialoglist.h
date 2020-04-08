#ifndef DIALOGLIST_H
#define DIALOGLIST_H

#include <QWidget>


namespace Ui {
class DialogList;
}

class DialogList : public QWidget
{
    Q_OBJECT

public:
    explicit DialogList(QWidget *parent = nullptr);
    ~DialogList();


private:
    Ui::DialogList *ui;
    QVector<bool> vIsShow;   // 记录每个窗口是否显示
};

#endif // DIALOGLIST_H
