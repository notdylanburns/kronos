#include "http.h"

void bad_request(char *msg, struct HTTPRequest *req) {
    destroy_httprequest(req);
    perror(msg);
    exit(EXIT_FAILURE);
}

struct HTTPRequest *new_httprequest() {
    struct HTTPRequest *req = malloc(sizeof(struct HTTPRequest));
    if (req == NULL) return NULL;
    req->method = METHOD_NONE;
    req->route = NULL;
    req->params = NULL;
    req->paramCount = 0;
    req->version = NULL;
    req->headers = NULL;
    req->header_count = 0;
    req->body = NULL;
    return req;
}

struct HTTPHeader *new_httpheader(char *key, char *value) {
    struct HTTPHeader *header = malloc(sizeof(struct HTTPHeader));
    if (header == NULL) return NULL;

    header->key = calloc(strlen(key) + 1, 1);
    header->value = calloc(strlen(value) + 1, 1);
    if (header->key == NULL || header->value == NULL) return NULL;

    strcpy(header->key, key);
    strcpy(header->value, value);

    return header;
}

void destroy_httpheader(struct HTTPHeader *header) {
    free(header->key);
    header->key = NULL;
    free(header->value);
    header->value = NULL;
    free(header);
    header = NULL;
}

int extract_method(int socketfd, struct HTTPRequest *req) {
    char nextByte;
    char *method = malloc(1);
    int methodLen = 0;
    while (1) {
        read(socketfd, &nextByte, 1);
        if (nextByte == ' ') {
            break;
        } else {
            methodLen++;
            method = realloc(method, methodLen + 1);
            method[methodLen - 1] = nextByte;
            method[methodLen] = '\0';
        }
    }

    if (strcmp(method, "GET") == 0) {
        req->method = GET;
    } else if (strcmp(method, "POST") == 0) {
        req->method = POST;
    } else if (strcmp(method, "PUT") == 0) {
        req->method = PUT;
    } else if (strcmp(method, "DELETE") == 0) {
        req->method = DELETE;
    } else { 
        free(method);
        return 1;
    }

    free(method);

    return 0;
}

struct URLParam *new_urlparam(char *key, char *value) {
    struct URLParam *param = malloc(sizeof(struct URLParam));
    if (param == NULL) return NULL;

    param->key = key;
    param->value = value;
    return param;
}

void destroy_urlparam(struct URLParam *param) {
    free(param->key);
    param->key = NULL;
    free(param->value);
    param->value = NULL;
    free(param);
    param = NULL;
}

void add_param(struct HTTPRequest *req, struct URLParam *param) {
    req->paramCount++;
    req->params = realloc(req->params, req->paramCount * sizeof(struct URLParam *));
    req->params[req->paramCount - 1] = param;
}

char *readParamKey(int socketfd) {
    char nextByte;
    char *key = malloc(1);
    int keyLen = 0;
    while (1) {
        read(socketfd, &nextByte, 1);
        if (nextByte == ' ') {
            return NULL;
        } else if (nextByte == '=') {
            break;
        } else {
            keyLen++;
            key = realloc(key, keyLen + 1);
            key[keyLen - 1] = nextByte;
            key[keyLen] = '\0';
        }
    }
    return key;
}

int readParamValue(int socketfd, struct URLParam *param) {
    char nextByte;
    char *value = malloc(1);
    int valueLen = 0;
    while (1) {
        read(socketfd, &nextByte, 1);
        if (nextByte == ' ') {
            param->value = value;
            return 0; //params done
        } else if (nextByte == '&') {
            param->value = value;
            return 1; //more params
        } else {
            valueLen++;
            value = realloc(value, valueLen + 1);
            value[valueLen - 1] = nextByte;
            value[valueLen] = '\0';
        }
    }
    return 0;
}

int extract_params(int socketfd, struct HTTPRequest *req) {
    char *key;
    while (1) {
        key = readParamKey(socketfd);
        if (key == NULL) {
            break;
        };
        struct URLParam *p = new_urlparam(key, NULL);
        int moreParams = readParamValue(socketfd, p);
        add_param(req, p);
        
        if (!moreParams) break; 
    }
    return 0;
}

int extract_route(int socketfd, struct HTTPRequest *req) {
    char nextByte;
    char *route = malloc(1);
    int routeLen = 0;
    while (1) {
        read(socketfd, &nextByte, 1);
        if (nextByte == ' ') {
            break;
        } else if (nextByte == '?') {
            extract_params(socketfd, req);
            break;
        } else {
            routeLen++;
            route = realloc(route, routeLen + 1);
            route[routeLen - 1] = nextByte;
            route[routeLen] = '\0';
        }
    }

    req->route = route;
    return 0;

}

