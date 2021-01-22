#include "connectiontestthread.h"
#include "ConnectionPool.h"

#include <QCoreApplication>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    //创建10个线程
    for (int i=0;i<10;i++) {
        ConnectionTestThread *thread = new ConnectionTestThread();

        thread->start();
    }

    ConnectionPool::release();

    return a.exec();
}
