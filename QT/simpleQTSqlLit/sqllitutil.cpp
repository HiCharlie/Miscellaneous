#include "sqllitutil.h"

#include <QDebug>


//参考https://www.cnblogs.com/wiessharling/p/3848962.html

/**
 * @brief sqlLitUtil::sqlLitUtil
 * @param strDatabase
 * 构造函数
 */
sqlLitUtil::sqlLitUtil(const QString strDatabase):
    m_strDatabase(strDatabase)
{
    createConnect(m_strDatabase); //创建连接
}

/**
 * @brief sqlLitUtil::~sqlLitUtil
 * 析构函数，关闭连接
 */
sqlLitUtil::~sqlLitUtil()
{
    m_db.close();
}

/**
 * @brief sqlLitUtil::createConnect
 * 创建一个与数据库的连接
 * @param strConn
 * @return
 */
bool sqlLitUtil::createConnect(const QString& strConn)
{
    //与数据库建立连接
    if(QSqlDatabase::contains("my_conn")){
        m_db = QSqlDatabase::database("my_conn");
    }else {
        m_db = QSqlDatabase::addDatabase("QSQLITE","my_conn");
    }

    //设置数据库名
    m_db.setDatabaseName(strConn);

    //打开数据库
    if(!m_db.open()){
        qDebug()<<"Open database failed!";
        return  false;
    }
    else {
        return  true;
    }
}

/**
 * @brief sqlLitUtil::excuteRecord
 * 通过SQL语句查询
 * @param strQuery
 * @return
 */
QSqlRecord sqlLitUtil::excuteRecord(const QString &strQuery)
{
    //判断是否已打开连接
    if(!m_db.isOpen()){
        createConnect(m_strDatabase);
    }

    //创建一个SqlQuery
    QSqlQuery query(m_db);
    query.prepare(strQuery);
    query.exec();
    return  query.record();
}

/**
 * @brief sqlLitUtil::excuteRecord
 * 通过参数化查询数据
 * @param strQuery
 * @param lstParameter
 * @return
 */
QSqlRecord sqlLitUtil::excuteRecord(const QString &strQuery, QList<QVariant> lstParameter)
{
    //判断是否已打开连接
    if(!m_db.isOpen()){
        createConnect(m_strDatabase);
    }

    //创建一个SqlQuery
    QSqlQuery query(m_db);
    query.prepare(strQuery);

    for (int i=0;i<lstParameter.count();i++) {
        query.bindValue(i,lstParameter[i]);
    }

    query.exec();
    return  query.record();
}

/**
 * @brief sqlLitUtil::excuteRecord
 * 单一参数查询数据
 * @param strQuery
 * @param Parameter
 * @return
 */
QSqlRecord sqlLitUtil::excuteRecord(const QString &strQuery, QVariant Parameter)
{
    //判断是否已打开连接
    if(!m_db.isOpen()){
        createConnect(m_strDatabase);
    }

    //创建一个SqlQuery
    QSqlQuery query(m_db);
    query.prepare(strQuery);
    query.bindValue(0, Parameter);

    query.exec();
    return  query.record();
}

/**
 * @brief sqlLitUtil::excuteSqlRecord
 * 通过sql语句查询数据
 * @param strQuery
 * @return
 */
QSqlQuery sqlLitUtil::excuteSqlRecord(const QString &strQuery)
{
    //判断是否已打开连接
    if(!m_db.isOpen()){
        createConnect(m_strDatabase);
    }

    //创建一个SqlQuery
    QSqlQuery query(m_db);
    query.prepare(strQuery);
    query.exec();
    return  query;
}

/**
 * @brief sqlLitUtil::excuteSqlRecord
 * 通过参数列表查询数据
 * @param strQuery
 * @param lstParameter
 * @return
 */
QSqlQuery sqlLitUtil::excuteSqlRecord(const QString &strQuery, QList<QVariant> lstParameter)
{
    //判断是否已打开连接
    if(!m_db.isOpen()){
        createConnect(m_strDatabase);
    }

    //创建一个SqlQuery
    QSqlQuery query(m_db);
    query.prepare(strQuery);

    for (int i=0;i<lstParameter.count();i++) {
        query.bindValue(i,lstParameter[i]);
    }

    query.exec();
    return  query;
}

/**
 * @brief sqlLitUtil::excuteSqlRecord
 * 单一参数查询数据
 * @param strQuery
 * @param Parameter
 * @return
 */
QSqlQuery sqlLitUtil::excuteSqlRecord(const QString &strQuery, QVariant Parameter)
{
    //判断是否已打开连接
    if(!m_db.isOpen()){
        createConnect(m_strDatabase);
    }

    //创建一个SqlQuery
    QSqlQuery query(m_db);
    query.prepare(strQuery);
    query.bindValue(0, Parameter);

    query.exec();
    return  query;
}

/**
 * @brief sqlLitUtil::excuteInt
 * @param strQuery
 * @return
 */