int extract_version(int socketfd, struct HTTPRequest *req) {
    char nextByte;
    char *version = malloc(1);
    int versionLen = 0;
    while (1) {
        read(socketfd, &nextByte, 1);
        if (nextByte == '\r') {
            read(socketfd, &nextByte, 1); //discard newline
            break;
        } else {
            versionLen++;
            version = realloc(version, versionLen + 1);
            version[versionLen - 1] = nextByte;
            version[versionLen] = '\0';
        }
    }

    req->version = version;
    return 0;
}

char *readKey(int socketfd) {
    char nextByte;
    char *key = malloc(1);
    if (key == NULL) return NULL;
    int keyLen = 0;
    while (1) {
        read(socketfd, &nextByte, 1);
        if (nextByte == ':') {
            read(socketfd, &nextByte, 1); //skip whitespace
            break;
        } else {
            keyLen++;
            key = realloc(key, keyLen + 1);
            key[keyLen - 1] = nextByte;
            key[keyLen] = '\0';
            if (strcmp(key, "\r\n") == 0) return NULL;
        }
    }

    return key;
}

char *readValue(int socketfd) {
    char nextByte;
    char *value = malloc(1);
    if (value == NULL) return NULL;
    value[0] = '\0';
    int valueLen = 0;
    while (1) {
        read(socketfd, &nextByte, 1);
        if (nextByte == '\r') {
            read(socketfd, &nextByte, 1); //discard newline
            break;
        } else {
            valueLen++;
            value = realloc(value, valueLen + 1);
            value[valueLen - 1] = nextByte;
            value[valueLen] = '\0';
        }
    }

    return value;
}

struct HTTPRequest *build_httprequest(int socketfd) {
    struct HTTPRequest *req = new_httprequest();
    if (req == NULL) return NULL;

    if (extract_method(socketfd, req) > 0) {
        bad_request("Failed to extract method from HTTP request", req);
    } else if (extract_route(socketfd, req) > 0) {
        bad_request("Failed to extract route from HTTP request", req);
    } else if (extract_version(socketfd, req) > 0) {
        bad_request("Failed to extract version from HTTP request", req);
    } 

    while (1) {
        char *key = readKey(socketfd);
        if (key == NULL) {
            //End of headers
            break;
        }

        /*if (strcmp(key, "Host:") == 0) {
            req->host = readValue(socketfd);
        } else if (strcmp(key, "User-Agent:") == 0) {
            req->user_agent = readValue(socketfd);
        } else if (strcmp(key, "Content-Length:") == 0) {
            req->content_length = atoi(readValue(socketfd));
        } else if (strcmp(key, "Content-Type:") == 0) {
            req->content_type = readValue(socketfd);
        } else {
            readValue(socketfd);
        }*/

        char *value = readValue(socketfd);
        struct HTTPHeader *header = new_httpheader(key, value);
        free(key);
        key = NULL;
        free(value);
        value = NULL;
        if (header == NULL) return NULL;
        req->header_count++;
        req->headers = realloc(req->headers, req->header_count * sizeof(struct HTTPHeader *));
        if (req->headers == NULL) return NULL;
        req->headers[req->header_count - 1] = header;
    }

    /*if (req->content_length == 0) {
        return req;
    } else {
        //char continue100[13] = "100 Continue";
        //send(socketfd, continue100, 13, 0);

        req->body = calloc(req->content_length + 1, 1);
        read(socketfd, req->body, req->content_length);

    }*/

    return req;

}

char *get_urlparam(struct HTTPRequest *req, char *key) {
    for (int i = 0; i < req->paramCount; i++) {
        if (strcmp(req->params[i]->key, key) == 0) {
            char *returnVal = calloc(strlen(req->params[i]->value) + 1, 1);
            strcpy(returnVal, req->params[i]->value);
            return returnVal;
        }
    }
    return NULL;
}

void destroy_httprequest(struct HTTPRequest *req) {
    free(req->route);
    req->route = NULL;
    free(req->version);
    req->version = NULL;
    free(req->body);
    req->body = NULL;
    for (int i = 0; i < req->paramCount; i++) destroy_urlparam(req->params[i]);
    free(req->params);
    req->params = NULL;
    for (int i = 0; i < req->header_count; i++) destroy_httpheader(req->headers[i]);
    free(req);
    req = NULL;
}

