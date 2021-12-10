#ifndef _KRONOS_ERROR_H_GUARD_
#define _KRONOS_ERROR_H_GUARD_

#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define KRONOS_SUCCESS 0
#define KRONOS_NOMEM ENOMEM
#define KRONOS_BAD_REQUEST EBADMSG

typedef int KRONOS_ERROR;

#define IS_ERROR(x) ((err = x) != KRONOS_SUCCESS)

extern int get_errno();
extern void throw_err(int err, char *type);
extern void print_err(int err, char *type);

#endif