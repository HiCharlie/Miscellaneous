#ifndef CONNECTIONTESTTHREAD_H
#define CONNECTIONTESTTHREAD_H

#include <QThread>

class ConnectionTestThread: public QThread
{
protected:
    void run();

public:
    ConnectionTestThread();
};

#endif // CONNECTIONTESTTHREAD_H
