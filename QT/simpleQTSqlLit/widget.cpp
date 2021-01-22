#include "widget.h"
#include "ui_widget.h"

#include <QDebug>


Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    sqllitUtil = new sqlLitUtil(); //创建自动连接sqllit数据库
}

Widget::~Widget()
{
    delete ui;
}


void Widget::on_btncreteTbl_clicked()
{
    //QString strsql=QString("create table %1(id int,name varchar(255),add varchar(255),tel varchar(20))").arg("mytestTable");
    QString strsql=QString("create table mytable(id int,name varchar,addr varchar,tel varchar)");

    if(sqllitUtil->excute(strsql)){
        qDebug()<<"create success!";
    }else {
        qDebug()<<"create fail!";
    }
}

void Widget::on_btnCreateData_clicked()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE","A");

    db.setHostName("localhost");
    db.setDatabaseName("qtdb.db");
    db.setUserName("zzzz");
    db.setPassword("123456");

    qDebug()<<"Connection name:" << db.connectionName();

    if(!db.open()){
        qDebug()<<"Database open failed.";
    }

    //QSqlQuery query(QSqlDatabase::database("A"));
    QSqlQuery query(db);


//    bool success = query.exec("create table automobile"
//                              "(id int primary key,"
//                              "atttibute varchar,"
//                              "type varchar,"
//                              "kind varchar,"
//                              "nation int,"
//                              "carnumbr int,"
//                              "elevaltor int,"
//                              "oil int,"
//                              "temperature int)");

    query.prepare("create table automobile"
                  "(id int primary key,"
                  "atttibute varchar,"
                  "type varchar,"
                  "kind varchar,"
                  "nation int,"
                  "carnumbr int,"
                  "elevaltor int,"
                  "oil int,"
                  "temperature int)");
    bool success =  query.exec();
    if(success){
        qDebug()<<"Create table success";
    }else {
        qDebug()<<"Create table failed";
    }

    db.close();
}
