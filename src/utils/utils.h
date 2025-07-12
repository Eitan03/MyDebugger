#ifndef UTILS_H
#define UTILS_H

#include <stdbool.h>
#include <stdio.h>
#include <sys/ptrace.h>

#ifndef my_malloc
#define my_malloc malloc
#define my_realloc realloc
#define my_free free
#endif

void validateErrno(int err, char *command);

#endif // UTILS_H