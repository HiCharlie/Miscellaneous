#include "ConnectionPool.h"
#include <QDebug>
#include <QFileInfo>
#include <QDir>

//配置文件头文件
#include <QSettings>
//#include "system_config.h"

QMutex ConnectionPool::mutex;
QWaitCondition ConnectionPool::waitConnection;
ConnectionPool* ConnectionPool::instance = NULL;

bool ConnectionPool::isDirOrFileExits(QString dbName)
{
    QFileInfo fileinfo(dbName);

    return  fileinfo.exists();
}

bool ConnectionPool::createSqlitePath(QString path)
{
    QDir parameterDir;

    return  parameterDir.mkpath(path);
}

void ConnectionPool::release()
{
    QMutexLocker locker(&mutex);
    delete  instance;
    instance = NULL;
}

QSqlDatabase ConnectionPool::openConnection()
{
    ConnectionPool& pool = ConnectionPool::getInstance();
    QString connectionName;

    QMutexLocker locker(&mutex);

    //已创建连接数
    int connectionCount = pool.unusedConnectionNames.size()+pool.usedConnectionNames.size();

    //如果连接已经用完，等待waitInterval毫秒看是否有可用连接，最长等待时间为maxWaitTime毫秒
    for(int i=0;
            i < pool.maxWaitTime &&
            pool.unusedConnectionNames.size() == 0 &&
            pool.maxConnectionCount == connectionCount;
            i += pool.waitInterval){
        //释放mutex，等待waitInterval
        waitConnection.wait(&mutex,pool.waitInterval);

        //重新计算已创建连接数
        connectionCount = pool.unusedConnectionNames.size()+pool.usedConnectionNames.size();
    }

    if(pool.unusedConnectionNames.size()>0){
        //有已经回收的连接，复用他们
        connectionName = pool.unusedConnectionNames.dequeue();
    }else if(connectionCount < pool.maxConnectionCount){
        //没有已经回收的连接，但是没有达到最大连接数，则创建新的连接
        connectionName = QString("Connection-%1").arg(connectionCount+1);
    }else {
        //已经达到最大连接数
        qDebug()<<"Cannot create more connections.";

        return  QSqlDatabase();
    }

    //创建新连接或使用已有连接
    QSqlDatabase db = pool.createConnection(connectionName);

    //将有效的连接放入usedConnectionNames
    if(db.isOpen()){
        pool.usedConnectionNames.enqueue(connectionName);
    }

    return  db;
}

void ConnectionPool::closeConnection(QSqlDatabase connection)
{
    ConnectionPool &pool = ConnectionPool::getInstance();
    QString connectionName = connection.connectionName();

    //如果是我们创建的连接，从used里删除，放入unused里
    if(pool.usedConnectionNames.contains(connectionName)){
        QMutexLocker locker(&mutex);
        pool.usedConnectionNames.removeOne(connectionName);
        pool.unusedConnectionNames.enqueue(connectionName);
        waitConnection.wakeOne();
    }

}

ConnectionPool::~ConnectionPool()
{
    //销毁连接池的时候删除所有的连接
    foreach(QString connectionName,usedConnectionNames){
        QSqlDatabase::removeDatabase(connectionName);     //删除代码有问题，应该延时删除
    }

    foreach(QString connectionName,unusedConnectionNames){
        QSqlDatabase::removeDatabase(connectionName);     //删除代码有问题，应该延时删除
    }

    qDebug()<<"ConnectionPool Destruct!";
}

ConnectionPool &ConnectionPool::getInstance()
{
    if(NULL == instance){
        QMutexLocker locker(&mutex);

        if(NULL==instance){
            instance = new ConnectionPool();
        }
    }

    return  *instance;
}

