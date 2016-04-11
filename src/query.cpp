#include "query.h"

using namespace orm;

Query::Query(const Query &) : m_queryType(QueryInvalid){}
Query &Query::operator=(const Query &){}

Query::Query(const QString &entity, QueryTypes type, const QString &connection) :
    m_entity(entity),
    m_queryType(type),
    m_currentConnectionName(connection),
    m_isDistinct(false),
    m_limitCount(0),
    m_limitStart(0)
{
}

Query::~Query(){}

const QSqlDatabase &Query::db() const
{
    static QSqlDatabase db;

    if (!db.isOpen()) {
        if (db.contains(m_currentConnectionName)) {
            db = QSqlDatabase::database(m_currentConnectionName);
        } else {
            db = QSqlDatabase::database();
        }
    }

    Q_ASSERT_X(db.isValid(), "ORM", "database connection not valid");
    Q_ASSERT_X(db.driver()->hasFeature(QSqlDriver::LastInsertId), "ORM", "database driver does not support last insert ID returning");
    Q_ASSERT_X(db.driver()->hasFeature(QSqlDriver::PreparedQueries), "ORM", "database driver does not support prepared queries");

    return db;
}

Query Query::select(const QString &entity, const QString &connection)
{
    return Query(entity, QuerySelect, connection);
}

Query Query::insert(const QString &entity, const QString &connection)
{
    return Query(entity, QueryInsert, connection);
}

Query Query::update(const QString &entity, const QString &connection)
{
    return Query(entity, QueryUpdate, connection);
}

Query Query::remove(const QString &entity, const QString &connection)
{
    return Query(entity, QueryRemove, connection);
}

void Query::createSelect(QSqlQuery &qr)
{
    QStringList sql;

    sql << "SELECT";

    if (m_isDistinct) {
        sql << "DISTINCT";
    }

    if (m_fields.count() == 0) {
        m_fields += entityFields(m_entity);
        for (int i = 0; i < m_joinEntities.count(); i++) {
            m_fields += entityFields(m_joinEntities.at(i));
        }
    }
    sql << m_fields.join(',');

    sql << "FROM" << m_entity;

    if (m_joinSql.count() > 0) {
        sql += m_joinSql;
    }

    if (!m_whereItems.isEmpty()) {
        sql << "WHERE";
        sql << m_whereItems;
    }

    if (!m_order.isEmpty()) {
        sql << "ORDER BY" << m_order;
    }

    if (m_limitCount != 0) {
        sql << "LIMIT" << QString("%1, %2").arg(m_limitCount, m_limitStart);
    }

    if (!m_group.isEmpty()) {
        sql << "GROUP BY" << m_group;
    }

    if (qr.prepare(sql.join(' '))) {
        for (int i = 0; i < m_whereBounds.size(); i++) {
            qr.addBindValue(m_whereBounds.at(i));
        }
    }
}

void Query::createInsert(QSqlQuery &qr)
{
    QStringList sql;
    sql << QString("INSERT INTO %1").arg(m_entity);

    QStringList fields, placeholders;
    for (int i=0; i < m_values.count(); i++) {
        if (m_values.field(i).value().isValid()) {
            fields << m_values.fieldName(i);
            placeholders << "?";
        }
    }

    sql << QString("(%1)").arg(fields.join(','))
        << QString("VALUES (%1)").arg(placeholders.join(','));

    if (qr.prepare(sql.join(' '))) {
        for (int i=0; i < fields.size(); i++) {
            int ix = m_values.indexOf(fields.at(i));
            qr.addBindValue(m_values.field(ix).value());
        }
    }
}

void Query::createUpdate(QSqlQuery &qr)
{
    QSqlIndex pi = db().primaryIndex(m_entity);

    QStringList sql;
    sql << QString("UPDATE %1").arg(m_entity)
        << "SET";

    QStringList fields,fields_sql;
    for (int i=0; i < m_values.count(); i++) {
        if (pi.contains(m_values.field(i).name())) continue;
        if (m_values.field(i).value().isValid()) {
            fields << m_values.field(i).name();
            fields_sql << QString("%1 = ?").arg(m_values.field(i).name());
        }
    }
    sql << fields_sql.join(',');

    if (!m_whereItems.isEmpty()) {
        sql << "WHERE";
        sql << m_whereItems;
    }

    if (qr.prepare(sql.join(' '))) {
        for (int i = 0; i < fields.size(); i++) {
            qr.addBindValue(m_values.field(fields.at(i)).value());
        }
        for (int i = 0; i < m_whereBounds.size(); i++) {
            qr.addBindValue(m_whereBounds.at(i));
        }
    }
}