struct HTTPResponse *new_httpresponse() {
    struct HTTPResponse *res = malloc(sizeof(struct HTTPResponse));
    if (res == NULL) return NULL;
    res->version = NULL;
    res->status = STATUS_NONE;
    res->status_msg = NULL;
    /*res->date = NULL;
    res->server = NULL;
    res->content_type = NULL;
    res->content_length = 0;*/
    res->body = NULL;
    res->headers = NULL;
    res->header_count = 0;
    return res;
}

struct HTTPHeader *get_resheader(struct HTTPResponse *res, char *header_name) {
    for (int i = 0; i < res->header_count; i++) {
        if (strcmp(res->headers[i]->key, header_name) == 0) return res->headers[i];
    }
    return NULL;
}

char *build_httpresponse(struct HTTPResponse *res) {
    int header_length = (strlen(res->version) + 1 + floor(log10(abs(res->status))) + 2 + strlen(res->status_msg) + 2); // length of header line
    for (int i = 0; i < res->header_count; i++) {
        header_length += (strlen(res->headers[i]->key) + strlen(res->headers[i]->value) + 4); // header + ": " + "\r\n"
    }

    char headers[header_length + 1];

    sprintf(headers, "%s %d %s\r\n", res->version, res->status, res->status_msg);

    for (int i = 0; i < res->header_count; i++) {
        strcat(headers, res->headers[i]->key);
        strcat(headers, ": ");
        strcat(headers, res->headers[i]->value);
        strcat(headers, "\r\n");
    };

    char *response = NULL;
    int response_length = header_length;
    struct HTTPHeader *content_len = get_resheader(res, "Content-Length");
    if (res->body != NULL || content_len != NULL) {
        response_length += (atoi(content_len->value) + 2);
        response = calloc(response_length + 1, 1);
        if (response == NULL) return NULL;
        strcat(response, headers);
        strcat(response, "\r\n");
        strcat(response, (char *)res->body);
        // Has potential to cause heap corruption due to buffer overflow
        // with additional null terminator sprintf(response, "%s\r\n%s", headers, res->body);
    } else {
        response = calloc(response_length + 1, 1);
        if (response == NULL) return NULL;
        memcpy(response, headers, response_length);
    }

    return response;
    
}

void destroy_httpresponse(struct HTTPResponse *res) {
    free(res->version);
    res->version = NULL;
    free(res->status_msg);
    res->status_msg = NULL;
    for (int i = 0; i < res->header_count; i++) destroy_httpheader(res->headers[i]);
    free(res->headers);
    res->headers = NULL;
    free(res->body);
    res->body = NULL;
    free(res);
    res = NULL;
}

int set_status(struct HTTPResponse *res, char *version, enum StatusCode status, char *status_msg) {
    int len = strlen(version) + 1;
    res->version = realloc(res->version, len);
    if (res->version == NULL) return 1;
    strcpy(res->version, version);
    res->version[len - 1] = '\0';

    res->status = status;

    len = strlen(status_msg) + 1;
    res->status_msg = realloc(res->status_msg, len);
    if (res->status_msg == NULL) return 1;
    strcpy(res->status_msg, status_msg);
    res->status_msg[len - 1] = '\0';

    return 0;
}

int set_content(struct HTTPResponse *res, MimeType content_type, int content_length, Bytes content) {

    int digits = floor((int)log10((int)abs(content_length))) + 1;
    char *content_len_s = calloc(digits + 1, 1);
    if (content_len_s == NULL) return 1;
    sprintf(content_len_s, "%d", content_length);
    if (write_header(res, "Content-Type", content_type) != 0) return 1;
    if (write_header(res, "Content-Length", content_len_s) != 0) return 1;
    free(content_len_s);
    res->body = realloc(res->body, content_length + 1);
    if (res->body == NULL) return 1;
    memcpy(res->body, content, content_length);

    return 0;

}

int write_header(struct HTTPResponse *res, char *header_name, char *header_value) {
    struct HTTPHeader *old_header = get_resheader(res, header_name);
    if (old_header == NULL) {
        res->header_count++;
        res->headers = realloc(res->headers, res->header_count * sizeof(struct HTTPHeader));
        if (res->headers == NULL) return 1;
        struct HTTPHeader *new_header = new_httpheader(header_name, header_value);
        if (new_header == NULL) return 1;
        res->headers[res->header_count - 1] = new_header;
    } else {
        old_header->value = realloc(old_header->value, strlen(header_value) + 1);
        if (old_header->value == NULL) return 1;
        strcpy(old_header->value, header_value);  
    }
    return 0;
}