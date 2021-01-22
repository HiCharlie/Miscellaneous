#include "connectiontestthread.h"
#include "ConnectionPool.h"

void ConnectionTestThread::run()
{
    //从数据库连接池获得连接
    QSqlDatabase db = ConnectionPool::openConnection();
    qDebug()<<"In Thread run():" << db.connectionName();

    QSqlQuery query(db);
    query.exec("SELECT * FROM user where id=1");

    while (query.next()) {
        qDebug()<<query.value("name").toString();
    }

    //连接使用完后，释放回数据库连接池
    ConnectionPool::closeConnection(db);
}

ConnectionTestThread::ConnectionTestThread()
{

}
