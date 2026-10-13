#ifndef STATEMENT_H
#define STATEMENT_H
#include <mysql/mysql.h>
#include <string>
#include <cstring>
#include <iostream>
#include <vector>
#include "mysql/StatementRow.h"

/*
typedef struct st_mysql_bind
{
    unsigned long* length;          // 数据长度

    bool is_null;                  // 是否为 NULL

    void *buffer;                  // 数据缓冲区

    unsigned long buffer_length;   // buffer 的容量

    enum enum_field_types buffer_type; // 数据类型

    bool error;                    // 是否发生错误

    unsigned char *row_ptr;        // 内部使用

    void *store_param_func;        // 参数存储函数

    void *fetch_result;            // 结果获取函数

    void *skip_result;             // 跳过结果函数

    bool is_unsigned;              // 是否为 unsigned

    bool long_data_used;           // 是否使用 mysql_stmt_send_long_data

    void *extension;               // 扩展字段

} MYSQL_BIND;
*/

class Statement
{
public:
    Statement(MYSQL* conn,const std::string& sql);
    ~Statement();

    // 输入参数绑定
    void bindInt(int index,int value);
    void bindString(int index,const std::string& value);
    void bindNull(int index);

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

    //--------------------------
    // row
    //--------------------------
    StatementRow& row();

private:

    MYSQL_STMT* stmt;
    StatementRow currentRow;

    std::vector<MYSQL_BIND> binds;
    std::vector<int> intValues;
    std::vector<std::string> stringValues;

    //--------------------------
    // result
    //--------------------------

    std::vector<MYSQL_BIND> resultBinds;
    std::vector<std::vector<char>> resultBuffers;
    std::vector<unsigned long> lengths;
    std::vector<char> isNull;

};

#endif