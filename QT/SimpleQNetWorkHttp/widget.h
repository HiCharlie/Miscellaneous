#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>

//QT+=network
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QHttpPart>
#include <QHttpMultiPart>
#include <QFile>

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
public:
    void getTest();
    void postTest();
    void formTest();

public slots:
    void receiveReply(QNetworkReply *reply);

private:
    Ui::Widget *ui;

    QNetworkAccessManager *manager;
    QTimer timer;
};

#endif // WIDGET_H
