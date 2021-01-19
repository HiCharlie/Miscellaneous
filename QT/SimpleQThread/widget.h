#ifndef WIDGET_H
#define WIDGET_H

#include "threadfromqthread.h"
#include "threadobject.h"

#include <QWidget>
#include <QTimer>

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = nullptr);
    ~Widget();

public slots:
    void onBtnQThreadRun();
    void onBtnQThreadLocalRun();
    void onBtnQThreadQuit();
    void onBtnQThreadTerminate();
    void onBtngetSomthing();
    void onBtnsetSomthing();
    void onBtndoSomthing();
    void onQThreadFinished();

    void progress(int val);
    void receiveMessage(const QString& str);
    void heartTimeOut();

    void onLocalThreadDestory(QObject* obj);

    void onbtnObjectMove2Thread();
    void onbtnObjectMove2Thread2();
    void onbtnThreadStop();
    void onbtnQuit();

private:
    void startObjThread();

private:
    Ui::Widget *ui;

    /* 全局线程 */
    ThreadFromQThread* m_thread;
    QTimer m_heart;

    ThreadFromQThread *m_currentRunLocalThread;

    ThreadObject* m_obj;
    QThread* m_objThread;

signals:
    void startObjThreadWork1();
    void startObjThreadWork2();
};

#endif // WIDGET_H
