#include "testquery.h"

void TestQuery::initTestCase()
{
    m_db = QSqlDatabase::database();

    QStringList sql_schema;
    sql_schema << "CREATE TABLE test_query ("
        << "id INTEGER NOT NULL,"
        << "parent_id INTEGER DEFAULT NULL,"
        << "field1 VARCHAR(255) NOT NULL,"
        << "field2 FLOAT NOT NULL,"
        << "PRIMARY KEY (id),"
        << "FOREIGN KEY (parent_id) REFERENCES test_query (id));";
    m_db.exec(sql_schema.join(' '));
    QVERIFY(!m_db.lastError().isValid());

    QStringList sql_join;
    sql_join << "CREATE TABLE test_join ("
        << "id INTEGER NOT NULL,"
        << "test_query_id INTEGER NOT NULL,"
        << "fieldjoin1 VARCHAR(255) NOT NULL,"
        << "field2 FLOAT NOT NULL,"
        << "PRIMARY KEY (id),"
        << "FOREIGN KEY (test_query_id) REFERENCES test_query (id));";
    m_db.exec(sql_join.join(' '));
    QVERIFY(!m_db.lastError().isValid());

    QStringList sql_data;
    sql_data << "INSERT INTO test_query (parent_id, field1, field2)"
         << "VALUES"
         << "(NULL, 'test1', 1.123),"
         << "(NULL, 'test2', 2.345),"
         << "(1, 'test3', 3.456);";
    m_db.exec(sql_data.join(' '));
    QVERIFY(!m_db.lastError().isValid());

    QStringList sql_join_data;
    sql_join_data << "INSERT INTO test_join (test_query_id, fieldjoin1, field2)"
         << "VALUES"
         << "(1, 'testjoin1', 3.2342),"
         << "(1, 'testjoin2', 2.1412),"
         << "(2, 'testjoin3', 1.4324);";
    m_db.exec(sql_join_data.join(' '));
    QVERIFY(!m_db.lastError().isValid());
}

void TestQuery::cleanupTestCase()
{
}

void TestQuery::testSimpleSelect()
{
    QSqlQuery qr = orm::Query::select("test_query").make();
    if (!qr.exec()) {
        qDebug() << qr.lastError().text();
    }
    QVERIFY(qr.isActive());
    QCOMPARE(qr.numRowsAffected(), 3);

    QSqlQuery qr2 = orm::Query::select("test_join").make();
    if (!qr2.exec()) {
        qDebug() << qr2.lastError().text();
    }
    QVERIFY(qr2.isActive());
    QCOMPARE(qr2.numRowsAffected(), 3);
}

void TestQuery::testJoinSelect()
{
    QSqlQuery qr = orm::Query::select("test_join").join("test_query", "test_query_id").make();
    if (!qr.exec()) {
        qDebug() << qr.lastError().text();
    }
    QVERIFY(qr.isActive());
    QVERIFY(qr.next());
    QCOMPARE(qr.value("test_query.field1").toString(), QString("test1"));
    QCOMPARE(qr.value("field2").toDouble(), 3.2342);
    QCOMPARE(qr.value("test_query.field2").toDouble(), 1.123);
}

void TestQuery::testSelfJoinSelect()
{
    QSqlQuery qr = orm::Query::select("test_query").join("LEFT JOIN test_query t2 ON test_query.parent_id = t2.id").where("t2.field1 = ?", "test1").make();
    if (!qr.exec()) {
        qDebug() << qr.lastQuery();
        qDebug() << qr.lastError().text();
    }
    QVERIFY(qr.isActive());
    QVERIFY(qr.next());
    QCOMPARE(qr.value("field1").toString(), QString("test3"));
    QCOMPARE(qr.value("field2").toDouble(), 3.456);
}

void TestQuery::testWhereCondition()
{
    QSqlQuery qr = orm::Query::select("test_query").where("field1 = ?", QVariantList() << "test2").make();
    if (!qr.exec()) {
        qDebug() << qr.lastError().text();
    }
    QVERIFY(qr.isActive());
    QVERIFY(qr.next());
    QCOMPARE(qr.value("field1").toString(),QString("test2"));
    QCOMPARE(qr.value("field2").toDouble(), 2.345);
}
