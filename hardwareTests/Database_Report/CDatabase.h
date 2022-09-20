#ifndef CDATABASE_H
#define CDATABASE_H

#include <sqlite3.h>
#include <string>

//Database operation errors
#define OPEN_ERROR 1

//Database execution errors
#define CREATE_ERROR 4
#define INSERT_ERROR 5
#define REMOVE_ERROR 6
#define GET_ERROR 7

//DEFINES
#define INSERT_LENGTH 256

class CDatabase
{
    friend class CCaregiver;
    public:
        CDatabase();
        ~CDatabase();

        int insert(std::string table,std::string values);
        int update(std::string table,std::string att,std::string update,std::string primKey, std::string values);
        int remove(std::string table,std::string primKey,std::string values);
        int get(std::string table,std::string data,std::string& d_text);
        int print(std::string table);

    private:
        sqlite3 *db;
        std::string database;
        int rc;
        char *zErrMsg;

        static int callback(void *NotUsed,int count,char **data,char **columns);
        static int callbackPrint(void* data, int columnsNum, char** fields, char** columns);
};

#endif // CDATABASE_H
