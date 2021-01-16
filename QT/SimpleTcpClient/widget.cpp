#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    setWindowTitle("Tcp Server");

    initClient();
}

Widget::~Widget()
{
    //析构关闭连接
    //client->disconnectFromHost();
    //if(client->state()!=QAbstractSocket::UnconnectedState){
    //    client->waitForDisconnected();
    //}
    //关闭套接字的I/O设备，并调用disconnectFromHost()关闭套接字的连接。
    //client->close();
    //中止当前连接并重置套接字。与disconnectFromHost()不同，
    //此函数立即关闭套接字，并丢弃写缓冲区中的所有待处理数据。
    client->close();
    delete ui;
}

/**
 * @brief Widget::initServer
 * 初始化服务器端
 */
void Widget::initClient()
{
    //client
    client = new QTcpSocket(this);

    //点击连接按钮
    connect(ui->btnConnect,&QPushButton::clicked,[this]{
        //判断当前是否已经连接
        if(client->state()==QTcpSocket::ConnectedState){
            //如果已经连接，断开连接
            //如果使用disconnectFromHost()不会重置套接字，isValid还是会为true
            client->abort();
        }else if(client->state()==QAbstractSocket::UnconnectedState) {
            //从界面上读取ip和端口
            const QHostAddress address=QHostAddress(ui->editAddress->text());
            const unsigned short port=ui->editPort->text().toUShort();
            //连接服务器
            client->connectToHost(address,port);
        }else {
            ui->editRecv->append("It is not connectedState or UnconnectedStatae");
        }
    });

    //连接状态
    connect(client,&QTcpSocket::connected,[this]{
        //已连接就设置为不可编辑
        ui->btnConnect->setText("Disconnect");
        ui->editAddress->setEnabled(false);
        ui->editPort->setEnabled(false);

        updateState();
    });

    //断开连接
    connect(client,&QTcpSocket::disconnected,[this]{
        //断开连接状态还原
        ui->btnConnect->setText("Connect");
        ui->editAddress->setEnabled(true);
        ui->editPort->setEnabled(true);

        updateState();
    });

    //发送按钮
    connect(ui->btnSend,&QPushButton::clicked,[this]{
        //判断是否可操作，isvalid表示准备好读写
        if(!client->isValid())
            return;

        //将发送区文本发送给客户端
        const QByteArray send_data=ui->editSend->toPlainText().toUtf8();
        //数据为空将返回
        if(send_data.isEmpty()){
            return;
        }

        client->write(send_data);
        //client->waitForBytesWritten();
    });

    //数据接收，触发readyread
    connect(client,&QTcpSocket::readyRead,[this]{
        //没有可读的数据就返回
        if(client->bytesAvailable()<=0)
            return;

        //注意收发两端文本要使用对应的编解码
        const QString recv_text=QString::fromUtf8(client->readAll());
        ui->editRecv->append(QString("[%1:%2]")
                             .arg(client->peerAddress().toString()
                                  .arg(client->peerPort())));

        ui->editRecv->append(recv_text);
    });

            //error信号 5.15换名字
#if QT_VERSION < QT_VERSION_CHECK(5,15,0)

    //错误信息
    connect(client, static_cast<void(QAbstractSocket::*)(QAbstractSocket::SocketError)>(&QAbstractSocket::error),
            [this](QAbstractSocket::SocketError){
        ui->editRecv->append(QString("Socket Error:")+ client->errorString());
    });
#else
    //错误信息
    connect(client,&QAbstractSocket::errorOccurred,[this](QAbstractSocket::SocketError){
        ui->textRecv->append("Socket Error:"+client->errorString());
    });
            });
#endif

}

void Widget::updateState()
{
    //当前client地址和端口、客户端连接数写入标题栏
    if(client->state()==QAbstractSocket::ConnectedState){
        setWindowTitle(QString("Tcp Client[%1:%2]")
                       .arg(client->localAddress().toString())
                       .arg(client->localPort()));
    }else {
        setWindowTitle("Tcp Client");
    }
}
