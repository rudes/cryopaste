FROM golang

ADD . /go/src/github.com/rudes/cryopaste/
RUN go install github.com/rudes/cryopaste
ENTRYPOINT /go/bin/cryopaste

EXPOSE 8080
