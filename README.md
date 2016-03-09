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
