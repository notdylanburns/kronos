#ifndef HTTP_H_GUARD_
#define HTTP_H_GUARD_

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <math.h>

enum HTTPMethod { METHOD_NONE, GET, POST, PUT, DELETE };
enum StatusCode {
    STATUS_NONE,

    //INFORMATIONAL
    CONTINUE=100,
    SWITCHING_PROTOCOL=101,
    PROCESSING=102,
    EARLY_HINTS=103,

    //SUCCESSFUL
    OK=200,
    CREATED=201,
    ACCEPTED=202,
    NON_AUTHORITATIVE_INFORMATION=203,
    NO_CONTENT=204,
    RESET_CONTENT=205,
    PARTIAL_CONTENT=206,
    MULTI_STATUS=207,
    ALREADY_REPORTED=208,
    IM_USED=226,

    //REDIRECTION
    MULTIPLE_CHOICE=300,
    MOVED_PERMANENTLY=301,
    FOUND=302,
    SEE_OTHER=303,
    NOT_MODIFIED=304,
    USE_PROXY=305, // DEPRECATED
  //UNUSED=306,
    TEMPORARY_REDIRECT=307,
    PERMANENT_REDIRECT=308,

    //CLIENT ERROR
    BAD_REQUEST=400,
    UNAUTHORIZED=401,
    PAYMENT_REQUIRED=402,
    FORBIDDEN=403,
    NOT_FOUND=404,
    METHOD_NOT_ALLOWED=405,
    NOT_ACCEPTABLE=406,
    PROXY_AUTHENTICATION_REQUIRED=407,
    REQUEST_TIMEOUT=408,
    CONFLICT=409,
    GONE=410,
    LENGTH_REQUIRED=411,
    PRECONDITION_FAILED=412,
    PAYLOAD_TOO_LARGE=413,
    URI_TOO_LONG=414,
    UNSUPPORTED_MEDIA_TYPE=415,
    RANGE_NOT_SATISFIABLE=416,
    EXPECTATION_FAILED=417,
    IM_A_TEAPOT=418,
    MISDIRECTED_REQUEST=421,
    UNPROCESSABLE_ENTITY=422,
    LOCKED=423,
    FAILED_DEPENDENCY=424,
    TOO_EARLY=425,
    UPGRADE_REQUIRED=426,
    PRECONDITION_REQUIRED=427,
    TOO_MANY_REQUESTS=429,
    REQUEST_HEADER_FIELDS_TOO_LARGE=431,
    UNAVAILABLE_FOR_LEGAL_REASONS=451,

    //SERVER ERROR
    INTERNAL_SERVER_ERROR=500,
    NOT_IMPLEMENTED=501,
    BAD_GATEWAY=502,
    SERVICE_UNAVAILABLE=503,
    GATEWAY_TIMEOUT=504,
    HTTP_VERSION_NOT_SUPPORTED=505,
    VARIANT_ALSO_NEGOTIATES=506,
    INSUFFICIENT_STORAGE=507,
    LOOP_DETECTED=508,
    NOT_EXTENDED=510,
    NETWORK_AUTHENTICATION_REQUIRED=511
};

struct URLParam {
    char *key;
    char *value;
};

extern struct URLParam *new_urlparam(char *key, char *value);
extern void destroy_urlparam(struct URLParam *param);

struct HTTPHeader {
    char *key;
    char *value;
};

extern struct HTTPHeader *new_httpheader(char *key, char *value);
extern void destroy_httpheader(struct HTTPHeader *header);

typedef char * MimeType;
typedef uint8_t * Bytes;

struct HTTPRequest {
    enum HTTPMethod method;
    char *route;
    struct URLParam **params;
    int paramCount;
    char *version;
    /*char *host;
    char *user_agent;
    int content_length;
    MimeType content_type;*/
    struct HTTPHeader **headers;
    int header_count;
    uint8_t *body;
};

extern struct HTTPRequest *new_httprequest();
extern struct HTTPRequest *build_httprequest(int socketfd);
extern void destroy_httprequest(struct HTTPRequest *req);
extern char *get_urlparam(struct HTTPRequest *req, char *key);

extern struct HTTPHeader *get_header(struct HTTPRequest *req, char *header_name);

struct HTTPResponse {
    char *version;
    enum StatusCode status;
    char *status_msg;
    struct HTTPHeader **headers;
    int header_count;
    /*char *server;
    char *date;
    MimeType content_type;
    int content_length;*/
    uint8_t *body;
};

extern struct HTTPResponse *new_httpresponse();
extern char *build_httpresponse(struct HTTPResponse *res);
extern void destroy_httpresponse(struct HTTPResponse *res);

extern int set_status(struct HTTPResponse *res, char *version, enum StatusCode status, char *status_msg);
extern int set_content(struct HTTPResponse *res, MimeType content_type, int content_length, Bytes content);
extern int write_header(struct HTTPResponse *res, char *header_name, char *header_value);

#endif
