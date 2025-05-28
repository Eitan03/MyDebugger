#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <sys/ptrace.h>

void validateErrno(int err, char *command);

#endif // UTILS_H