int sqlLitUtil::excuteInt(const QString &strQuery)
{
    //判断是否已打开连接
    if(!m_db.isOpen()){
        createConnect(m_strDatabase);
    }

    //创建一个SqlQuery
    QSqlQuery query(m_db);
    query.prepare(strQuery);
    query.exec();

    int id = 0;
    while(query.next()){
        id = query.value(0).toInt();
    }

    return  id;
}

/**
 * @brief sqlLitUtil::excuteInt
 * @param strQuery
 * @param lstParameter
 * @return
 */
int sqlLitUtil::excuteInt(const QString &strQuery, QList<QVariant> lstParameter)
{
    //判断是否已打开连接
    if(!m_db.isOpen()){
        createConnect(m_strDatabase);
    }

    //创建一个SqlQuery
    QSqlQuery query(m_db);
    query.prepare(strQuery);
    for (int i=0;i<lstParameter.count();i++) {
        query.bindValue(i,lstParameter[i]);
    }
    query.exec();

    int id = 0;
    while(query.next()){
        id = query.value(0).toInt();
    }

    return  id;
}

/**
 * @brief sqlLitUtil::excuteInt
 * @param strQuery
 * @param Parameter
 * @return
 */
int sqlLitUtil::excuteInt(const QString &strQuery, QVariant Parameter)
{
    //判断是否已打开连接
    if(!m_db.isOpen()){
        createConnect(m_strDatabase);
    }

    //创建一个SqlQuery
    QSqlQuery query(m_db);
    query.prepare(strQuery);

    query.bindValue(0,Parameter);
    query.exec();

    int id = 0;
    while(query.next()){
        id = query.value(0).toInt();
    }

    return  id;
}

/**
 * @brief sqlLitUtil::excute
 * @param strQuery
 * @return
 */
bool sqlLitUtil::excute(const QString &strQuery)
{
    //判断是否已打开连接
    if(!m_db.isOpen()){
        createConnect(m_strDatabase);
    }

    //创建一个SqlQuery
    QSqlQuery query(m_db);
    query.prepare(strQuery);

    return query.exec();
}

/**
 * @brief sqlLitUtil::excute
 * @param strQuery
 * @param lstParameter
 * @return
 */
bool sqlLitUtil::excute(const QString &strQuery, QList<QVariant> lstParameter)
{
    //判断是否已打开连接
    if(!m_db.isOpen()){
        createConnect(m_strDatabase);
    }

    //创建一个SqlQuery
    QSqlQuery query(m_db);
    query.prepare(strQuery);
    for (int i=0;i<lstParameter.count();i++) {
        query.bindValue(i,lstParameter[i]);
    }

    return query.exec();
}

/**
 * @brief sqlLitUtil::excute
 * @param strQuery
 * @param Parameter
 * @return
 */
bool sqlLitUtil::excute(const QString &strQuery, QVariant Parameter)
{
    //判断是否已打开连接
    if(!m_db.isOpen()){
        createConnect(m_strDatabase);
    }

    //创建一个SqlQuery
    QSqlQuery query(m_db);
    query.prepare(strQuery);
    query.bindValue(0,Parameter);

    return query.exec();
}

/**
 * @brief sqlLitUtil::ExecuteString
 * @param strQuery
 * @return
 */
QString sqlLitUtil::ExecuteString(const QString &strQuery)
{
    //判断是否已打开连接
    if(!m_db.isOpen())
    {
        createConnect(m_strDatabase);
    }

    QSqlQuery query(m_db);
    query.prepare(strQuery);
    query.exec();
    QString temp;
    while(query.next())
    {
        temp = query.value(0).toString();
    }

    return temp;
}

/**
 * @brief sqlLitUtil::executeQueryModel
 * @param p_queryModel
 * @param strQuery
 */
void sqlLitUtil::executeQueryModel(QSqlQueryModel *p_queryModel, const QString &strQuery)
{
    //判断是否已打开连接
    if(!m_db.isOpen())
    {
        createConnect(m_strDatabase);
    }

    QSqlQuery query(m_db);
    query.prepare(strQuery);
    p_queryModel->setQuery(query);
}

/**
 * @brief sqlLitUtil::executeQueryModel
 * @param p_queryModel
 * @param strQuery
 * @param lstParameter
 */
void sqlLitUtil::executeQueryModel(QSqlQueryModel *p_queryModel, const QString &strQuery, QList<QVariant> lstParameter)
{
    //判断是否已打开连接
    if(!m_db.isOpen())
    {
        createConnect(m_strDatabase);
    }

    QSqlQuery query(m_db);
    query.prepare(strQuery);
    for(int i = 0; i < lstParameter.count(); i++)
        query.bindValue(i, lstParameter[i]);
    p_queryModel->setQuery(query);
}

/**
 * @brief sqlLitUtil::executeQueryModel
 * @param p_queryModel
 * @param strQuery
 * @param Parameter
 */
void sqlLitUtil::executeQueryModel(QSqlQueryModel *p_queryModel, const QString &strQuery, QVariant Parameter)
{
    //判断是否已打开连接
    if(!m_db.isOpen())
    {
        createConnect(m_strDatabase);
    }

    QSqlQuery query(m_db);
    query.prepare(strQuery);

    query.bindValue(0, Parameter);
    p_queryModel->setQuery(query);
}



