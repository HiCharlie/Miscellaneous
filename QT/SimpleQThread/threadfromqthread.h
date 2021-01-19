#ifndef THREADFROMQTHREAD_H
#define THREADFROMQTHREAD_H

#include <QThread>
#include <QMutex>


class ThreadFromQThread : public QThread
{
    Q_OBJECT
public:
    ThreadFromQThread(QObject* ptr);
    ~ThreadFromQThread();

public:
    void setSomething();
    void getSomething();
    void setRunCount(int count);
    void run();
    void doSomething();

public slots:
    void stopImmediately();

private:
    QMutex m_lock;
    bool m_isCanRun;

    int m_runCount;

signals:
    void message(const QString &info);
    void progress(int present);

};

#endif // THREADFROMQTHREAD_H
