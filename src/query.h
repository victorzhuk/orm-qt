#ifndef QUERY_H
#define QUERY_H

#include "orm-qt_global.h"

#include <QDebug>

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>
#include <QSqlField>
#include <QSqlIndex>
#include <QSqlDriver>

namespace orm {

class ORMQTSHARED_EXPORT Query : public QObject
{
    Q_OBJECT

public:
    enum JoinTypes {
        JoinInner,
        JoinLeft,
        JoinRight
    };

public:
    static Query select(const QString &/*entity*/, const QString &connection = QString());
    static Query insert(const QString &/*entity*/, const QString &connection = QString());
    static Query update(const QString &/*entity*/, const QString &connection = QString());
    static Query remove(const QString &/*entity*/, const QString &connection = QString());

public:
    Query(const Query&);
    ~Query();
    QSqlQuery make();

public:
    /*select,update,remove*/
    Query& where(const QString &/*sql*/, QVariantList /*bind values*/);
    Query& where(const QString &/*sql*/, const QVariant &/*value*/);
    Query& where(const QString &/*sql*/);

    /*select*/
    Query& distinct();

    Query& fields(const QString &/*fields sql*/);
    Query& fields(const QStringList &/*fields list*/);
    Query& fieldsFrom(const QString &/*entity*/);

    Query& order(const QString &/*fields sql*/);

    Query& limit(int /*count*/, int limit_start = 0);

    Query& group(const QString &/*fields sql*/);

    Query& join(const QString &/*join sql*/);
    Query& join(const QString &/*entity*/, const QString &/*foreign key*/, JoinTypes join_type = JoinLeft);

    /*insert,update*/
    Query& values(const QSqlRecord &/*record with values*/);
    Query& values(const QMap<QString,QVariant> &/*map with values*/);
    Query& values(const QSqlField &/*single field*/);
    Query& values(const QString &/*single name*/, const QVariant &/*single value*/);

private:
    enum QueryTypes {
        QueryInvalid,
        QuerySelect,
        QueryInsert,
        QueryUpdate,
        QueryRemove
    };

private:
    Query& operator=(const Query&);
    Query(const QString &/*entity*/, QueryTypes, const QString &/*connection name*/);

    const QSqlDatabase& db() const;

private:
    void createSelect(QSqlQuery&);
    void createInsert(QSqlQuery&);
    void createUpdate(QSqlQuery&);
    void createRemove(QSqlQuery&);

    QString escapeTableName(const QString &);
    QString escapeFieldName(const QString &);

    QStringList entityFields(const QString &/*entity*/);

private:
    QString m_currentConnectionName;

    QueryTypes m_queryType;
    QString m_entity;

    QStringList m_fields;

    QString m_whereItems;
    QVariantList m_whereBounds;

    bool m_isDistinct;

    QString m_order;

    QString m_group;

    int m_limitStart;
    int m_limitCount;

    QSqlRecord m_values;

    QStringList m_joinSql;
    QStringList m_joinEntities;
};

}

#endif // QUERY_H
