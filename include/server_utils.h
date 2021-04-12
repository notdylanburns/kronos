#ifndef SERVER_UTILS_H_GUARD_
#define SERVER_UTILS_H_GUARD_

#include <stdio.h>
#include "server.h"
#include "http.h"

#define ROUTE(n) void n(struct HTTPRequest *req, struct HTTPResponse *res)
#define STATIC(n, filepath, mime) void n(struct HTTPRequest *req, struct HTTPresponse *res) { FILE *f = fopen(filepath, "rb"); if (f == NULL) { printf("Failed to open %s\n", filepath); return; }; fseek(f, 0, SEEK_END); int filesize = ftell(f); fseek(f, 0, SEEK_SET); char fileContents[filesize]; fread(fileContents, 1, filesize, f); set_content(res, mime, filesize, (Bytes)fileContents); }
#define RESOURCE(s, p, f) route(server, GET, p, &f)

#endif