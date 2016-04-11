#ifndef ACTIVERECORD_H
#define ACTIVERECORD_H

#include "orm-qt_global.h"

#include <QDebug>

#include <QObject>
#include <QSqlDatabase>
#include <QSqlDriver>
#include <QSqlRecord>
#include <QSqlField>
#include <QSqlIndex>
#include <QSqlError>
#include <QSqlQuery>
#include <QStringList>
#include <QVector>

#include "query.h"

namespace orm {

class ORMQTSHARED_EXPORT ActiveRecord : public QObject
{
    Q_OBJECT

/*main*/
public:
    explicit ActiveRecord(QObject *parent = 0);
    explicit ActiveRecord(const ActiveRecord&, QObject *parent = 0);
    explicit ActiveRecord(const QString&/*entity*/, QObject *parent = 0);
    ~ActiveRecord();

public:
    enum States { StateInvalid, StateNew, StateLoaded, StateSaved, StateUnsaved };

/*operators*/
public:
    ActiveRecord&           operator=(const ActiveRecord&);

/*public functions*/
public:
    bool                    load(const QVariant&/*PK value*/);
    bool                    reload();

    bool                    save();
    bool                    remove();

    void                    setPrimaryKey(const QVariant&/*PK value*/);
    QString                 primaryKey() const;

    QVariant                value(const QString&/*field*/) const;
    void                    setValue(const QString&/*field*/, const QVariant&/*value*/);

    const QSqlRecord        makeRecord() const;
    QMap<QString, QVariant> values() const;
    void                    setValues(const QSqlRecord&);

    const QString&          entity() const;

    bool                    isPersistent() const;
    bool                    isValid() const;

    QString                 lastError() const;

/*private functions*/
private:
    void                    swap(const ActiveRecord&);
    void                    init();

/*members*/
private:
    QString                 m_entity;
    QMap<QString, QVariant> m_values;

    QSqlField               m_primaryKey;

    States                  m_currentState;

    QString                 m_lastError;

};

}

#endif // ACTIVERECORD_H
