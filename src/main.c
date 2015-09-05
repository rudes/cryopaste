#include "main.h"

int main(int argc, char* argv[]) {
	int i;
	CURLcode err;

	if (argc < 2) {
		usage(argv[0]);
		exit(1);
	}

	for (i = 1; i < argc; i++) {
		if (fileset) {
			break;
		}
		if (strcmp(argv[i], "-h") == 0) {
			usage(argv[0]);
			exit(1);
		} else {
			if ((curl = curl_easy_init()) != NULL) {
				for (i = 1; i < argc; i++) {
					parse_files(argv[i]);
				}
				fileset = 1;
				if ((err = curl_easy_setopt(curl, CURLOPT_URL, url)) != CURLE_OK) {
					printf("Unable to set URL: %s ERROR: %s\n", url, curl_easy_strerror(err));
					exit(1);
				}
				if ((err = curl_easy_setopt(curl, CURLOPT_PORT, port)) != CURLE_OK) {
					printf("Unable to set PORT: %lu ERROR: %s\n", port, curl_easy_strerror(err));
					exit(1);
				}
				if ((err = curl_easy_setopt(curl, CURLOPT_HTTPPOST, paste_post)) != CURLE_OK) {
					printf("Unable to set POST for HTTPPOST, ERROR: %s\n", curl_easy_strerror(err));
					exit(1);
				}
				if ((err = curl_easy_perform(curl)) != CURLE_OK) {
					printf("Failed to submit post to %s, ERROR: %s\n", url, curl_easy_strerror(err));
					exit(1);
				}
				curl_easy_cleanup(curl);
			}
		}
	}
	return 0;
}

// Add files to the form from argv[i]
void parse_files(char* s) {
	CURLFORMcode err;
	if(access(s, F_OK) != -1) {
	if ((err = curl_formadd(&paste_post, &paste_last, CURLFORM_COPYNAME, "file",
			CURLFORM_FILE, s, CURLFORM_END)) != CURLE_OK) {
		printf("Failed to add FILE: %s, ERROR: %s\n", s, formadd_error(err));
	}
	} else {
		printf("Unable to access %s, please verify the file exists.\n", s);
	}
}

// Print usage for command
void usage(char* s) {
	printf("Usage: cryopaste file1.c file2.c ...\n\n \
cryopaste takes files and sends them over http\n \
to http://cryopaste.com, the command will return a link.\n\n \
eg: http://cryopaste.com/abZfhd\n\n \
Each file is a ID on the website\n \
so to specify a file you can do:\n\n \
http://cryopaste.com/abZfhd#file1.c\n\n");
}

// curl_formadd returns a CURLFORMcode but libcurl doesn't have a
// error handler for CURLFORMcode, this handles all the errors defined in curl/curl.h
const char *formadd_error(CURLFORMcode err) {
	switch (err) {
		case CURL_FORMADD_OK:
			return "Should be no issue, Formadd should have passed without issue.";
			break;
		case CURL_FORMADD_MEMORY:
			return "Memory Allocation failed.";
			break;
		case CURL_FORMADD_OPTION_TWICE:
			return "One option has been supplied twice for one Form.";
			break;
		case CURL_FORMADD_NULL:
			return "A null pointer was given for a string.";
			break;
		case CURL_FORMADD_UNKNOWN_OPTION:
			return "An unknown option was used.";
			break;
		case CURL_FORMADD_INCOMPLETE:
			return "Not enough data was provided for FormInfo to complete.";
			break;
		case CURL_FORMADD_ILLEGAL_ARRAY:
			return "Illegal Option is used in the Array.";
			break;
		case CURL_FORMADD_DISABLED:
			return "libcurl was built without the curl_formadd function support.";
			break;
		case CURL_FORMADD_LAST:
			return "Last Enum.";
			break;
	}
}
