#include "threadobject.h"
#include <QDebug>
#include <QThread>
#include <QMutexLocker>
#include <QElapsedTimer>
#include <limits>


/*
 * 继承QObject多线程的方法线程的创建很简单，只要让QThread的start函数运行起来就行，
 * 但是需要注意销毁线程的方法在线程创建之后，这个QObject的销毁不应该在主线程里进行，
 * 而是通过deleteLater槽进行安全的销毁，因此，继承QObject多线程的方法在创建时有
 * 几个槽函数需要特别关注：
 * 一个是QThread的finished信号对接QObject的deleteLater使得线程结束后，
 * 继承QObject的那个多线程类会自己销毁
 * 另一个是QThread的finished信号对接QThread自己的deleteLater。
*/

ThreadObject::ThreadObject(QObject* parent):QObject (parent)
  ,m_runCount(20),m_runCount2(std::numeric_limits<int>::max())
  ,m_isStop(true)
{

}

ThreadObject::~ThreadObject()
{
    qDebug()<<"ThreadObject destory";
    emit message(QString("Destory %1->%2, thread id:%3")
                 .arg(__FILE__).arg(__FUNCTION__)
                 .arg((int)QThread::currentThreadId()));
}

void ThreadObject::setRunCount(int count)
{
    m_runCount = count;
    emit message(QString("Destory %1->%2, thread id:%3")
                 .arg(__FILE__).arg(__FUNCTION__)
                 .arg((int)QThread::currentThreadId()));
}

void ThreadObject::stop()
{
    QMutexLocker locker(&m_stopMutex);

    emit message(QString("Destory %1->%2, thread id:%3")
                 .arg(__FILE__).arg(__FUNCTION__)
                 .arg((int)QThread::currentThreadId()));

    m_isStop = true;
}

void ThreadObject::runSomeBigWork1()
{
    {
        QMutexLocker locker(&m_stopMutex);
        m_isStop = false;
    }

    int count = 0;
    QString str = QString("%1->%2,thread id:%3")
            .arg(__FILE__).arg(__FUNCTION__)
            .arg((int)QThread::currentThreadId());

    emit message(str);
    int process = 0;

    while(1){
        {
            QMutexLocker locker(&m_stopMutex);
            if(m_isStop)
                return;
        }

        if(m_runCount == count){
            break;
        }

        QThread::sleep(1);
        int pro = ((float)count/m_runCount)*100;
        if(pro!=process){
            process=pro;
            emit progress((((float)count)/m_runCount)*100);
            emit message(QString("Object::runtimes:%1,m_runCount:%2")
                         .arg(count).arg(m_runCount2));
        }
        ++count;
    }
}

void ThreadObject::runSomeBigWork2()
{
    {
        QMutexLocker locker(&m_stopMutex);
        m_isStop = false;
    }

    int count = 0;
    QString str = QString("%1->%2,thread id:%3")
            .arg(__FILE__).arg(__FUNCTION__)
            .arg((int)QThread::currentThreadId());
    emit message(str);

    int process = 0;
    QElapsedTimer timer;
    timer.start();

    while(1){
        {
            QMutexLocker locker(&m_stopMutex);
            if(m_isStop)
                return;
        }

        if(m_runCount2 == count){
            break;
        }

        //QThread::sleep(1);
        int pro = ((float)count/m_runCount2)*100;
        if(pro!=process){
            process = pro;

            //emit progress((((float)count)/m_runCount)*100);
            emit progress(pro);
            emit message(QString("%1,%2,%3,%4")
                         .arg(count).arg(m_runCount2)
                         .arg(pro).arg(timer.elapsed()));

            timer.restart();
        }
        ++count;
    }
}
