//c99 create_database.c -o create_database `mysql_config --cflags --libs`

#include <mysql.h>
#include <stdio.h>
#include <stdlib.h>

int main()
{
	//printf("MySQL version is : %s\n", mysql_get_client_info());
	
	MYSQL *con = mysql_init(NULL);

	if (con == NULL)
	{
	  fprintf(stderr, "%s\n", mysql_error(con));
	  exit(1);
	}

	if (mysql_real_connect(con, "localhost", "raspberry", "pi", NULL, 0, NULL, 0) == NULL)
	{
	  fprintf(stderr, "%s\n", mysql_error(con));
	  mysql_close(con);
	  exit(1);
	}

	if (mysql_query(con, "CREATE DATABASE DHT11_SENSOR"))
	{
		fprintf(stderr, "%s\n", mysql_error(con));
		mysql_close(con);
		exit(1);
	}

	mysql_close(con);
 
	return 0;
}
