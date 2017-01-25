/*
创建表格user
*/
CREATE TABLE user(
    ID INTEGER PRIMARY KEY AUTOINCREMENT,
    name VARCHAR (30),
    money INTEGER,
	--createdtime TimeStamp NOT NULL DEFAULT CURRENT_TIMESTAMP
	createdtime TimeStamp NOT NULL DEFAULT (datetime('now','localtime'))
    );

/*
插入数据
*/
INSERT INTO user(name,money) VALUES('wu',10000);
INSERT INTO user(name,money) VALUES('zhou',20000);
INSERT INTO user(name,money) VALUES('jing',9000);
INSERT INTO user(name,money) VALUES('justin',7600);
