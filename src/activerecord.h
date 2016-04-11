#ifndef ACTIVERECORD_H
#define ACTIVERECORD_H

#include "orm-qt_global.h"

#include <QObject>
#include <QSqlRecord>

#include "query.h"

namespace orm {

class ORMQTSHARED_EXPORT ActiveRecord
{
public:
    ActiveRecord(const QString &/*entity*/, const QString &connection = QString());

public:
    bool load(const QVariant &/*value*/);
    bool save();
    bool remove();

    void setRecord(const QSqlRecord &);
    const QSqlRecord &record() const;

    void setValue(const QString &/*name*/, const QVariant &/*val*/);
    QVariant value(const QString &/*name*/) const;

    const QString &entity() const;
    const QSqlField &primaryKey() const;
    const QString &lastError() const;

    bool isValid() const;
    bool isPersistent() const;

private:
    void init();
    const QSqlDatabase& db();

private:
    QString    m_connectionName;

    QString    m_entity;
    QSqlField  m_primaryKey;
    QSqlRecord m_record;

    bool       m_isValid;
    bool       m_isPersistent;

    QString    m_lastError;

};

}

#endif // ACTIVERECORD_H
