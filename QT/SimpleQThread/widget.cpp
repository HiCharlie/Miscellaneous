#include "widget.h"
#include "ui_widget.h"

#include <QDebug>


Widget::Widget(QWidget *parent) :
    QWidget(parent),m_objThread(NULL),m_currentRunLocalThread(NULL),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    // 初始化控件
    ui->progressBar->setRange(0,100);
    ui->progressBar->setValue(0);
    ui->progressBarheart->setRange(0,100);
    ui->progressBarheart->setValue(0);

    //按钮的信号槽关联
    connect(ui->btnthreadRun,&QPushButton::clicked,
            this,&Widget::onBtnQThreadRun);

    connect(ui->btnthreadLocalRun,&QPushButton::clicked,
            this,&Widget::onBtnQThreadLocalRun);

    connect(ui->btnthreadQuit,&QPushButton::clicked,
            this,&Widget::onBtnQThreadQuit);

    connect(ui->btnthreadTerminate,&QPushButton::clicked,
            this,&Widget::onBtnQThreadTerminate);

    connect(ui->btngetSomthing,&QPushButton::clicked,
            this,&Widget::onBtngetSomthing);

    connect(ui->btnsetSomething,&QPushButton::clicked,
            this,&Widget::onBtnsetSomthing);

    connect(ui->btndoSomthing,&QPushButton::clicked,
            this,&Widget::onBtndoSomthing);


    //继承object按钮信号连接
    connect(ui->btnObjectMove2Thread,&QPushButton::clicked,
            this,&Widget::onbtnObjectMove2Thread);
    connect(ui->btnObjectMove2Thread2,&QPushButton::clicked,
            this,&Widget::onbtnObjectMove2Thread2);
    connect(ui->btnThreadStop,&QPushButton::clicked,
            this,&Widget::onbtnThreadStop);
    connect(ui->btnQuit,&QPushButton::clicked,
            this,&Widget::onbtnQuit);

    //心跳信号关联
    connect(&m_heart,&QTimer::timeout,this,&Widget::heartTimeOut);
    m_heart.setInterval(100);

    //全局线程的创建
    //全局线程创建时可以把窗体指针作为父对象
    m_thread=new ThreadFromQThread(this);

    //关联线程信号
    connect(m_thread,&ThreadFromQThread::message,this,&Widget::receiveMessage);
    connect(m_thread,&ThreadFromQThread::progress,this,&Widget::progress);
    connect(m_thread,&QThread::finished,this,&Widget::onQThreadFinished);

    //UI心跳开始
    m_heart.start();
}

Widget::~Widget()
{
    qDebug()<< "start destory widget...";

    m_thread->stopImmediately();
    m_thread->wait();       //主线程等待子线程结束

    if(m_objThread){
        m_objThread->quit();
        m_objThread->wait();
    }

    delete ui;
    qDebug()<< "end destory widget...";
}

void Widget::onBtnQThreadRun()
{
    ui->progressBar->setValue(0);
    if(m_thread->isRunning()){
        return;
    }

    m_thread->start();
}

void Widget::onBtnQThreadLocalRun()
{
    if(m_currentRunLocalThread){
        m_currentRunLocalThread->stopImmediately();
    }

    //局部线程创建
    ThreadFromQThread* thread = new ThreadFromQThread(NULL);
    connect(thread,&ThreadFromQThread::message,this,&Widget::receiveMessage);
    connect(thread,&ThreadFromQThread::progress,this,&Widget::progress);
    connect(thread,&QThread::finished,this,&Widget::onQThreadFinished);
    connect(thread,&QThread::finished,thread,&QObject::deleteLater);    //线程结束后调用deleteLater来销毁分配的内存

    connect(thread,&QThread::destroyed,this,&Widget::onLocalThreadDestory);

    m_currentRunLocalThread = thread;

    thread->start();
}

void Widget::onBtnQThreadQuit()
{
    ui->textEdit->append("m_thread->quit() but not work");
    m_thread->quit();
}

void Widget::onBtnQThreadTerminate()
{
    m_thread->terminate();
}

void Widget::onBtngetSomthing()
{
    m_thread->getSomething();
}

void Widget::onBtnsetSomthing()
{
    m_thread->setSomething();
}

void Widget::onBtndoSomthing()
{
    m_thread->doSomething();
}

void Widget::onQThreadFinished()
{
    ui->textEdit->append("ThreadFromQThread finish");
}

void Widget::progress(int val)
{
    ui->progressBar->setValue(val);
}

void Widget::receiveMessage(const QString &str)
{
    ui->textEdit->append(str);
}

void Widget::heartTimeOut()
{
    static int s_heartCount = 0;
    ++s_heartCount;
    if(s_heartCount>100){
        s_heartCount = 0;
    }

    ui->progressBarheart->setValue(s_heartCount);
}

void Widget::onLocalThreadDestory(QObject *obj)
{
    //if(qobject_cast<QObject*>(m_currentRunLocalThread)==obj){
    if(m_currentRunLocalThread==obj){
        m_currentRunLocalThread = NULL;
    }
}

void Widget::onbtnObjectMove2Thread()
{
    if(!m_objThread){
        startObjThread();       //函数创建新线程，调用start()函数
    }

    emit startObjThreadWork1();     //主线程通过信号唤起子线程的槽函数
    ui->textEdit->append("start obj Thread work1");
}

void Widget::onbtnObjectMove2Thread2()
{
    if(!m_objThread){
        startObjThread();
    }

    emit startObjThreadWork2();     //主线程通过信号唤起子线程的槽函数
    ui->textEdit->append("start obj Thread work2");
}

void Widget::onbtnThreadStop()
{
    if(m_objThread){
        if(m_obj){
            m_obj->stop();
        }
    }
}

void Widget::onbtnQuit()
{
    if(m_objThread){
        //m_objThread->quit();

    }
}

/**
 * @brief Widget::startObjThread
 * 创建线程
 */
void Widget::startObjThread()
{
    if(m_objThread){
        return;
    }

    m_objThread = new QThread();
    m_obj = new ThreadObject();
    m_obj->moveToThread(m_objThread);
    connect(m_objThread,&QThread::finished,m_objThread,&QObject::deleteLater);
    connect(m_objThread,&QThread::finished,m_obj,&QObject::deleteLater);

    connect(this,&Widget::startObjThreadWork1,m_obj,&ThreadObject::runSomeBigWork1);
    connect(this,&Widget::startObjThreadWork2,m_obj,&ThreadObject::runSomeBigWork2);

    connect(m_obj,&ThreadObject::progress,this,&Widget::progress);
    connect(m_obj,&ThreadObject::message,this,&Widget::receiveMessage);

    m_objThread->start();
}



