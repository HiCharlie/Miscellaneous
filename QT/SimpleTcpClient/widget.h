#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTcpServer>
#include <QTcpSocket>

//QT_BEGIN_NAMESPACE
namespace Ui {
class Widget;
}
//QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = nullptr);
    ~Widget();

private:
    //初始化Client操作
    void initClient();

    //更新当前状态
    void updateState();

private:
    Ui::Widget *ui;

    //socket对象
    QTcpSocket *client;

};

#endif // WIDGET_H
