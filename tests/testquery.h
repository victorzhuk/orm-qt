#ifndef TESTQUERY_H
#define TESTQUERY_H

#include <QtTest>

#include "query.h"

class TestQuery : public QObject
{
    Q_OBJECT

private:
    QString m_dbFile;
    QSqlDatabase m_db;

private slots:
    void initTestCase();
    void cleanupTestCase();

    void testSimpleSelect();
    void testJoinSelect();
    void testSelfJoinSelect();
    void testWhereCondition();
};

#endif // TESTQUERY_H
