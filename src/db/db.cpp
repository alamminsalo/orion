#include "db.h"



Db::Db()
{

}

QSqlError Db::initDb()
{
    this->sql = QSqlDatabase::addDatabase("QSQLITE");
    this->sql.setDatabaseName(":memory:");

        if (!db.open())
            return db.lastError();

        QStringList tables = db.tables();
        if (tables.contains("books", Qt::CaseInsensitive)
            && tables.contains("authors", Qt::CaseInsensitive))
            return QSqlError();
}

std::vector<Channel *> Db::readChannels()
{

}

void Db::writeChannels(std::vector<Channel>)
{

}
