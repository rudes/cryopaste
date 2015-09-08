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
		} else if (strcmp(argv[i], "-v") == 0) {
			verbose = 1;
		} else {
			if ((curl = curl_easy_init()) != NULL) {
				for (i = 1; i < argc; i++) {
					parse_files(argv[i]);
				}
				fileset = 1;
				if (verbose) {
					printf("Setting URL: %s for curl...\n", url);
				}
				if ((err = curl_easy_setopt(curl, CURLOPT_URL, url)) != CURLE_OK) {
					fprintf(stderr, "Unable to set URL: %s ERROR: %s\n", url, curl_easy_strerror(err));
					exit(1);
				}
				if (verbose) {
					printf("Setting PORT: %lu for curl...\n", port);
				}
				if ((err = curl_easy_setopt(curl, CURLOPT_PORT, port)) != CURLE_OK) {
					fprintf(stderr, "Unable to set PORT: %lu ERROR: %s\n", port, curl_easy_strerror(err));
					exit(1);
				}
				if (verbose) {
					printf("Setting POST to curl...\n");
				}
				if ((err = curl_easy_setopt(curl, CURLOPT_HTTPPOST, paste_post)) != CURLE_OK) {
					fprintf(stderr, "Unable to set POST for HTTPPOST, ERROR: %s\n", curl_easy_strerror(err));
					exit(1);
				}
				if (verbose) {
					printf("Pushing data to URL: %s\n", url);
				}
				if ((err = curl_easy_perform(curl)) != CURLE_OK) {
					fprintf(stderr, "Failed to submit post to %s, ERROR: %s\n", url, curl_easy_strerror(err));
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
	if (strcmp(s, "-v") == 0) {
		return;
	}
	if (strcmp(s, "-h") == 0) {
		return;
	}
	if (verbose) {
		printf("Parsing %s...\n", s);
	}
	if(is_binary(s)) {
		return;
	}
	if(access(s, F_OK) != -1) {
		if (verbose) {
			printf("Adding %s to form...\n", s);
		}
		if ((err = curl_formadd(&paste_post, &paste_last, CURLFORM_COPYNAME, "file",
				CURLFORM_FILE, s, CURLFORM_END)) != CURLE_OK) {
			fprintf(stderr, "Failed to add FILE: %s, ERROR: %s\n", s, formadd_error(err));
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
cryopaste will skip binary files,\n \
we don't want to fill up the server with accidental binaries\n \
if you do wildcards (./*) it will skip directories as well.\n \
we recommend you do wildcards for each directory\n \
(src/* include/*)\n\n \
Each file is an ID on the webpage\n \
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

// Check for non-ascii chars in the file
// should mean it's a binary.
int is_binary(char* f) {
	int ch;
	int r = 0;
	FILE *fp = fopen(f, "r");
	if (verbose) {
		printf("Checking if %s is binary...\n", f);
	}
	if (fp != NULL) {
		while(1)
		{
			ch = fgetc(fp);
			if(feof(fp)) { break; }
			if(!isascii(ch)) {
				printf("%s is a binary File, skipping..\n", f);
				r = 1;
				break;
			}
		}
		fclose(fp);
	} else {
		fprintf(stderr, "Unable to open FILE: %s, ERROR: %s\n", f, strerror(errno));
	}
	return r;
}

