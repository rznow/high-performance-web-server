#ifndef STATEMENT_H
#define STATEMENT_H
#include <mysql/mysql.h>
#include <string>
#include <cstring>
#include <iostream>
#include <vector>

class Statement
{
public:
    Statement(MYSQL* conn,const std::string& sql);
    ~Statement();

    // 输入参数绑定
    void bindInt(int index,int value);
    void bindString(int index,const std::string& value);

    // 执行
    bool execute();

    // 查询结果
    bool storeResult();
    bool fetch();

    // 输出绑定
    void bindResultInt(int index,int& value);
    void bindResultString(int index,char* buf,unsigned long size);

    // 常用信息
    my_ulonglong affectedRows() const;
    my_ulonglong insertId() const;

private:

    MYSQL_STMT* stmt;

    std::vector<MYSQL_BIND> binds;
    std::vector<int> intValues;
    std::vector<std::string> stringValues;

    //--------------------------
    // result
    //--------------------------

    std::vector<MYSQL_BIND> resultBinds;

    std::vector<unsigned long> lengths;
    std::vector<char> isNull;
};

#endif