void Query::createRemove(QSqlQuery &qr)
{
    QStringList sql;
    sql << QString("DELETE FROM %1").arg(m_entity);

    if (!m_whereItems.isEmpty()) {
        sql << "WHERE";
        sql << m_whereItems;
    }

    if (qr.prepare(sql.join(' '))) {
        for (int i = 0; i < m_whereBounds.size(); i++) {
            qr.addBindValue(m_whereBounds.at(i));
        }
    }
}

QStringList Query::entityFields(const QString &entity)
{
    QStringList fields_list;

    QSqlRecord rec = db().record(entity);
    for (int i = 0; i < rec.count(); i++) {
        QString pk_name = db().primaryIndex(entity).field(0).name();
        if (pk_name == rec.field(i).name()) {
            if (entity == m_entity) {
                fields_list << QString("%1.%2 AS %2").arg(entity).arg(pk_name);
            }
            continue;
        }
        QString postfix;
        QString full_name = QString("%1.%2").arg(entity).arg(rec.fieldName(i));
        if (entity == m_entity) {
            postfix = db().driver()->escapeIdentifier(rec.fieldName(i), QSqlDriver::FieldName);
        } else {
            // TODO hove to work this with driver side escaping
            postfix = QString("\"%1\"").arg(full_name);
        }
        fields_list << QString("%1 AS %2").arg(db().driver()->escapeIdentifier(full_name, QSqlDriver::TableName)).arg(postfix);
    }

    return fields_list;
}

QSqlQuery Query::make()
{
    QSqlQuery qr;

    switch (m_queryType) {
        case QuerySelect:
            createSelect(qr);
            break;
        case QueryInsert:
            createInsert(qr);
            break;
        case QueryUpdate:
            createUpdate(qr);
            break;
        case QueryRemove:
            createRemove(qr);
            break;
        default:
            break;
    }

    return qr;
}

Query &Query::where(const QString &sql, QVariantList binds)
{
    m_whereItems = sql;
    m_whereBounds = binds;
    return *this;
}

Query &Query::where(const QString &sql, const QVariant &bind)
{
    return where(sql, QVariantList() << bind);
}

Query &Query::where(const QString &sql)
{
    return where(sql, QVariantList());
}

Query &Query::distinct()
{
    m_isDistinct = true;
    return *this;
}

Query &Query::fields(const QString &fields_sql)
{
    m_fields << fields_sql;
    return *this;
}

Query &Query::fields(const QStringList &fields_list)
{
    return fields(fields_list.join(','));
}

Query &Query::fieldsFrom(const QString &entity)
{
    return fields(entityFields(entity));
}

Query &Query::order(const QString &fields_sql)
{
    m_order = fields_sql;
    return *this;
}

Query &Query::limit(int limit_count, int limit_start)
{
    m_limitCount = limit_count;
    m_limitStart = limit_start;
    return *this;
}

Query &Query::join(const QString &ent, const QString &fk, Query::JoinTypes join_type)
{
    QString join_type_str;
    switch (join_type) {
        case JoinLeft:
            join_type_str = "LEFT JOIN";
            break;
        case JoinRight:
            join_type_str = "RIGHT JOIN";
            break;
        case JoinInner:
            join_type_str = "INNER JOIN";
            break;
        default:
            join_type_str = "JOIN";
            break;
    }

    m_joinEntities << ent;
    m_joinSql << QString ("%1 %3 ON %2.%4 = %3.%5").arg(join_type_str)
                                                   .arg(m_entity)
                                                   .arg(ent)
                                                   .arg(fk)
                                                   .arg(db().primaryIndex(ent).field(0).name());

    return *this;
}

Query &Query::join(const QString &join_sql)
{
    m_joinSql << join_sql;
    return *this;
}

Query &Query::group(const QString &fields_sql)
{
    m_group = fields_sql;
    return *this;
}

Query &Query::values(const QSqlRecord &rec)
{
    m_values = rec;
    return *this;
}

Query &Query::values(const QMap<QString, QVariant> &values_map)
{
    QSqlRecord rec;
    QMapIterator<QString, QVariant> i(values_map);
    while (i.hasNext()) {
        i.next();
        QSqlField fld(i.key());
        fld.setValue(i.value());
        rec.append(fld);
    }
    return values(rec);
}

Query &Query::values(const QSqlField &field)
{
    QSqlRecord rec;
    rec.append(field);
    return values(rec);
}

Query &Query::values(const QString &name, const QVariant &value)
{
    QSqlField fld(name);
    fld.setValue(value);
    return values(fld);
}
