#define MAX_KEY_LENGTH		32
#define MAX_VALUE_LENGTH	32
#define MAX_PARAM_COUNT		10
struct key_value {
	char key[MAX_KEY_LENGTH];	
	char value[MAX_KEY_LENGTH];
};

int reportGps(struct key_value kvs[]);
int reportBs(struct key_value kv[]);
int getAllLocation();
int getLocation(char *sn);
char* getValue(char* key, struct key_value kvs[]);
