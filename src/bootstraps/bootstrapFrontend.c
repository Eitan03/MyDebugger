#include "bootstrapFrontend.h"

struct my_windowLayoutVerticalParams codeWindowLayoutParams;
struct my_windowLayoutVerticalParams upRightWindowLayoutParams;
struct my_windowLayoutGridParams windowGridParams;

struct Window codeWindow = {};
struct Window upRightWindow = {};
struct Window bottomRightWindow = {};

char *upRightWindowText[] = {"Welcome To My Debugger!"};

char **registersText = NULL; /* allocated */

void initWindows(int instructionCount, const char **instructionsText, uint64_t instructionLineStart)
{

    codeWindowLayoutParams = (struct my_windowLayoutVerticalParams){
        .isLinesNumbered = true,
        .isNumberedHex = true,
        .numberedLineStartIndex = instructionLineStart};
    upRightWindowLayoutParams = (struct my_windowLayoutVerticalParams){
        .isLinesNumbered = false,
        .isNumberedHex = false,
        .numberedLineStartIndex = 0};
    windowGridParams = (struct my_windowLayoutGridParams){.horizontal_lines = 3};

    codeWindow.posX = 0;
    codeWindow.posY = 0;
    codeWindow.width = fe_width() / 2;
    codeWindow.height = fe_height();
    codeWindow.title = "My Window";
    codeWindow.textsNum = instructionCount;
    codeWindow.texts = instructionsText;
    codeWindow.layout_type = MY_WINDOW_LAYOUT_TYPE_VERTICAL;
    codeWindow.layoutParams = &codeWindowLayoutParams;

    upRightWindow.posX = codeWindow.width;
    upRightWindow.posY = 0;
    upRightWindow.width = fe_width() / 2;
    upRightWindow.height = fe_height() / 2;
    upRightWindow.title = "Up Right Window";
    upRightWindow.textsNum = 1;
    upRightWindow.texts = (const char **)upRightWindowText;
    upRightWindow.layout_type = MY_WINDOW_LAYOUT_TYPE_VERTICAL;
    upRightWindow.layoutParams = &upRightWindowLayoutParams;

    bottomRightWindow.posX = codeWindow.width;
    bottomRightWindow.posY = upRightWindow.height;
    bottomRightWindow.width = fe_width() / 2;
    bottomRightWindow.height = fe_height() / 2;
    bottomRightWindow.title = "Registers";
    bottomRightWindow.textsNum = REGISTERS_NUMBER;
    bottomRightWindow.texts = (const char **)registersText;
    bottomRightWindow.layout_type = MY_WINDOW_LAYOUT_TYPE_GRID;
    bottomRightWindow.layoutParams = &windowGridParams;
}

void drawFrontend()
{
    fe_drawWindow(&codeWindow);
    fe_drawWindow(&upRightWindow);
    fe_drawWindow(&bottomRightWindow);

    fe_present();
    fe_execute_events();
}