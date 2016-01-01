package main

import (
	"crypto/md5"
	"fmt"
	"io"
	"log"
	"net/http"
	"os"
	"path/filepath"
	"strings"
	"text/template"
	"time"
)

const (
	STATIC_URL  = "/templates/static/"
	STATIC_ROOT = "/go/src/github.com/rudes/cryopaste/templates/static/"
)

type Files struct {
	Filename, Extension, Content string
}

type Data struct {
	Tag   string
	Files []Files
}

func main() {
	http.HandleFunc("/", handler)
	http.HandleFunc(STATIC_URL, StaticHandler)
	http.HandleFunc("/cryopaste.tar", DownloadHandler)
	log.Fatal(http.ListenAndServe(":8080", nil))
}

func handler(w http.ResponseWriter, r *http.Request) {
	log.Println("method:", r.Method)

	if r.Method == "GET" {
		// Do nothing for right now.
		if r.URL.Path == "/" {
			http.ServeFile(w, r, "/go/src/github.com/rudes/cryopaste/templates/home.tmpl")
			return
		} else {
			render(w, r, r.URL.Path)
		}
	} else {
		// Handle incoming files.
		err := r.ParseMultipartForm(100000)
		if err != nil {
			http.Error(w, err.Error(), http.StatusInternalServerError)
			return
		}

		m := r.MultipartForm

		h := md5.Sum([]byte(r.RemoteAddr + time.Now().String()))
		hish := fmt.Sprintf("%x", h[:3])
		os.MkdirAll("/go/src/github.com/rudes/cryopaste/code/"+hish, os.ModeDir|0744)
		files := m.File["file"]
		for i, _ := range files {
			f, err := files[i].Open()
			defer f.Close()
			if err != nil {
				http.Error(w, err.Error(), http.StatusInternalServerError)
				return
			}
			d, err := os.Create("/go/src/github.com/rudes/cryopaste/code/" + hish + "/" + files[i].Filename)
			defer d.Close()
			if err != nil {
				http.Error(w, err.Error(), http.StatusInternalServerError)
				return
			}
			if _, err := io.Copy(d, f); err != nil {
			}
		}
		fmt.Fprintf(w, "http://cryopaste.com/%s\n", hish)
	}
}

func StaticHandler(w http.ResponseWriter, req *http.Request) {
	sf := req.URL.Path[len(STATIC_URL):]
	if len(sf) != 0 {
		f, err := http.Dir(STATIC_ROOT).Open(sf)
		if err == nil {
			content := io.ReadSeeker(f)
			http.ServeContent(w, req, sf, time.Now(), content)
			return
		}
	}
	http.NotFound(w, req)
}

func DownloadHandler(w http.ResponseWriter, req *http.Request) {
	sf := req.URL.Path[1:]
	if len(sf) != 0 {
		f, err := http.Dir(STATIC_ROOT).Open(sf)
		if err == nil {
			content := io.ReadSeeker(f)
			http.ServeContent(w, req, sf, time.Now(), content)
			return
		}
	}
	http.NotFound(w, req)
}

func render(w http.ResponseWriter, r *http.Request, l string) {
	root := filepath.Join("/go/src/github.com/rudes/cryopaste/code/", l)
	f, err := os.Open(root)
	if err != nil {
		http.Error(w, err.Error(), http.StatusInternalServerError)
		return
	}
	defer f.Close()

	s, err := f.Readdirnames(-1)
	if err != nil {
		http.Error(w, err.Error(), http.StatusInternalServerError)
		return
	}

	D := Data{
		Tag: string([]byte(l[1:])),
	}

	for _, name := range s {
		file := filepath.Join(root, name)
		content, err := os.Open(file)
		if err != nil {
			http.Error(w, err.Error(), http.StatusInternalServerError)
			return
		}
		defer content.Close()
		stat, err := content.Stat()
		if err != nil {
			http.Error(w, err.Error(), http.StatusInternalServerError)
			return
		}
		text := make([]byte, stat.Size())
		_, err = content.Read(text)
		if err != nil {
			http.Error(w, err.Error(), http.StatusInternalServerError)
			return
		}
		txt := strings.Replace(string(text), "<", "&lt;", -1)
		txt = strings.Replace(txt, ">", "&gt;", -1)
		var ext string
		if strings.Contains(file, ".") {
			ext = filepath.Ext(file)
		} else {
			ext = filepath.Base(file)
		}
		fileData := Files{
			Filename:  name,
			Extension: ext,
			Content:   txt,
		}
		D.Files = append(D.Files, fileData)
	}

	// for _, end := range D.Files {
	// 	fmt.Fprintf(w, "%s\n", end.Content)
	// }

	t, err := template.ParseFiles("/go/src/github.com/rudes/cryopaste/templates/base.tmpl")
	if err != nil {
		http.Error(w, err.Error(), http.StatusInternalServerError)
		return
	}
	t.Execute(w, D)
}
