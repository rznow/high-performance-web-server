CREATE DATABASE IF NOT EXISTS miniforum
DEFAULT CHARACTER SET utf8mb4;


USE miniforum;


-- 用户表
DROP TABLE IF EXISTS comments;
DROP TABLE IF EXISTS post_like;
DROP TABLE IF EXISTS posts;
DROP TABLE IF EXISTS user_info;

CREATE TABLE IF NOT EXISTS user_info 
(
    user_id INT AUTO_INCREMENT PRIMARY KEY,

    user_name VARCHAR(32) NOT NULL UNIQUE,

    password VARCHAR(128) NOT NULL,

    create_time TIMESTAMP DEFAULT CURRENT_TIMESTAMP,

    deleted TINYINT(1) NOT NULL DEFAULT 0
);



-- 帖子表
CREATE TABLE posts
(
    post_id         INT PRIMARY KEY AUTO_INCREMENT,

    user_id         INT NOT NULL,

    title           VARCHAR(128) NOT NULL,

    content         TEXT NOT NULL,

    like_count      INT NOT NULL DEFAULT 0,

    comment_count   INT NOT NULL DEFAULT 0,

    view_count      INT NOT NULL DEFAULT 0,

    create_time     TIMESTAMP DEFAULT CURRENT_TIMESTAMP,

    update_time     TIMESTAMP DEFAULT CURRENT_TIMESTAMP
                    ON UPDATE CURRENT_TIMESTAMP,

    deleted         TINYINT(1) NOT NULL DEFAULT 0,

    FOREIGN KEY (user_id)
        REFERENCES user_info(user_id)
        ON DELETE CASCADE
);

-- 点赞表
CREATE TABLE post_like
(
    user_id INT NOT NULL,

    post_id INT NOT NULL,

    create_time TIMESTAMP DEFAULT CURRENT_TIMESTAMP,

    PRIMARY KEY (user_id, post_id),

    FOREIGN KEY (user_id)
        REFERENCES user_info(user_id)
        ON DELETE CASCADE,

    FOREIGN KEY (post_id)
        REFERENCES posts(post_id)
        ON DELETE CASCADE
);

--评论表
CREATE TABLE comments
(
    comment_id      INT AUTO_INCREMENT PRIMARY KEY,

    post_id         INT NOT NULL,

    user_id         INT NOT NULL,

    parent_id       INT DEFAULT 0 COMMENT '父评论, 0表示一级评论',

    reply_user_id   INT DEFAULT NULL COMMENT '回复的用户',

    content         TEXT NOT NULL,

    like_count      INT DEFAULT 0,

    create_time     TIMESTAMP DEFAULT CURRENT_TIMESTAMP,

    deleted         TINYINT DEFAULT 0,

    FOREIGN KEY(post_id)
        REFERENCES posts(post_id)
        ON DELETE CASCADE,

    FOREIGN KEY(user_id)
        REFERENCES user_info(user_id)
        ON DELETE CASCADE,

    FOREIGN KEY(reply_user_id)
        REFERENCES user_info(user_id)
        ON DELETE SET NULL
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