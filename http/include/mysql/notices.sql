USE miniforum;

INSERT INTO notices(title, content, expire_time) 
VALUES
(
    '论坛公告',
    '欢迎来到 MiniForum！请文明发言，禁止发布违法信息。',
    NULL
);

INSERT INTO notices(title, content, expire_time)
VALUES
(
    '开发进展',
    '目前为止,账户的登录和注册,帖子的保存和缓存已完成,在进行帖子的获取和展开',
    NULL
);