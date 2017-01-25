#include <stdio.h>
#include <string.h>
#include <sqlite3.h>
#include "sqliteHelper.h"


char sql[128];
sqlite3 *db;


int main()
{
	// 1）打开db
	int nResult = sqlite3_open("douniugame.db",&db);
    if (nResult != SQLITE_OK)
    {
		printf("[main]open sqlite3 failed:%s\n",sqlite3_errmsg(db));
        return 0;
    }
    else
    {
        printf("[main]open sqlite3 success\n");
    }

	// 2）创建表
	/*char* errmsg;
    memset(sql, 0, sizeof(sql));
    strcpy(sql, "create table user(id INTEGER PRIMARY KEY AUTOINCREMENT, name VARCHAR (30), money INTEGER);");
    sqlite3_exec(db, sql, NULL, NULL, &errmsg);
  
	// 3）insert
	memset(sql, 0, sizeof(sql));
	strcpy(sql, "begin;\n");
	int i=0;
    for (i=0;i<3;i++)
    {
		strcat(sql, "insert into tb(data) values('line');\n");
    }
    strcat(sql, "commit;");
	nResult = sqlite3_exec(db,sql,NULL,NULL, &errmsg);
	if (nResult != SQLITE_OK)
    {
		printf("[main]open sqlite3 failed:%s\n",errmsg);
		sqlite3_close(db);
        return 0;
    }*/
	
	// 4）select
	memset(sql, 0, sizeof(sql));
    strcpy(sql, "select * from user where name = 'justin'");//strcpy(sql, "select * from user");
	//nResult = select_by_callback(db, sql);
	nResult = select_by_table(db, sql);
	if (nResult == -1)
	{
		sqlite3_close(db);
	}
  
	// 5）关闭db
    sqlite3_close(db);
    return 0;
}