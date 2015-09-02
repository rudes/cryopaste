#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <stdio.h>


// Variables for curl_formadd
struct curl_httppost* paste_post = NULL;
struct curl_httppost* paste_last = NULL;
// Variables for curl_easy_setopt
CURL *curl = NULL;
//char url[] = "http://cryopaste.com/";
char url[] = "http://localhost/upload";
long port = 8080;


// Function declerations
void parse_files(char* s);
void usage(char* s);
