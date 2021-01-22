#ifndef SQLLITUTIL_H
#define SQLLITUTIL_H

#include <QString>
#include <QSqlQuery>
#include <QSqlDatabase>
#include <QSqlDriver>
#include <QSqlQueryModel>
#include <QSqlRecord>

class sqlLitUtil
{
public:
    sqlLitUtil(const QString strDatabase="smart.db");
    ~sqlLitUtil();

    bool createConnect(const QString& strConn);  //创建连接

    QSqlRecord excuteRecord(const QString& strQuery);
    QSqlRecord excuteRecord(const QString& strQuery,QList<QVariant> lstParameter);
    QSqlRecord excuteRecord(const QString& strQuery,QVariant Parameter);

    QSqlQuery excuteSqlRecord(const QString& strQuery);
    QSqlQuery excuteSqlRecord(const QString& strQuery,QList<QVariant> lstParameter);
    QSqlQuery excuteSqlRecord(const QString& strQuery,QVariant Parameter);

    int excuteInt(const QString& strQuery);
    int excuteInt(const QString& strQuery,QList<QVariant> lstParameter);
    int excuteInt(const QString& strQuery,QVariant Parameter);

    bool excute(const QString& strQuery);
    bool excute(const QString& strQuery,QList<QVariant> lstParameter);
    bool excute(const QString& strQuery,QVariant Parameter);

    QString ExecuteString(const QString& strQuery);

    void executeQueryModel(QSqlQueryModel *p_queryModel, const QString& strQuery);
    void executeQueryModel(QSqlQueryModel *p_queryModel, const QString& strQuery,QList<QVariant> lstParameter);
    void executeQueryModel(QSqlQueryModel *p_queryModel, const QString& strQuery, QVariant Parameter);

private:
    QSqlDatabase m_db;
    QString m_strDatabase;

};

#endif // SQLLITUTIL_H
