#ifndef BOOTSTRAP_FRONTEND_H
#define BOOTSTRAP_FRONTEND_H

#include "frontend/frontend.h"

#include "LinkedList/LinkedList.h"
#include "MyPtrace/MyPtrace.h"

extern struct my_windowLayoutVerticalParams codeWindowLayoutParams;
extern struct my_windowLayoutVerticalParams messagesWindowLayoutParams;
extern struct my_windowLayoutGridParams windowGridParams;

extern struct Window codeWindow;
extern struct Window messagesWindow;
extern struct Window registersWindow;

extern LinkedList *messagesWindowText;

void addMessageToMessagesWindow(char *fmt, ...);

extern char **registersText; /* allocated */

void initWindows(int instructionCount, char **instructionsText);
void drawFrontend();

#endif // BOOTSTRAP_FRONTEND_H