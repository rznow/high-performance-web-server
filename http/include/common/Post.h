#ifndef POST_H
#define POST_H

#include <string>
#include <iostream>
struct Post
{
    int post_id;
    int user_id;
    std::string author;
    std::string title;
    std::string content;
    std::string create_time;

    void print()
    {
        std::cout<<"post_id:\t"<<post_id<<std::endl;
        std::cout<<"user_id:\t"<<user_id<<std::endl;
        std::cout<<"author:\t\t"<<author<<std::endl;
        std::cout<<"title:\t\t"<<title<<std::endl;
        std::cout<<"content:\t"<<content<<std::endl;
        std::cout<<"create_time:\t"<<create_time<<std::endl;
        std::cout<<std::endl;
    }
};
#endif