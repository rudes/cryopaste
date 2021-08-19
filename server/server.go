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
	STATIC_URL    = "/templates/static/"
	CODE_ROOT     = "/app/code/"
	STATIC_ROOT   = "/app/templates/static/"
	TEMPLATE_ROOT = "/app/templates/"
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
			http.ServeFile(w, r, TEMPLATE_ROOT+"/home.tmpl")
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
		err = os.MkdirAll(CODE_ROOT+hish, os.ModeDir|0744)
		if err != nil {
			http.Error(w, err.Error(), http.StatusInternalServerError)
			return
		}
		files := m.File["file"]
		for i := range files {
			f, err := files[i].Open()
			if err != nil {
				http.Error(w, err.Error(), http.StatusInternalServerError)
				return
			}
			defer f.Close()
			d, err := os.Create(CODE_ROOT + hish + "/" + files[i].Filename)
			if err != nil {
				http.Error(w, err.Error(), http.StatusInternalServerError)
				return
			}
			defer d.Close()
			if _, err := io.Copy(d, f); err != nil {
				http.Error(w, err.Error(), http.StatusInternalServerError)
				return
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
	root := filepath.Join(CODE_ROOT, l)
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

	t, err := template.ParseFiles(TEMPLATE_ROOT + "base.tmpl")
	if err != nil {
		http.Error(w, err.Error(), http.StatusInternalServerError)
		return
	}
	err = t.Execute(w, D)
	if err != nil {
		http.Error(w, err.Error(), http.StatusInternalServerError)
		return
	}
}
