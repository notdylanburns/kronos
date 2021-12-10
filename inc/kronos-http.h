#ifndef _KRONOS_HTTP_H_GUARD_
#define _KRONOS_HTTP_H_GUARD_

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

#include "kronos-error.h"
#include "kronos-url.h"
#include "kronos-stringstream.h"

enum HTTPMethod { METHOD_NONE, GET, POST, PUT, DELETE };
enum StatusCode {
    STATUS_NONE,

    //INFORMATIONAL
    STATUS_CONTINUE=100,
    STATUS_SWITCHING_PROTOCOL=101,
    STATUS_PROCESSING=102,
    STATUS_EARLY_HINTS=103,

    //SUCCESSFUL
    STATUS_OK=200,
    STATUS_CREATED=201,
    STATUS_ACCEPTED=202,
    STATUS_NON_AUTHORITATIVE_INFORMATION=203,
    STATUS_NO_CONTENT=204,
    STATUS_RESET_CONTENT=205,
    STATUS_PARTIAL_CONTENT=206,
    STATUS_MULTI_STATUS=207,
    STATUS_ALREADY_REPORTED=208,
    STATUS_IM_USED=226,

    //REDIRECTION
    STATUS_MULTIPLE_CHOICE=300,
    STATUS_MOVED_PERMANENTLY=301,
    STATUS_FOUND=302,
    STATUS_SEE_OTHER=303,
    STATUS_NOT_MODIFIED=304,
    STATUS_USE_PROXY=305, // DEPRECATED
  //STATUS_UNUSED=306,
    STATUS_TEMPORARY_REDIRECT=307,
    STATUS_PERMANENT_REDIRECT=308,

    //CLIENT ERROR
    STATUS_BAD_REQUEST=400,
    STATUS_UNAUTHORIZED=401,
    STATUS_PAYMENT_REQUIRED=402,
    STATUS_FORBIDDEN=403,
    STATUS_NOT_FOUND=404,
    STATUS_METHOD_NOT_ALLOWED=405,
    STATUS_NOT_ACCEPTABLE=406,
    STATUS_PROXY_AUTHENTICATION_REQUIRED=407,
    STATUS_REQUEST_TIMEOUT=408,
    STATUS_CONFLICT=409,
    STATUS_GONE=410,
    STATUS_LENGTH_REQUIRED=411,
    STATUS_PRECONDITION_FAILED=412,
    STATUS_PAYLOAD_TOO_LARGE=413,
    STATUS_URI_TOO_LONG=414,
    STATUS_UNSUPPORTED_MEDIA_TYPE=415,
    STATUS_RANGE_NOT_SATISFIABLE=416,
    STATUS_EXPECTATION_FAILED=417,
    STATUS_IM_A_TEAPOT=418,
    STATUS_MISDIRECTED_REQUEST=421,
    STATUS_UNPROCESSABLE_ENTITY=422,
    STATUS_LOCKED=423,
    STATUS_FAILED_DEPENDENCY=424,
    STATUS_TOO_EARLY=425,
    STATUS_UPGRADE_REQUIRED=426,
    STATUS_PRECONDITION_REQUIRED=427,
    STATUS_TOO_MANY_REQUESTS=429,
    STATUS_REQUEST_HEADER_FIELDS_TOO_LARGE=431,
    STATUS_UNAVAILABLE_FOR_LEGAL_REASONS=451,

    //SERVER ERROR
    STATUS_INTERNAL_SERVER_ERROR=500,
    STATUS_NOT_IMPLEMENTED=501,
    STATUS_BAD_GATEWAY=502,
    STATUS_SERVICE_UNAVAILABLE=503,
    STATUS_GATEWAY_TIMEOUT=504,
    STATUS_HTTP_VERSION_NOT_SUPPORTED=505,
    STATUS_VARIANT_ALSO_NEGOTIATES=506,
    STATUS_INSUFFICIENT_STORAGE=507,
    STATUS_LOOP_DETECTED=508,
    STATUS_NOT_EXTENDED=510,
    STATUS_NETWORK_AUTHENTICATION_REQUIRED=511
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
    struct URLParamMap *params;
    int paramCount;
    char *version;
    struct HTTPHeader **headers;
    int header_count;
    uint8_t *body;
};

extern struct HTTPRequest *new_httprequest();
extern KRONOS_ERROR build_httprequest(struct HTTPRequest **req, char *req_string, uint len);
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
