#include "utils.h"

void validateErrno(int err, char *command)
{
    if (err != 0)
    {
        perror(command);
        exit(1);
    }
}