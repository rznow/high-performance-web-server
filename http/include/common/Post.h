#ifndef POST_H
#define POST_H

#include <string>
struct Post
{
    int post_id;
    int user_id;
    std::string user_name;
    std::string title;
    std::string content;
};
#endif