#include <mysql/mysql.h>
#include <string>
#include <vector>

class UserInfo;
class Post;
class Comment;
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
        const std::string& db = "miniforum",
        int port = 3306);

    bool query(const std::string& sql);

    bool reconnect();

    int loginSQL(const std::string& name, const std::string& password, UserInfo& user);

    int registerSQL(const std::string& name, const std::string& password);

    int savePost(Post& p);

    void saveComment(Comment& c);

    MYSQL* get();

    void getPosts(std::vector<Post>& posts, size_t size,size_t offset);

    void getRootComments(std::vector<Comment>& comments, size_t post_id, size_t size, size_t offset);

    void getComments(std::vector<Comment>& comments, size_t post_id);

    bool getPost(int post_id, Post& p);

    bool delPost(int post_id);

    int like(int post_id, int user_id, bool& liked);

    bool liked(int post_id, int user_id);

    int view(int post_id);

    bool checkPost(int post_id, int user_id);

    bool modPost(int post_id, std::string title, std::string content);
};