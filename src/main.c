#include "main.h"

int main(int argc, char* argv[]) {
	int i;
	if (argc > 1) {
		curl = curl_easy_init();
		for (i = 1; i < argc; i++) {
			parse_files(argv[i]);
		}
		printf("Pushing Files to Server.\n");
		curl_easy_setopt(curl, CURLOPT_URL, url);
		curl_easy_setopt(curl, CURLOPT_PORT, port);
		curl_easy_setopt(curl, CURLOPT_HTTPPOST, paste_post);
		curl_easy_perform(curl);
	} else {
		usage(argv[0]);
		exit(1);
	}
	return 0;
}

void parse_files(char* s) {
	printf("Adding %s...\n",s);
	curl_formadd(&paste_post, &paste_last, CURLFORM_COPYNAME, "file",
			CURLFORM_FILE, s, CURLFORM_END);
}

void usage(char* s) {
	printf("Usage: %s file1.f file2.f ...\n \
cryopaste takes files and sends them over http\n \
to cryopaste.com, the command will return a link.\n \
eg: cryopaste.com/abZfhd\n", s);
}
