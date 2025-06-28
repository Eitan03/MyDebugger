#include <stdio.h>

#define TB_IMPL
#include "./bootstraps/bootstrapFrontend.h"
#include "./bootstraps/bootstrapMPT.h"

int main(int argc, char *argv[])
{
    printf("running on process %ld\n", (long)getpid());
    getc(stdin);

    if (argc < 2)
    {
        printf("Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    traceeContext = mpt_initTrace(argv[1], (const char *)&(argv[2]));

    fe_init();

    mpt_listenToChild(traceeContext, childExecHandler, childSignalHandler);
    freeMPT();

    fe_exit();

    return 0;
}
