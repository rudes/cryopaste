#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <curl/curl.h>
#include <stdio.h>


/* Variables for curl_formadd */
struct curl_httppost* paste_post = NULL;
struct curl_httppost* paste_last = NULL;

/* Variables for curl_easy_setopt */
CURL *curl = NULL;
char url[] = "http://cryopaste.com/";
long port = 80;

/* If files have alredy been handled */
int fileset = 0;
/* If verbose flag is set */
int verbose = 0;

/* Version */
int CRYO_MAJOR_VERSION = 1;
int CRYO_MINOR_VERSION = 0;
int CRYO_PATCH_VERSION = 1;

/* Function declerations */
void parse_files(char* s);
void usage(char* s);
const char *formadd_error(CURLFORMcode err);
int is_binary(char* f);
int is_directory(char* f);
void kill_curl(CURL * curl);
