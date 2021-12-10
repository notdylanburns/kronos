#include "kronos-http.h"

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

KRONOS_ERROR extract_method(struct StringStream *ss, enum HTTPMethod *m) {
    for (;;) {
        char method_string[16] = {'\0'};
        char c = get(ss);
        switch (c) {
            case ' ':
                if (strcmp(method_string, "GET") == 0) {
                    *m = GET;
                } else if (strcmp(method_string, "POST") == 0) {
                    *m = POST;
                } else if (strcmp(method_string, "PUT") == 0) {
                    *m = PUT;
                } else if (strcmp(method_string, "DELETE") == 0) {
                    *m = DELETE;
                } else return KRONOS_BAD_REQUEST;
                return KRONOS_SUCCESS;
                break;

            case '\0': 
                return KRONOS_BAD_REQUEST;
                break;

            default:
                if (!isalpha(c)) return KRONOS_BAD_REQUEST;
                strcat(method_string, &c);
        }
    }
}

KRONOS_ERROR extract_route(struct StringStream *ss, char **r) {
    for (;;) {
        int len = 0;
        char *route = NULL;
        char c = get(ss);
        switch (c) {
            case ' ':
            case '?':
                *r = route; 
                return KRONOS_SUCCESS;
                break;

            case '\0': 
                free(route);
                return KRONOS_BAD_REQUEST;
                break;

            default:
                if (!isalpha(c)) {
                    free(route);
                    return KRONOS_BAD_REQUEST;
                }
                char *tmp = realloc(route, ++len);
                if (tmp == NULL) {
                    free(route);
                    return KRONOS_NOMEM;
                }
                route = tmp;
        }
    }
}

KRONOS_ERROR extract_params(struct StringStream *ss, struct URLParamMap **params) {
    *params = new_param_map(0, NULL);
    if (*params == NULL) return KRONOS_NOMEM;
    
    char pairsremain = 1;
    while (pairsremain) {
        int keylen = 0;
        char inkey = 1, isfirst = 1;
        char *key = NULL;
        char c;
        while (inkey) {
            c = get(ss);
            switch (c) {
                case '=':
                    inkey = 1; 
                    return KRONOS_SUCCESS;
                    break;
                
                case ' ':
                    if (isfirst)
                        return KRONOS_SUCCESS; 
                    free(key);
                    return KRONOS_BAD_REQUEST;
                    break;

                case '\0': 
                    free(key);
                    return KRONOS_BAD_REQUEST;
                    break;

                default:
                    if (!isalpha(c)) return KRONOS_BAD_REQUEST;
                    char *tmp = realloc(key, ++keylen);
                    if (tmp == NULL) {
                        free(key);
                        return KRONOS_NOMEM;
                    }
                    key = tmp;
            }
        }
        int vallen = 0; 
        char inval = 1;
        char *value = NULL;
        while (inval) {
            c = get(ss);
            switch (c) {
                case ' ':
                    pairsremain = 0;
                    inval = 0;
                    break;

                case '&':
                    inval = 0;
                    break;

                case '\0':
                    free(key);
                    free(value);
                    return KRONOS_BAD_REQUEST;
                    break;

                default:
                    if (!isalpha(c)) return KRONOS_BAD_REQUEST;
                    char *tmp = realloc(value, ++vallen);
                    if (tmp == NULL) {
                        free(key);
                        free(value);
                        return KRONOS_NOMEM;
                    }
                    value = tmp;

            }
        }
        struct URLParam *up = new_urlparam(key, value);
        free(key);
        free(value);
        if (up == NULL) {
            return KRONOS_NOMEM;
        }

        add_url_param(*params, up);
    }
    return KRONOS_SUCCESS;
}

KRONOS_ERROR extract_version(struct StringStream *ss, char **v) {
    for (;;) {
        int len = 0;
        char *version = NULL;
        char c = get(ss);
        switch (c) {
            case ' ':
                *v = version; 
                return KRONOS_SUCCESS;
                break;

            case '\0': 
                free(version);
                return KRONOS_BAD_REQUEST;
                break;

            default: ;
                char *tmp = realloc(version, ++len);
                if (tmp == NULL) {
                    free(version);
                    return KRONOS_NOMEM;
                }
                version = tmp;
        }
    }
}

KRONOS_ERROR build_httprequest(struct HTTPRequest **req, char *req_string, uint len) {
    *req = new_httprequest();
    if (*req == NULL) return KRONOS_NOMEM;

    struct StringStream *ss = new_stringstream(req_string, len);
    if (ss == NULL) {
        destroy_httprequest(*req);
        return KRONOS_NOMEM;
    }

    int err;
    if (IS_ERROR(extract_method(ss, &((*req)->method)))) {
        destroy_stringstream(ss);
        destroy_httprequest(*req);
        return err;
    }

    if (IS_ERROR(extract_route(ss, &((*req)->route)))) {
        destroy_stringstream(ss);
        destroy_httprequest(*req);
        return err;
    }

    if (IS_ERROR(extract_params(ss, &((*req)->params)))) {
        destroy_stringstream(ss);
        destroy_httprequest(*req);
        return err;
    }

    if (IS_ERROR(extract_version(ss, &((*req)->version)))) {
        destroy_stringstream(ss);
        destroy_httprequest(*req);
        return err;
    }

    while (1) {
        /*char *key = readKey(socketfd);
        if (key == NULL) {
            //End of headers
            break;
        }

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
        req->headers[req->header_count - 1] = header;*/
    }

    return KRONOS_SUCCESS;

}

char *get_param(struct HTTPRequest *req, char *key) {
    struct URLParam *up = get_url_param(req->params, key);
    if (up == NULL) return NULL;
    return up->value;
}

void destroy_httprequest(struct HTTPRequest *req) {
    free(req->route);
    req->route = NULL;
    free(req->version);
    req->version = NULL;
    free(req->body);
    req->body = NULL;
    for (int i = 0; i < req->paramCount; i++) destroy_param_map(req->params);
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
