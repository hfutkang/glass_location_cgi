#include <stdio.h>
#include <string.h>
#include <mysql/mysql.h>
#include "dbcommand.h"
#include <syslog.h>
#include <fcgi_stdio.h>

#define MAX_SQL_LENGTH		1024

static MYSQL mysql;
static int initialized = 0;

static int init_db_connect() {
	syslog(LOG_WARNING, "init_db_connect");	
	if(!initialized) {
		void *ret;
		mysql_init(&mysql);
		char arg = 1;
		mysql_options(&mysql, MYSQL_OPT_RECONNECT, &arg);
		ret = mysql_real_connect(&mysql,
					"127.0.0.1", "glass", "glass", "glass_location_info"
					, 3306, NULL, 0);
		if(ret != NULL) {
		
			fprintf(stderr, "connect db error\n");
			return -1;
		}
	}
	return 0;
}
int reportGps(struct key_value kvs[]) {
	syslog(LOG_WARNING, "reportGps");
	char cmdbuf[MAX_SQL_LENGTH];
	int length;
	int ret;

	char* sn = getValue("serial", kvs);
	char* lat = getValue("latitude", kvs);
	char* longit = getValue("longitude", kvs);
	char* time = getValue("time", kvs);
	
	init_db_connect();
	length = sprintf(cmdbuf, "insert location (sn, latitude, longitude, time) values(\"%s\", \"%s\", \"%s\", \"%s\") \
					on duplicate key update latitude=\"%s\",longitude=\"%s\",time=\"%s\"",
			sn, lat, longit, time, lat, longit, time);
	ret = mysql_real_query(&mysql, cmdbuf, length);
	return ret;
}

int reportBs(struct key_value kvs[]) {
	syslog(LOG_WARNING, "reportBs");
	
	char cmdbuf[MAX_SQL_LENGTH];
	int length;
	int ret;
	
	char* sn = getValue("serial", kvs);
	char* mmc = getValue("mmc", kvs);
	char* mnc = getValue("mnc", kvs);
	char* lac = getValue("lac", kvs);
	char* cid = getValue("cid", kvs);
	char* time = getValue("time", kvs);
	
	init_db_connect();
	
	length = sprintf(cmdbuf, "insert location (sn, mmc, mnc, lac, cid, time) values(\"%s\", \"%s\", \"%s\", \"%s\", \"%s\", \"%s\") on duplicate \
			key update mmc=\"%s\",mnc=\"%s\",lac=\"%s\",cid=\"%s\",time=\"%s\"", sn, mmc, mnc, lac, \
			cid, time, mmc, mnc, lac, cid, time);
	ret = mysql_real_query(&mysql, cmdbuf, length);
	return ret;
}

int getAllLocation() {
	syslog(LOG_WARNING, "getAllLocation");
	char cmdbuf[MAX_SQL_LENGTH];
	int length;
	int ret;
	MYSQL_RES *result;

	init_db_connect();
	
	length = sprintf(cmdbuf, "select * from location");
	
	ret = mysql_real_query(&mysql, cmdbuf, length);
	result = mysql_store_result(&mysql);
	
	if(result) {
		int row_num;
		int i;
		MYSQL_ROW row;
		row_num = mysql_num_rows(result);
		output_location(result, row_num);
//		for(i = 0; i < row_num; i++) {
//			row = mysql_fetch_row(result);
//			char temp[128];
//			sprintf(temp, "%s,%s,%s,%s,%s,%s,%s,%s", row[0],row[1],row[2],row[3],row[4],row[5],row[6],row[7]);
//			syslog(LOG_WARNING, temp);
//		}
		mysql_free_result(result);
		return row_num;
	}
	return -1;
}

int output_location(MYSQL_RES *result, int row_num) {
	syslog(LOG_WARNING, "output_location");
	MYSQL_ROW row;
	printf("<result>\n");
	int i;
	for(i = 0; i<row_num; i++) {
		row = mysql_fetch_row(result);
		printf("<location>\n");
		printf("<sn>%s</sn>\n", row[0] == NULL?"":row[0]);
		printf("<latitude>%s</latitude>\n", row[1] == NULL?"":row[1]);
		printf("<longitude>%s</longitude>\n", row[2] == NULL?"":row[2]);
		printf("<mmc>%s</mmc>\n", row[3] == NULL?"":row[3]);
		printf("<mnc>%s</mnc>\n", row[4] == NULL?"":row[4]);
		printf("<lac>%s</lac>\n", row[5] == NULL?"":row[5]);
		printf("<cid>%s</cid>\n", row[6] == NULL?"":row[6]);
		printf("<time>%s</time>\n", row[7] == NULL?"":row[7]);
		printf("</location>\n");
	}
	printf("</result>\n");
	return 0;
}
