#ifndef SERVER_UTILS_H_GUARD_
#define SERVER_UTILS_H_GUARD_

#include <stdio.h>
#include "server.h"
#include "http.h"

#define ROUTE(name) void name(struct HTTPRequest *req, struct HTTPResponse *res)
#define STATIC(name, filepath, mime) void name(struct HTTPRequest *req, struct HTTPResponse *res) { \
                                      FILE *f = fopen(filepath, "rb"); \ 
                                      if (f == NULL) { \
                                          printf("Failed to open %s\n", filepath); \
                                          set_status(res, "HTTP/1.1", STATUS_INTERNAL_SERVER_ERROR, "Internal Server Error"); \ 
                                          return; \
                                      }; \ 
                                      fseek(f, 0, SEEK_END); \
                                      int filesize = ftell(f); \
                                      fseek(f, 0, SEEK_SET); \
                                      char fileContents[filesize + 1]; \
                                      fileContents[filesize] = '\0'; \
                                      fread(fileContents, 1, filesize, f); \
                                      set_content(res, mime, filesize, (Bytes)fileContents); \
                                  }
#define RESOURCE(server, path, filepath) route(server, GET, path, &filepath)

#endif