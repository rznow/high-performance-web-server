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
        const std::string& host = "127.0.0.1",
        const std::string& user = "webserver",
        const std::string& password = "123456",
        const std::string& db = "webserver",
        int port = 3306);

    bool query(const std::string& sql);

    bool reconnect();

    int loginSQL(const std::string& name, const std::string& password);

    int registerSQL(const std::string& name, const std::string& password);

    MYSQL* get();
};