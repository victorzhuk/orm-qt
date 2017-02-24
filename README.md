## Missing Object Relation Mapping realisation in Qt framework

This library presents you a simple way to work with database tables like some script languages do.

### Features:
- easy use and integrate with your project
- full CRUD support with ActiveRecord pattern
- deep integration with Qt
- easy SQL select/insert/update/delete manipulation

### Example code:
Let we have created a table `foobar` with fields `foo` and `bar`:

~~~CPP
// ... other Qt includes
#include <activerecord.h>

// create empty record
orm::ActiveRecord ar_obj("foobar");

// set values
ar_obj.setValue("foo", "this is test string");
ar_obj.setValue("bar", 123);

// save this record
bool ok = ar_obj.save();

// get result primary key
QVariant pk = ar_obj.primaryKey().value();

// load second obj
orm::ActiveRecord ar2_obj("foobar");
ar2_obj.load(pk);

// get value
int val = ar2_obj.value("bar").toInt();

// remove object
ar2_obj.remove();
~~~

### Notes
"Where are other features?" you can ask. As I noted before, this is light library, all your need exists in Qt already.

For example you need to select many records. Your example code:
~~~CPP
QSqlQuery qr = orm::Query::select("foobar").where("id > ?", 100).make();
if (qr.exec()) {
        while (qr.next()) {
                orm::ActiveRecord ar_obj("foobar");
                ar_obj.setRecord(qr.record());
                ar_obj.setValue("foo", "new_foo_value");
                ar_obj.save();
        }
}
~~~

As you can see, any wrapper only complicate code. orm-qt aim to efficiently manipulate standart Qt classes and try not to reinvent the wheel.
Another example, you need to create some model. Your example code:
~~~CPP
// ...
QTableView *view = new QTableView;
QSqlQueryModel *model = new QSqlQueryModel;
// ...
QSqlQuery qr = orm::Query::select("foobar").where("id > ?", 100).make();
if (qr.exec()) {
        model->setQuery(qr);
        view->setModel(model);
}
// get ActiveRecord from 1st row
QSqlRecord rec = model.record(0);
if (!rec.isEmpty()) {
        orm::ActiveRecord ar_obj("foobar");
        ar_obj.setRecord(rec);
        // some actions...
}
~~~

If you work need any other functions or classies your can email me.
Unfortunately this time documentation is empty. I fix it neares time, you can see `test` folder for code examples.

### License
This code under MIT license  
Home: https://github.com/victorzhuk/orm-qt  
Author: Victor Zhuk <zhuk@protonmail.com>
