SHELL=/bin/bash

main:
	gcc src/main.c -o cryopaste -lcurl -Iinc

install:
	if [ -a "/usr/local/bin" ]; then \
		mv cryopaste /usr/local/bin; \
	else \
		mkdir -p /usr/local/bin; \
		mv cryopaste /usr/local/bin; \
	fi

clean:
	rm -vf cryopaste
