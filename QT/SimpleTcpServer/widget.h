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
    //初始化Server操作
    void initServer();

    //close Server
    void closeServer();

    //更新当前状态
    void updateState();

private:
    Ui::Widget *ui;
    //server用于监听端口，获取新的tcp连接描述符
    QTcpServer *server;
    //储存已连接的socket对象
    QList<QTcpSocket *> clientList;
};

#endif // WIDGET_H
