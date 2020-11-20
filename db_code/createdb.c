#include <stdio.h>
#include <stdlib.h>

// sudo apt install default-libmysqlclient-dev
#include <mysql/mysql.h>

int main(int argc, char **argv)
{
    MYSQL *con = mysql_init(NULL);

    if (con == NULL)
    {
        fprintf(stderr, "%s\n", mysql_error(con));
        exit(1);
    }

    if (mysql_real_connect(con, "localhost", "root", "password",
                           NULL, 0, NULL, 0) == NULL)
    {
        fprintf(stderr, "%s\n", mysql_error(con));
        mysql_close(con);
        exit(1);
    }

    if (mysql_query(con, "CREATE DATABASE testdb"))
    {
        fprintf(stderr, "%s\n", mysql_error(con));
        mysql_close(con);
        exit(1);
    }

    // if (mysql_query(con, "DROP DATABASE testdb"))
    // {
    //     fprintf(stderr, "%s\n", mysql_error(con));
    //     mysql_close(con);
    //     exit(1);
    // }

    mysql_close(con);
    exit(0);
}

// http://zetcode.com/db/mysqlc/