#ifndef POST_H
#define POST_H

#include <string>
#include <iostream>
#include <mysql/mysql.h>
struct Post
{
    int post_id;
    int user_id;
    std::string author;
    std::string title;
    std::string content;
    int like_count;
    int comment_count;
    int view_count;
    std::string create_time;

    Post():post_id(-1){};
    Post(const Post&p) = default;
    Post& operator=(const Post&p) = default;
    Post(MYSQL_ROW row)
    {
        int idx = 0;
        post_id       = std::stoi(row[idx++]);
        user_id       = std::stoi(row[idx++]);      
        author        = row[idx++];
        title         = row[idx++];
        content       = row[idx++];
        like_count    = std::stoi(row[idx++]);
        comment_count = std::stoi(row[idx++]);
        view_count    = std::stoi(row[idx++]);
        create_time   = row[idx++];
    };

    void print()
    {
        std::cout<<"post_id:\t"<<post_id<<std::endl;
        std::cout<<"user_id:\t"<<user_id<<std::endl;
        std::cout<<"author:\t\t"<<author<<std::endl;
        std::cout<<"title:\t\t"<<title<<std::endl;
        std::cout<<"content:\t"<<content<<std::endl;
        std::cout<<"like_count:\t"<<like_count<<std::endl;
        std::cout<<"comment_count:\t"<<comment_count<<std::endl;
        std::cout<<"view_count:\t"<<view_count<<std::endl;
        std::cout<<"create_time:\t"<<create_time<<std::endl;
        std::cout<<std::endl;
    }
};
#endif