CREATE USER 'webserver'
IDENTIFIED BY 'password';


GRANT ALL PRIVILEGES
ON miniforum.*
TO 'webserver'@'localhost';


FLUSH PRIVILEGES;