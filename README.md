# cryopaste - Command line Paste Bin

cryopaste is a command line tool that sends your code files
to the [cryopaste.com](http://cryopaste.com/) server
the server is written in [Go](http://golang.org/)
and the client is written in [C](http://www.cprogramming.com/)

### Installation

You can download the tarball for the
client source code at [cryopaste.com](http://cryopaste.com/)
You will need [libcurl](http://curl.haxx.se/libcurl/) to install it,
you can get it from your package manager

```bash
wget cryopaste.com/cryopaste.tar
tar -zxvf cryopaste.tar
cd cryopaste
make
make install
```

If you're looking to host your own instance you'll need to do some tweaking.

You'll need [git](https://git-scm.com/),
[docker](https://www.docker.com/),
[libcurl](https://curl.haxx.se/libcurl/),
[gcc](https://gcc.gnu.org/), and
[make](https://www.gnu.org/software/make/).

```bash
# Clone the repo
git clone http://lab.cryocorp.co/rudes/cryopaste.com.git
cd cryopaste.com
# Replace the URL in line 18 with your domain.
# Also replace the port if you're changing the port on line 19.
$EDITOR client/inc/main.h
# Build the client for yourself.
cd client
make && make install && make clean
# Build the tarbal for your clients
cd ../
tar -czf server/templates/static/cryopaste.tar client
# Build and deploy the docker image
cd server
docker build -t .
docker run -d -p 8080:8080 --name=cryopaste cryopaste
# You should be good to go now.
```
