#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QUdpSocket>

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = nullptr);
    ~Widget();
private:
    void initClient();      //初始化client
    void updateState();     //更新当前状态
private:
    Ui::Widget *ui;
    QUdpSocket *udpSocket;      // socket对象
};

#endif // WIDGET_H
