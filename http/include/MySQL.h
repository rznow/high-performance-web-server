#include <mysql/mysql.h>
#include <string>
class MySQL
{
private:
    MYSQL* conn;
    
public:
    MySQL();
    ~MySQL();

    bool connect(
        const std::string& host,
        const std::string& user,
        const std::string& password,
        const std::string& db,
        int port);

    bool query(const std::string& sql);

    MYSQL_RES* storeResult();

    MYSQL* get();
};