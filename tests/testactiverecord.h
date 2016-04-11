#ifndef TESTACTIVERECORD_H
#define TESTACTIVERECORD_H

#include <QtTest>

#include "activerecord.h"

class TestActiveRecord: public QObject
{
    Q_OBJECT

private:
    QString m_dbFile;
    QSqlDatabase m_db;

private slots:
    void initTestCase();
    void cleanupTestCase();

    void save_data();
    void save();
};

#endif // TESTACTIVERECORD_H
