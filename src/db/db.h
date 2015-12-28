#ifndef DB_H
#define DB_H

#include <QtSql/QSqlDatabase>

class Db
{
private:
    QSqlDatabase db;

public:
    Db();
    QSqlError initDb();
    std::vector<Channel*> readChannels();
    void writeChannels(std::vector<Channel>);
};

#endif // DB_H
