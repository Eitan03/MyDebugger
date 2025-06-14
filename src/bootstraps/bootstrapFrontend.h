#ifndef BOOTSTRAP_FRONTEND_H
#define BOOTSTRAP_FRONTEND_H

#include "frontend/frontend.h"

#include "MyPtrace/MyPtrace.h"

extern struct my_windowLayoutVerticalParams codeWindowLayoutParams;
extern struct my_windowLayoutVerticalParams upRightWindowLayoutParams;
extern struct my_windowLayoutGridParams windowGridParams;

extern struct Window codeWindow;
extern struct Window upRightWindow;
extern struct Window bottomRightWindow;

extern char *upRightWindowText[];

extern char **registersText; /* allocated */

void initWindows(int instructionCount, const char **instructionsText, uint64_t instructionLineStart);
void drawFrontend();

#endif // BOOTSTRAP_FRONTEND_H