#include "widget.h"
#include "ui_widget.h"

#include <QUrlQuery>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>

#include <QDebug>

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    //创建QNetworkAccessManager
    manager = new QNetworkAccessManager(this);

    //查看支持的协议 ("ftp", "file", "qrc", "http", "data") 没有https
    qDebug()<<manager->supportedSchemes();

    //QNetworkAccessManager是异步的
    connect(manager,&QNetworkAccessManager::finished,
            this,&Widget::receiveReply);
    //测试get
    connect(ui->btnGet,&QPushButton::clicked,this,&Widget::getTest);
    //测试post
    connect(ui->btnPost,&QPushButton::clicked,this,&Widget::postTest);
    //测试form
    connect(ui->btnForm,&QPushButton::clicked,this,&Widget::formTest);

    //定时器验证是否阻塞界面
    connect(&timer,&QTimer::timeout,this,[this]{
        ui->editTime->setTime(QTime::currentTime());
    });
    timer.start(1000);  //1s定时
}

Widget::~Widget()
{
    delete ui;
}

void Widget::getTest()
{
    //构建请求对象
    QNetworkRequest request;
    request.setUrl(QUrl(ui->editGetUrl->text()));

    //设置请求头
    //已知头类型参考文件QNetworkRequest::KnownHeaders
    request.setHeader(QNetworkRequest::ContentTypeHeader,"application/json");
    //自定义类型
    request.setRawHeader("self-header","gongjianbo");

    //发送请求
    manager->get(request);

    //可以开启一个局部的事件循环，等待响应，不会阻塞线程
    //QNetworkReply *reply=manager->get(request);
    //QEventLoop eventLoop;
    //connect(manager, &QNetworkAccessManager::finished, &eventLoop, &QEventLoop::quit);
    //eventLoop.exec();
    //receiveReply(reply)
}

void Widget::postTest()
{
    //构建请求对象
    QNetworkRequest request;
    request.setUrl(QUrl(ui->editPostUrl->text()));

    //设置请求头
    request.setHeader(QNetworkRequest::ContentTypeHeader,"application/json");

    //发送请求
    manager->post(request,ui->editPostData->toPlainText().toUtf8());
}

void Widget::formTest()
{
    //构建一个新的Manager
    QNetworkAccessManager manager2;
    //构建一个multiPart用于提交表单
    //注意：multiPart请在请求完成后在删除
    QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

    //文本内容
    QHttpPart namePart;
    //Content-Type对照表详情百度http://tool.oschina.net/commons/
    namePart.setHeader(QNetworkRequest::ContentTypeHeader,QVariant("text/plain"));
    namePart.setHeader(QNetworkRequest::ContentDispositionHeader,QVariant("form-data; name=\"thename\";"));
    namePart.setBody("HiCharlie");

    QHttpPart agePart;
    agePart.setHeader(QNetworkRequest::ContentTypeHeader,QVariant("text/plain"));
    agePart.setHeader(QNetworkRequest::ContentDispositionHeader,QVariant("form-data; age=\"theage\";"));
    agePart.setBody("27");

    multiPart->append(namePart);
    multiPart->append(agePart);


    //文件内容
    QHttpPart filePart;
    agePart.setHeader(QNetworkRequest::ContentTypeHeader,QVariant("text/plain"));
    //需要有filename的文件，不然提交不成功
    agePart.setHeader(QNetworkRequest::ContentDispositionHeader,QVariant("form-data; name=\"thefile\";filename=\"file.txt\";"));
    QFile *textFile=new QFile(":/file.txt");
    textFile->setParent(multiPart); //设置父类，在删除reply时一起删除
    if(textFile->open(QIODevice::ReadOnly)){
        //要读取小块数据，请使用setBody();对于像图像这样比较大的数据块，请使用setBodyDevice()。
        //filePart.setBody(textFile->readAll());
        filePart.setBodyDevice(textFile);
        multiPart->append(filePart);
    }

    //图片内容
    QHttpPart imagePart;
    agePart.setHeader(QNetworkRequest::ContentTypeHeader,QVariant("text/plain"));
    //需要有filename的文件，不然提交不成功
    agePart.setHeader(QNetworkRequest::ContentDispositionHeader,QVariant("form-data; name=\"theimage\";filename=\"image.png\";"));
    QFile *imageFile=new QFile(":/image.png");
    textFile->setParent(multiPart); //设置父类，在删除reply时一起删除
    if(textFile->open(QIODevice::ReadOnly)){
        //要读取小块数据，请使用setBody();对于像图像这样比较大的数据块，请使用setBodyDevice()。
        //filePart.setBody(textFile->readAll());
        filePart.setBodyDevice(imageFile);
        multiPart->append(imagePart);
    }

    //没有Boundary可能会导致提交不成功
    multiPart->setBoundary("qtdata");
    QNetworkRequest request(QUrl("http://httpbin.org/post"));
    request.setRawHeader("Content-Type","multipart/form-data;boundary=qtdata");

    //提交表单
    QNetworkReply *reply=manager2.post(request,multiPart);
    multiPart->setParent(reply); //在删除reply时一并释放

    QEventLoop eventLoop;
    connect(&manager2, &QNetworkAccessManager::finished, &eventLoop, &QEventLoop::quit);
    eventLoop.exec();

    qDebug()<<"reply:"<<reply->readAll();
    reply->deleteLater();
}


void Widget::receiveReply(QNetworkReply *reply)
{
    if(reply->error()!=QNetworkReply::NoError){
        //处理中的错误信息
        qDebug()<<"reply error:"<<reply->errorString();
    }else{
        //请求方式
        qDebug()<<"operation:"<<reply->operation();
        //状态码
        qDebug()<<"status code:"<<reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        qDebug()<<"url:"<<reply->url();
        qDebug()<<"row header:"<<reply->rawHeaderList();

        //重定向
        /*if (status_code == 301 || status_code == 302){
              // Or the target URL if it was a redirect:
              QVariant redirectionTargetUrl =reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
              //qDebug() << "redirection Url is " << redirectionTargetUrl.toString();
              QUrl url(redirectionTargetUrl.toString());
              manager->get(QNetworkRequest(url));
        }*/

        //获取响应信息
        const QByteArray reply_data=reply->readAll();
        qDebug()<<"read all:"<<reply_data;
        ui->editRecv->append("[Reply]"+QTime::currentTime().toString("hh:mm:ss"));
        //这里根据自己编码选择对应的QString方法
        ui->editRecv->append(QString::fromUtf8(reply_data));

        //解析json
        QJsonParseError json_error;
        QJsonDocument document=QJsonDocument::fromJson(reply_data,&json_error);
        if(json_error.error==QJsonParseError::NoError){
            if(document.isObject()){
                const QJsonObject obj=document.object();
                qDebug()<<obj;
                if(obj.contains("args")){
                    QJsonValue value=obj.value(("args"));
                    qDebug()<<"[args]"<<value;
                    //QJsonValue(object, QJsonObject({"ie":"utf-8"}))
                }
            }
        }else{
            qDebug()<<"json error:"<<json_error.errorString();
        }

    }
    reply->deleteLater();
}
