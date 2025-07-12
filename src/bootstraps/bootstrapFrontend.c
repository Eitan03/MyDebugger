#include "bootstrapFrontend.h"
#include "utils/utils.h"

#include <stdarg.h>

struct my_windowLayoutVerticalParams codeWindowLayoutParams;
struct my_windowLayoutVerticalParams messagesWindowLayoutParams;
struct my_windowLayoutGridParams windowGridParams;

struct Window codeWindow = {};
struct Window messagesWindow = {};
struct Window registersWindow = {};

LinkedList *messagesWindowText = NULL;

char **registersText = NULL; /* allocated */

// used for the LinkedList FreeValueFunction
void freeString(void *str, void *arg)
{
    my_free((char *)str);
}

void addMessageToMessagesWindow(char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    char *msg = (char *)my_malloc(128 * sizeof(char));
    validateErrno(msg == NULL, "malloc");
    vsnprintf(msg, 128, fmt, args);
    datatypes_linkedList_add(messagesWindowText, msg, 0);
}

void initWindows(int instructionCount, char **instructionsText)
{

    codeWindowLayoutParams = (struct my_windowLayoutVerticalParams){
        .isLinesNumbered = false,
        .isNumberedHex = false,
        .numberedLineStartIndex = 0};
    messagesWindowLayoutParams = (struct my_windowLayoutVerticalParams){
        .isLinesNumbered = false,
        .isNumberedHex = false,
        .numberedLineStartIndex = 0};
    windowGridParams = (struct my_windowLayoutGridParams){.horizontal_lines = 3};

    codeWindow.posX = 0;
    codeWindow.posY = 0;
    codeWindow.width = fe_width() / 2;
    codeWindow.height = fe_height();
    codeWindow.title = "Assembly Code";
    codeWindow.textsNum = instructionCount;
    codeWindow.texts.array = (const char **)instructionsText;
    codeWindow.isTextList = false;
    codeWindow.layout_type = MY_WINDOW_LAYOUT_TYPE_VERTICAL;
    codeWindow.layoutParams = &codeWindowLayoutParams;

    messagesWindowText = datatypes_linkedList_create(NULL, freeString, NULL, NULL);

    messagesWindow.posX = codeWindow.width;
    messagesWindow.posY = 0;
    messagesWindow.width = fe_width() / 2;
    messagesWindow.height = fe_height() / 2;
    messagesWindow.title = "Messages";
    messagesWindow.textsNum = 15;
    messagesWindow.texts.list = messagesWindowText;
    messagesWindow.isTextList = true;
    messagesWindow.layout_type = MY_WINDOW_LAYOUT_TYPE_VERTICAL;
    messagesWindow.layoutParams = &messagesWindowLayoutParams;

    registersWindow.posX = codeWindow.width;
    registersWindow.posY = messagesWindow.height;
    registersWindow.width = fe_width() / 2;
    registersWindow.height = fe_height() / 2;
    registersWindow.title = "Registers";
    registersWindow.textsNum = REGISTERS_NUMBER;
    registersWindow.texts.array = (const char **)registersText;
    registersWindow.isTextList = false;
    registersWindow.layout_type = MY_WINDOW_LAYOUT_TYPE_GRID;
    registersWindow.layoutParams = &windowGridParams;
}

void drawFrontend()
{
    fe_clear();

    fe_drawWindow(&codeWindow);
    fe_drawWindow(&messagesWindow);
    fe_drawWindow(&registersWindow);

    fe_present();
    fe_execute_events();
}