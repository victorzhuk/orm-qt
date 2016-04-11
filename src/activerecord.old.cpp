#include "activerecord.old.h"

using namespace orm;

/******************* basic ***********************/
ActiveRecord::ActiveRecord(QObject *parent) : QObject(parent),
    m_currentState(StateInvalid)
{
}

ActiveRecord::ActiveRecord(const QString &entity, QObject *parent) : QObject(parent),
    m_entity(entity),
    m_currentState(StateInvalid)
{
    init();
}

ActiveRecord::~ActiveRecord()
{
}

void ActiveRecord::swap(const ActiveRecord &ar_from)
{
    m_entity       = ar_from.m_entity;
    m_primaryKey   = ar_from.m_primaryKey;
    m_values       = ar_from.m_values;
    m_currentState = ar_from.m_currentState;
    m_lastError    = ar_from.m_lastError;
}

void ActiveRecord::init()
{
    QSqlDatabase db = QSqlDatabase::database();
    if (db.isValid()) {
        QStringList tables = db.tables(QSql::Tables) + db.tables(QSql::Views);
        Q_ASSERT_X(tables.contains(m_entity), "ORM", "entity does not exists");
        if (tables.contains(m_entity)) {
            QSqlIndex pk_ix = db.primaryIndex(m_entity);
            Q_ASSERT_X(pk_ix.count() == 1, "ORM", "primary key invalid");
            if (pk_ix.count() == 1) {
                m_primaryKey.setName(pk_ix.field(0).name());
                m_currentState = StateNew;
            } else {
                m_lastError = "primary key null or multiple";
            }
        } else {
            m_lastError = "entity does not exists";
        }
    } else {
        m_lastError = db.lastError().text();
    }
}

QString ActiveRecord::lastError() const
{
    return m_lastError;
}

ActiveRecord &ActiveRecord::operator=(const ActiveRecord &ar_from)
{
    swap(ar_from);
    return *this;
}

/******************* getters ***********************/
bool ActiveRecord::isValid() const
{
    return (m_currentState != StateInvalid) ? true : false;
}

//const QString& ActiveRecord::entity() const
{
    return m_entity;
}

bool ActiveRecord::isPersistent() const
{
    return (m_currentState == StateLoaded || m_currentState == StateSaved) ? true : false;
}

const QSqlRecord ActiveRecord::makeRecord() const
{
    QSqlRecord rec;

    if (isValid()) {
        rec = QSqlDatabase::database().record(m_entity);

        for (int i=0; i < rec.count(); i++) {
            if (m_values.contains(rec.field(i).name())) {
                rec.setValue(i, m_values.value(rec.field(i).name()));
            }
        }
    }

    return rec;
}

QMap<QString, QVariant> ActiveRecord::values() const
{
    return m_values;
}

//QVariant ActiveRecord::value(const QString& field_name) const
{
    QVariant val;
    if (isValid()) {
        if (m_values.contains(field_name)) {
            val = m_values.value(field_name);
        }
    }
    return val;
}

//void ActiveRecord::setValue(const QString &field_name, const QVariant &field_value)
{
    if (isValid()) {
        m_values[field_name] = field_value;
        if (m_currentState != StateNew) {
            m_currentState = StateUnsaved;
        }
    }
}

void ActiveRecord::setValues(const QSqlRecord &rec)
{
    for (int i=0; i < rec.count(); i++) {
        m_values.insert(rec.field(i).name(), rec.field(i).value());
    }
}


/******************* active record ***********************/
bool ActiveRecord::save()
{
    if (isValid()) {
        QSqlQuery qr;
        if (m_primaryKey.value().isValid()) {
            qr = Query::update(m_entity).values(makeRecord()).where(QString("%1 = ?").arg(m_primaryKey.name()), QVariantList() << m_primaryKey.value()).make();
            if (qr.exec()) {
                if (qr.numRowsAffected() > 0) {
                    m_currentState = StateSaved;
                    return true;
                }
            }
        } else {
            qr = Query::insert(m_entity).values(makeRecord()).make();
            if (qr.exec()) {
                QVariant pk = qr.lastInsertId();
                if (pk.isValid()) {
                    m_primaryKey.setValue(pk);
                    m_currentState = StateSaved;
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

bool ActiveRecord::remove()
{
    if (isValid()) {
        QSqlQuery qr;

        if (m_primaryKey.value().isValid()) {
            qr = Query::remove(m_entity).where(QString("%1 = ?").arg(m_primaryKey.name()), QVariantList() << m_primaryKey.value()).make();
            if (qr.exec()) {
                if (qr.numRowsAffected() > 0) {
                    m_primaryKey.setValue(QVariant());
                    m_currentState = StateNew;
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

void ActiveRecord::setPrimaryKey(const QVariant &pk_value)
{
    if (isValid()) {
        m_primaryKey.setValue(pk_value);
        m_currentState = StateUnsaved;
    }
}

QString ActiveRecord::primaryKey() const
{
    return m_primaryKey.name();
}

bool ActiveRecord::load(const QVariant &pk_value)
{
    if (isValid()) {
        if (m_currentState != StateInvalid) {
            QSqlQuery qr = Query::select(m_entity).where(QString("%1 = ?").arg(m_primaryKey.name()), QVariantList() << m_primaryKey.value()).make();
            if (qr.exec()) {
                if (qr.next()) {
                    m_primaryKey.setValue(pk_value);
                    for (int i=0; i < qr.record().count(); i++) {
                        QSqlField field = qr.record().field(i);
                        m_values[field.name()] = field.value();
                    }
                    m_currentState = StateLoaded;
                    return true;
                } else {
                    m_lastError = "object does not persists";
                }
            } else {
                m_lastError = qr.lastError().text();
            }
        }
    }

    return false;

}

bool ActiveRecord::reload()
{
    return load(m_primaryKey.value());
}
