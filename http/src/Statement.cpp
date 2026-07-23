#include "mysql/Statement.h"


Statement::Statement(MYSQL* conn, const std::string& sql)
{
    stmt = mysql_stmt_init(conn);

    if(stmt == nullptr)
        throw std::runtime_error("mysql_stmt_init failed");

    if(mysql_stmt_prepare(
            stmt,
            sql.c_str(),
            sql.length()))
    {
        throw std::runtime_error(
            mysql_stmt_error(stmt));
    }

    int count = mysql_stmt_param_count(stmt);

    binds.resize(count);

    memset(binds.data(),0,sizeof(MYSQL_BIND)*count);

    intValues.resize(count);
    stringValues.resize(count);
}

void Statement::bindInt(int index, int value)
{
    intValues[index]=value;

    binds[index].buffer_type=MYSQL_TYPE_LONG;
    binds[index].buffer=&intValues[index];
}

void Statement::bindString(int index, const std::string& value)
{
    stringValues[index]=value;

    binds[index].buffer_type = MYSQL_TYPE_STRING;

    binds[index].buffer = (void*)stringValues[index].c_str();

    binds[index].buffer_length = stringValues[index].length();
}

void Statement::bindNull(int index)
{
    static char nullValue = 1;

    binds[index].buffer_type = MYSQL_TYPE_NULL;
    binds[index].buffer = nullptr;
    binds[index].is_null = reinterpret_cast<bool*>(&nullValue);
}

bool Statement::execute()
{
    if(mysql_stmt_bind_param(stmt, binds.data()))
    {
        std::cout<< mysql_stmt_error(stmt)<< std::endl;

        return false;
    }

    if(mysql_stmt_execute(stmt))
    {
        std::cout<< mysql_stmt_error(stmt)<< std::endl;

        return false;
    }

    return true;
}

Statement::~Statement()
{
    if(stmt)
    {
        mysql_stmt_close(stmt);
    }
}

bool Statement::storeResult()
{
    if(mysql_stmt_store_result(stmt))
    {
        std::cout << mysql_stmt_error(stmt) << std::endl;
        return false;
    }

    MYSQL_RES* meta = mysql_stmt_result_metadata(stmt);

    if(meta == nullptr)
        return false;

    int count = mysql_num_fields(meta);

    MYSQL_FIELD* fields = mysql_fetch_fields(meta);

    resultBinds.resize(count);

    resultBuffers.resize(count);

    lengths.resize(count);

    isNull.resize(count);

    memset(resultBinds.data(),0,sizeof(MYSQL_BIND)*count);

    for(int i=0;i<count;i++)
    {
        unsigned long len = fields[i].max_length;

        if(len == 0)
        {
            switch(fields[i].type)
            {
                case MYSQL_TYPE_LONG:
                    len = sizeof(int);
                    break;

                case MYSQL_TYPE_LONGLONG:
                    len = sizeof(long long);
                    break;

                default:
                    len = 4096;
            }
        }

        resultBuffers[i].resize(len + 1);

        resultBinds[i].buffer_type = fields[i].type;

        resultBinds[i].buffer = resultBuffers[i].data();

        resultBinds[i].buffer_length = resultBuffers[i].size();

        resultBinds[i].length = &lengths[i];

        resultBinds[i].is_null = reinterpret_cast<bool*>(&isNull[i]);
    }

    mysql_free_result(meta);

    // if(mysql_stmt_bind_result(stmt,resultBinds.data()))
    // {
    //     std::cout<<mysql_stmt_error(stmt)<<std::endl;
    //     return false;
    // }

    // currentRow.buffers = &resultBuffers;
    // currentRow.lengths = &lengths;
    // currentRow.nulls   = &isNull;
    // currentRow.binds = &resultBinds;

    return true;
}

// void Statement::bindResultInt(int index,int& value)
// {
//     if(resultBinds.size()<=index)
//     {
//         size_t oldSize = resultBinds.size();
//         resultBinds.resize(index + 1);
//         lengths.resize(index + 1);
//         isNull.resize(index + 1);

//         for (size_t i = oldSize; i < resultBinds.size(); ++i) {
//             memset(&resultBinds[i], 0, sizeof(MYSQL_BIND));
//         }
//     }

//     resultBinds[index].buffer_type=MYSQL_TYPE_LONG;
//     resultBinds[index].buffer=&value;

//     resultBinds[index].length=&lengths[index];

//     resultBinds[index].is_null=reinterpret_cast<bool*>(&isNull[index]);;
// }

// void Statement::bindResultString(int index, char* buf, unsigned long size)
// {
//     if(resultBinds.size()<=index)
//     {
//         size_t oldSize = resultBinds.size();
//         resultBinds.resize(index + 1);
//         lengths.resize(index + 1);
//         isNull.resize(index + 1);

//         for (size_t i = oldSize; i < resultBinds.size(); ++i) {
//             memset(&resultBinds[i], 0, sizeof(MYSQL_BIND));
//         }
//     }

//     resultBinds[index].buffer_type=MYSQL_TYPE_STRING;

//     resultBinds[index].buffer=buf;

//     resultBinds[index].buffer_length=size;

//     resultBinds[index].length=&lengths[index];

//     resultBinds[index].is_null=reinterpret_cast<bool*>(&isNull[index]);;
// }

bool Statement::fetch()
{
    if(mysql_stmt_bind_result(stmt, resultBinds.data()))
    {
        std::cout<<mysql_stmt_error(stmt)<<std::endl;

        return false;
    }

    int ret=mysql_stmt_fetch(stmt);

    if(ret==MYSQL_NO_DATA)
        return false;

    if(ret==1)
    {
        std::cout<<mysql_stmt_error(stmt)<<std::endl;

        return false;
    }

    currentRow.buffers = &resultBuffers;
    currentRow.lengths = &lengths;
    currentRow.nulls   = &isNull;

    return true;
}

my_ulonglong Statement::affectedRows() const
{
    return mysql_stmt_affected_rows(stmt);
}

my_ulonglong Statement::insertId() const
{
    return mysql_stmt_insert_id(stmt);
}

StatementRow& Statement::row()
{
    return currentRow;
}