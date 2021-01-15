#include "widget.h"
#include "ui_widget.h"
#include <QList>
#include <QNetworkInterface>
#include <QDebug>
#include <QNetworkDatagram>

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    setWindowTitle("client");

    initClient();
}

Widget::~Widget()
{
    udpSocket->abort();     //关闭套接字，并丢弃写缓冲区中的所有待处理数据
    delete ui;
}

/**
 * @brief Widget::initClient
 * 初始化client
 */
void Widget::initClient()
{
    udpSocket = new QUdpSocket(this);   //创建udp socket对象

    //获取本机IP
    QList<QHostAddress> ipAddressList = QNetworkInterface::allAddresses();
    qDebug()<<"ip list:" << ipAddressList;

    //下拉框切换
    connect(ui->comboBox,QOverload<int>::of(&QComboBox::currentIndexChanged),
            [=](int index){
        switch (index) {
        case 0:
            ui->editLocalAddress->setText(ipAddressList.at(1).toString());
            ui->editPeerAddress->setText(ipAddressList.at(1).toString());
            break;
        case 1:
            ui->editLocalAddress->setText("Any");
            ui->editPeerAddress->setText("Broadcast");
            break;
        case 2:
            ui->editLocalAddress->setText("224.0.0.2");
            ui->editPeerAddress->setText("224.0.0.2");
            break;
        }
    });

    ui->editLocalAddress->setText(ipAddressList.at(1).toString());
    ui->editPeerAddress->setText(ipAddressList.at(1).toString());

    // 点击绑定端口，根据ui设置进行绑定
    connect(ui->btnBind,&QPushButton::clicked,[this]{
        //判断当前是否已经绑定，bind了就取消
        if(udpSocket->state() == QAbstractSocket::BoundState){
            //关闭套接字，并丢弃写缓冲区中的所有待处理数据
            udpSocket->abort();
        }else if(udpSocket->state()==QAbstractSocket::UnconnectedState){
            //从界面读取IP地址和端口
            const QHostAddress address=QHostAddress(ui->editLocalAddress->text());
            const unsigned short port=ui->editLocalPort->text().toUShort();

            //绑定本机地址
            //combobox::单播 广播 组播
            switch (ui->comboBox->currentIndex()) {
            case 0:
                //可以指定本地绑定的ip
                udpSocket->bind(address,port);
                //udpSocket->bind(port);
                break;
            case 1:
                //可以指定本地绑定的ip
                //udpSocket->bind(port);
                udpSocket->bind(QHostAddress::AnyIPv4,port);
                break;
            case 2:
                //组播ip必须是D类IP
                //D类IP端 224.0.0.0 到 239.255.255.255
                //且组播地址不能是244.0.0.1
                udpSocket->bind(QHostAddress::AnyIPv4,port);    //根据Qt示例，组播的话IPv4和v6分开的
                udpSocket->joinMulticastGroup(address);     //QHostAddress("224.0.0.2")
                break;
            default:
                break;
            }
        }else {
            ui->txtRecv->append("It is not BoundState or UnconnectedState..");
        }

    });

    //绑定状态改变
    connect(udpSocket,&QUdpSocket::stateChanged,[this](QAbstractSocket::SocketState socketState){
        //已绑定就设置为不可编辑
        const bool is_bind=(socketState==QAbstractSocket::BoundState);
        ui->btnBind->setText(is_bind?"Disbind":"Bind");
        ui->editLocalAddress->setEnabled(!is_bind);
        ui->editLocalPort->setEnabled(!is_bind);
        ui->editPeerAddress->setEnabled(!is_bind);
        ui->editPeerPort->setEnabled(!is_bind);

        updateState();
    });

    //发送数据
    connect(ui->btnSend,&QPushButton::clicked,[this]{
        //判断是可操作，isValid表示准备好读写
        if(!udpSocket->isValid())
            return;

        //将发送区文本发送给客户端
        const QByteArray send_data=ui->txtSend->toPlainText().toUtf8();

        //数据为空就返回
        if(send_data.isEmpty())
            return;

        //从界面上读取ip和端口
        const QString address_text=ui->editPeerAddress->text();
        const QHostAddress address=QHostAddress(address_text);
        const unsigned short port=ui->editPeerAddress->text().toUShort();

        //combobox：单播 广播 组播
        switch (ui->comboBox->currentIndex()) {
        case 0:
            udpSocket->writeDatagram(QNetworkDatagram(send_data,address,port));
            break;
        case 1:
            udpSocket->writeDatagram(QNetworkDatagram(send_data,QHostAddress::Broadcast,port));
            break;
        case 2:
            udpSocket->writeDatagram(QNetworkDatagram(send_data,address,port)); //QHostAddress("224.0.0.2")
            break;
        default:
            break;
        }
    });

    //收到数据，触发readyRead
    connect(udpSocket,&QUdpSocket::readyRead,[this]{
        //没有可读的数据就返回
        if(!udpSocket->hasPendingDatagrams() ||
                udpSocket->pendingDatagramSize()<=0)
            return;
        //注意收发两端文本要使用对应的编解码
        QNetworkDatagram recv_datagram=udpSocket->receiveDatagram();
        const QString recv_text=QString::fromUtf8(recv_datagram.data());
        ui->txtRecv->append(QString("[%1:%2]")
                            .arg(recv_datagram.senderAddress().toString())
                            .arg(recv_datagram.senderPort()));
        ui->txtRecv->append(recv_text);
    });

    //error信号
#if QT_VERSION < QT_VERSION_CHECK(5,15,0)
    //错误信息
    connect(udpSocket,static_cast<void(QAbstractSocket::*)
            (QAbstractSocket::SocketError)>(&QAbstractSocket::error),
            [this](QAbstractSocket::SocketError){
        ui->txtRecv->append("Socket Error:"+udpSocket->errorString());
    });
#else
    //错误信息
    connect(udpSocket,&QAbstractSocket::errorOccurred,
            [this](QAbstractSocket::SocketError){
        ui->txtRecv->append("Socket Error:"+udpSocket->errorString());
#endif
}

/**
 * @brief Widget::updateState
 * 更新当前状态
 */
void Widget::updateState()
{
    //将当前socket绑定的地址和端口写在标题栏
    if(udpSocket->state()==QAbstractSocket::BoundState){
        setWindowTitle(QString("Client[%1:%2]")
                       .arg(udpSocket->localAddress().toString())
                       .arg(udpSocket->localPort()));
    }else{
        setWindowTitle("Client");   //标题
    }
}

