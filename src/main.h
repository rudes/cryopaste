#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <curl/curl.h>
#include <stdio.h>


// Variables for curl_formadd
struct curl_httppost* paste_post = NULL;
struct curl_httppost* paste_last = NULL;

// Variables for curl_easy_setopt
CURL *curl = NULL;
char url[] = "http://cryopaste.com/";
long port = 80;

// If files have alredy been handled
int fileset = 0;

// Function declerations
void parse_files(char* s);
void usage(char* s);
const char *formadd_error(CURLFORMcode err);
