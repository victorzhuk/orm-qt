#include <QCoreApplication>
#include <QtTest>
#include <QFile>

#include "testquery.h"
#include "testactiverecord.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("test_database.sqlite");
    db.open();
    db.exec("CREATE DATABASE test_db");

    QTest::qExec(new TestQuery, argc, argv);
    QTest::qExec(new TestActiveRecord, argc, argv);

    if (QFile::exists("test_database.sqlite")) {
        QFile::remove("test_database.sqlite");
    }

    return 0;
}