ConnectionPool::ConnectionPool()
{
    //创建数据库连接的这些信息在实际开发的时候都需要通过读取配置文件得到
    QString DirPath = QCoreApplication::applicationDirPath();   //获取程序运行目录
    qDebug()<<"DirPath:" + DirPath;
    QString ConfigDirPath = DirPath+"//" + "config";            //配置文件目录,固定写入程序
    QString ConfigFile = ConfigDirPath + "//" + "conf.ini";

    //判断是否存在目录
    if(!isDirOrFileExits(ConfigFile))
    {
        qDebug()<<"configFile is invalid."<<ConfigDirPath;

        createSqlitePath(ConfigDirPath);

        //这里采用写在代码中方式
        hostName = "127.0.0.1";
        databaseName = "mytest";
        databasePath = "data";
        userName = "root";
        password = "";
        databaseType = "QMYSQL";
        testOnBorrow = true;
        testOnBorrowSql = "SELECT 1";

        maxWaitTime = 1000;
        waitInterval = 200;
        maxConnectionCount = 5;

        QSettings set1(ConfigFile,QSettings::IniFormat);
        set1.setIniCodec(QTextCodec::codecForName("utf-8"));
        set1.beginGroup("SqlSetting");

        set1.setValue("hostName", hostName);            //主机名称
        set1.setValue("databaseName", databaseName);    //数据库名称或文件名称
        set1.setValue("databasePath", databasePath);    //数据库路径
        set1.setValue("userName", userName);            //用户名
        set1.setValue("password", password);            //密码
        set1.setValue("databaseType", databaseType);    //数据库类型
        set1.setValue("testOnBorrow", testOnBorrow);    //测试是否连接
        set1.setValue("testOnBorrowSql", testOnBorrowSql);  //测试语句
        set1.setValue("maxWaitTime", maxWaitTime);          //最大等待时间
        set1.setValue("waitInterval", waitInterval);        //等待间隔
        set1.setValue("maxConnectionCount", maxConnectionCount);    //最大连接数

        set1.endGroup();

    }

    //从ini文件中读取sql配置
    QSettings set(ConfigFile,QSettings::IniFormat);
    set.setIniCodec(QTextCodec::codecForName("utf-8"));
    set.beginGroup("SqlSetting");

    hostName = set.value("hostName").toString();
    databaseName = set.value("databaseName").toString();
    databasePath = set.value("databasePath").toString();
    userName = set.value("userName").toString();
    password = set.value("password").toString();
    databaseType = set.value("databaseType").toString();
    testOnBorrow = set.value("testOnBorrow").toBool();
    testOnBorrowSql = set.value("testOnBorrowSql").toString();

    maxWaitTime = set.value("maxWaitTime").toInt();
    waitInterval = set.value("waitInterval").toInt();
    maxConnectionCount = set.value("maxConnectionCount").toInt();
    set.endGroup();
}

QSqlDatabase ConnectionPool::createConnection(const QString &connectionName)
{
    //连接已创建过的，复用它
    if(QSqlDatabase::contains(connectionName)){
        QSqlDatabase db1 = QSqlDatabase::database(connectionName);

        if(testOnBorrow){
            //返回连接前访问数据库，是否连接断开，重新建立连接
            qDebug()<<"Test connection on borrow, execute:"
                    <<testOnBorrowSql << ", for"
                   << connectionName;

            QSqlQuery query(testOnBorrowSql,db1);

            // 测试有错误或无法打开
            if(query.lastError().type()!=QSqlError::NoError && db1.open()){

                qDebug()<< "Open database error:" << db1.lastError().text();
                return  QSqlDatabase();
            }
        }

        return  db1;        //返回已创建过的
    }

    //创建一个新的连接
    QSqlDatabase db = QSqlDatabase::addDatabase(databaseType,connectionName);
    db.setHostName(hostName);

    if(databaseType == "QSQLITE"){
        QString dataFilePath = QString(".//%1").arg(databasePath);

        if(!isDirOrFileExits(dataFilePath))
        {
            createSqlitePath(dataFilePath);
        }

        db.setDatabaseName(QString("%1//%2.db").arg(dataFilePath).arg(databaseName));
    } else {
        db.setDatabaseName(databaseName);
    }

    db.setUserName(userName);
    db.setPassword(password);

    if(!db.open()){
        qDebug()<<"Open database error:"<<db.lastError().text();

        return  QSqlDatabase();
    }

    return  db; //返回新创建的连接
}
