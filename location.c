#include <fcgi_stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include "dbcommand.h"

void getKeyAndValue(char* pair, struct key_value *kv) {
	syslog(LOG_WARNING, "getKeyAndValue");
	char* temp;
	temp = strsep(&pair, "=");
	strcpy(kv->key, temp);
	if(pair != NULL){
		temp = strsep(&pair, "=");
		strcpy(kv->value, temp);
	}
}

void sep_request(char *request, struct key_value kvs[]) {
	int n = 0;
	char* temp = NULL;
	while(request) {
		temp = strsep(&request, "&");
		getKeyAndValue(temp, kvs + n);
		n++;		
	}
}

char* getValue(char *key, struct key_value kvs[]) {
	int i = 0;
	for(; i < MAX_PARAM_COUNT; i++) {
		if(strcmp(key, kvs[i].key) == 0) {
			return kvs[i].value;
		}
	}
	return NULL;
}

int process_request(char *request) {
	syslog(LOG_WARNING, "process_requset");	
	struct key_value params[MAX_PARAM_COUNT];

	if(request == NULL)
		return -1;

	sep_request(request, params);
	
	char *type = getValue("type", params);	

	if(type == NULL)
		return -1;

	if(strcmp(type, "gps") == 0) {
		printf("Content-type: text/xml\r\n\r\n");
		printf("<result>ok</result>\n");
		reportGps(params);	
	}
	else if(strcmp(type, "bs") == 0) {
		printf("Content-type: text/xml\r\n\r\n");
		printf("<result>ok</result>\n");
		reportBs(params);
	}
	else if(strcmp(type, "loc") == 0) {
		printf("Content-type: text/xml\r\n\r\n");
		getAllLocation();
	}
	return 0;
}

int main()
{
	int ret;
	int contentLength;
	int temp;
	char *method;
	char *request;
	int recycle = 0;
	setvbuf(stdin, NULL, _IONBF, 0);
	while (FCGI_Accept() >= 0) {
		method = getenv("REQUEST_METHOD");
		if(method == NULL) {
			printf("Status: 200 OK\r\n");
			printf("Content-type:text/xml\r\n\r\n");
			printf("<result>no request</result>\n");
		}
		
		if(strcasecmp(method, "POST")==0){

			char *leng = getenv("CONTENT_LENGTH");
			if(leng != NULL) {
				contentLength = atoi(leng);
			} else {
				contentLength = 0;
			}
			
			request = (char *)malloc(contentLength + 1);
			memset(request, 0, contentLength + 1);
			fgets(request, contentLength + 1, stdin);
			recycle = 1;
		}
		else {
			recycle = 0;
			request = getenv("QUERY_STRING");
		}
		ret = process_request(request);
		if(recycle)
			free(request);
	}
	return ret;
}
