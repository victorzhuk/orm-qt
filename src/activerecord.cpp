#include "activerecord.h"

using namespace orm;

ActiveRecord::ActiveRecord(const QString &entity, const QString &connection) :
    m_connectionName(connection),
    m_entity(entity),
    m_isValid(false),
    m_isPersistent(false)
{
    init();
}

void ActiveRecord::init()
{
    if (db().isValid())  {
        QStringList tables = db().tables(QSql::Tables) + db().tables(QSql::Views);
        Q_ASSERT_X(tables.contains(m_entity), "ORM", "entity does not exists");
        if (tables.contains(m_entity)) {
            QSqlIndex pk_ix = db().primaryIndex(m_entity);
            Q_ASSERT_X(pk_ix.count() == 1, "ORM", "primary key invalid");
            if (pk_ix.count() == 1) {
                m_primaryKey.setName(pk_ix.field(0).name());
                m_record = db().record(m_entity);
                m_isValid = true;
            } else {
                m_lastError = "primary key null or multiple";
            }
        } else {
            m_lastError = "entity does not exists";
        }
    } else {
        m_lastError = db().lastError().text();
    }
}

const QSqlDatabase &ActiveRecord::db()
{
    static QSqlDatabase db;

    if (!db.isOpen()) {
        if (db.contains(m_connectionName)) {
            db = QSqlDatabase::database(m_connectionName);
        } else {
            db = QSqlDatabase::database();
        }
    }

    return db;
}

bool ActiveRecord::isPersistent() const
{
    return m_isPersistent;
}

bool ActiveRecord::isValid() const
{
    return m_isValid;
}

bool ActiveRecord::load(const QVariant &pk_value)
{
    bool result = false;

    if (m_isValid) {
        QSqlQuery qr = Query::select(m_entity, m_connectionName).where(QString("%1 = ?").arg(m_primaryKey.name()), QVariantList() << pk_value).make();
        if (qr.exec()) {
            if (qr.next()) {
                m_record.clear();
                m_primaryKey.setValue(pk_value);
                for (int i=0; i < qr.record().count(); i++) {
                    m_record.append(qr.record().field(i));
                }
                result = m_isPersistent = true;
            } else {
                m_lastError = "object does not persists";
            }
        } else {
            m_lastError = qr.lastError().text();
        }
    }

    return result;
}

bool ActiveRecord::save()
{
    bool result = false;

    if (m_isValid) {
        QSqlQuery qr;

        QSqlRecord save_values(m_record);

        if (save_values.field(m_primaryKey.name()).isNull()) {
            save_values.remove(m_record.indexOf(m_primaryKey.name()));
        }

        if (m_primaryKey.value().isValid()) {
            qr = Query::update(m_entity).values(save_values).where(QString("%1 = ?").arg(m_primaryKey.name()), QVariantList() << m_primaryKey.value()).make();
            if (qr.exec()) {
                if (qr.numRowsAffected() > 0) {
                    if (save_values.value(m_primaryKey.name()).isValid()) {
                        m_primaryKey.setValue(save_values.value(m_primaryKey.name()));
                    }
                    result = m_isPersistent = true;
                }
            }
        } else {
            qr = Query::insert(m_entity).values(save_values).make();
            if (qr.exec()) {
                QVariant pk = qr.lastInsertId();
                if (pk.isValid()) {
                    m_primaryKey.setValue(pk);
                    result = m_isPersistent = true;
                }
            }
        }

        if (qr.lastError().isValid()) {
            m_lastError = qr.lastError().text();
        }
    }

    return result;
}

bool ActiveRecord::remove()
{
    if (m_isValid && m_isPersistent) {
        QSqlQuery qr;

        if (m_primaryKey.value().isValid()) {
            qr = Query::remove(m_entity).where(QString("%1 = ?").arg(m_primaryKey.name()), QVariantList() << m_primaryKey.value()).make();
            if (qr.exec()) {
                if (qr.numRowsAffected() > 0) {
                    m_primaryKey.setValue(QVariant());
                    m_isPersistent = false;
                    return true;
                }
            }
        }

        if (qr.lastError().isValid()) {
            m_lastError = qr.lastError().text();
        }
    }

    return false;
}

void ActiveRecord::setRecord(const QSqlRecord &rec)
{
    m_record = rec.keyValues(m_record);
}

void ActiveRecord::setValue(const QString &name, const QVariant &val)
{
    m_record.setValue(name, val);
}

const QSqlField &ActiveRecord::primaryKey() const
{
    return m_primaryKey;
}

QVariant ActiveRecord::value(const QString &name) const
{
    return m_record.value(name);
}

const QString &ActiveRecord::entity() const
{
    return m_entity;
}

const QSqlRecord &ActiveRecord::record() const
{
    return m_record;
}

const QString &ActiveRecord::lastError() const
{
    return m_lastError;
}
