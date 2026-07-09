CREATE DATABASE IF NOT EXISTS miniforum
DEFAULT CHARACTER SET utf8mb4;


USE miniforum;


-- 用户表
CREATE TABLE IF NOT EXISTS user_info 
(
    user_id INT AUTO_INCREMENT PRIMARY KEY,

    user_name VARCHAR(32) NOT NULL UNIQUE,

    password VARCHAR(128) NOT NULL,

    create_time TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);



-- 帖子表
CREATE TABLE IF NOT EXISTS posts
(
    post_id INT AUTO_INCREMENT PRIMARY KEY,

    user_id INT NOT NULL,

    title VARCHAR(128) NOT NULL,

    content TEXT NOT NULL,

    create_time TIMESTAMP DEFAULT CURRENT_TIMESTAMP,


    FOREIGN KEY(user_id)
    REFERENCES user_info(user_id)

);

-- 公告表
CREATE TABLE IF NOT EXISTS notices
(
    notice_id INT AUTO_INCREMENT PRIMARY KEY,

    title VARCHAR(100),

    content TEXT,

    create_time TIMESTAMP DEFAULT CURRENT_TIMESTAMP,

    expire_time TIMESTAMP NULL
);