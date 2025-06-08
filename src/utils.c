#include "utils.h"
#include <stdlib.h>

void validateErrno(int err, char *command)
{
    if (err != 0)
    {
        perror(command);

        exit(EXIT_FAILURE);
    }
}