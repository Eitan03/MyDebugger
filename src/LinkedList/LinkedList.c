#include "LinkedList.h"

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

LinkedList *datatypes_linkedList_create(ComparisonFunction cmpFunc, FreeValueFunction freeValueFunc, void *cmpFuncAdditionalArgs,
                                        void *freeValueFuncAdditionalArgs)
{
    LinkedList *ls = (LinkedList *)malloc(sizeof(LinkedList));
    ls->length = 0;
    ls->first = NULL;
    ls->last = NULL;
    ls->cmpFunc = cmpFunc;
    ls->cmpFuncAdditionalArgs = cmpFuncAdditionalArgs;
    ls->freeValueFunc = freeValueFunc;
    ls->freeValueFuncAdditionalArgs = freeValueFuncAdditionalArgs;
    return ls;
}

static void destroyLinkedNode(LinkedNode *node, FreeValueFunction freeValueFunc, void *FreeValueFuncAdditionalArgs)
{
    freeValueFunc(node->value, FreeValueFuncAdditionalArgs);
    if (node->next != NULL)
    {
        destroyLinkedNode(node->next, freeValueFunc, FreeValueFuncAdditionalArgs);
    }
    free(node);
}

void datatypes_linkedList_destroy(LinkedList *ls)
{
    if (ls->length > 0)
    {
        destroyLinkedNode(ls->first, ls->freeValueFunc, ls->freeValueFuncAdditionalArgs);
    }
    free(ls);
}

void datatypes_linkedList_append(LinkedList *ls, Value val)
{
    if (ls->length > 0)
    {
        ls->last->next = (LinkedNode *)malloc(sizeof(LinkedNode));
        ls->last = ls->last->next;
        ls->last->value = val;
        ls->last->next = NULL;
    }
    else
    {

        ls->first = (LinkedNode *)malloc(sizeof(LinkedNode));
        ls->last = ls->first;
        ls->first->value = val;
        ls->first->next = NULL;
    }

    ls->length++;
}

static LinkedNode *getNodeAtPos(LinkedList *ls, int pos)
{
    LinkedNode *curNode = ls->first;
    int i;
    for (i = 0; i < pos; i++)
    {
        curNode = curNode->next;
    }
    return curNode;
}

void datatypes_linkedList_add(LinkedList *ls, Value val, int pos)
{
    if (pos > ls->length)
    {
        printf("error! pos was bigger then the linkedList length\n");
        exit(EXIT_FAILURE);
    }

    if (pos > 0)
    {
        LinkedNode *curNode = getNodeAtPos(ls, pos - 1);

        LinkedNode *newNode = (LinkedNode *)malloc(sizeof(LinkedNode));

        newNode->next = curNode->next;
        newNode->value = val;

        curNode->next = newNode;
    }
    else
    {

        LinkedNode *newNode = (LinkedNode *)malloc(sizeof(LinkedNode));

        newNode->next = ls->first;
        newNode->value = val;

        ls->first = newNode;
    }

    ls->length++;
}

void datatypes_linkedList_delete(LinkedList *ls, int pos)
{
    if (pos > ls->length)
    {
        printf("error! pos was bigger then the Linkedlist length\n");
        exit(EXIT_FAILURE);
    }

    if (pos == 0)
    {
        LinkedNode *nodeToDelete = ls->first;
        ls->first = nodeToDelete->next;

        nodeToDelete->next = NULL;
        destroyLinkedNode(nodeToDelete, ls->freeValueFunc, ls->freeValueFuncAdditionalArgs);
    }
    else
    {
        LinkedNode *prevNode = getNodeAtPos(ls, pos - 1);
        LinkedNode *nodeToDelete = prevNode->next;
        prevNode->next = nodeToDelete->next;

        nodeToDelete->next = NULL;
        destroyLinkedNode(nodeToDelete, ls->freeValueFunc, ls->freeValueFuncAdditionalArgs);
    }

    ls->length--;
}

int datatypes_linkedList_remove(LinkedList *ls, Value val)
{
    LinkedNode *lastNode = NULL;

    LinkedNode *curNode;
    for (curNode = ls->first; curNode != NULL; curNode = curNode->next)
    {
        if (ls->cmpFunc(curNode->value, val, ls->cmpFuncAdditionalArgs))
        {
            if (lastNode == NULL)
            {
                ls->first = curNode->next;
                curNode->next = NULL;
                destroyLinkedNode(curNode, ls->freeValueFunc, ls->freeValueFuncAdditionalArgs);
            }
            else
            {
                lastNode->next = curNode->next;
                curNode->next = NULL;
                destroyLinkedNode(curNode, ls->freeValueFunc, ls->freeValueFuncAdditionalArgs);
            }
            ls->length--;
            return 1;
        }
        lastNode = curNode;
    }
    return 0;
}

int datatypes_linkedList_add_if_doesnt_exists(LinkedList *ls, Value val)
{
    LinkedNode *newNode;

    LinkedNode *curNode;
    for (curNode = ls->first; curNode->next != NULL; curNode = curNode->next)
    {
        if (ls->cmpFunc(curNode->value, val, ls->cmpFuncAdditionalArgs))
        {
            return 0;
        }
    }
    if (curNode->next != NULL)
    {
        curNode = curNode->next;
    }

    if (ls->cmpFunc(curNode->value, val, ls->cmpFuncAdditionalArgs))
    {
        return 0;
    }

    newNode = (LinkedNode *)malloc(sizeof(LinkedNode));

    newNode->next = NULL;
    newNode->value = val;
    curNode->next = newNode;
    ls->length++;

    return 1;
}

int datatypes_linkedList_exists(LinkedList *ls, Value val)
{
    LinkedNode *curNode;
    for (curNode = ls->first; curNode != NULL; curNode = curNode->next)
    {
        if (ls->cmpFunc(curNode->value, val, ls->cmpFuncAdditionalArgs))
        {
            return 1;
        }
    }
    return 0;
}

int datatypes_linkedList_length(LinkedList *ls)
{
    return ls->length;
}

const LinkedNode *datatypes_linkedList_getNode(LinkedList *ls, int pos)
{
    if (pos > ls->length)
    {
        printf("error! pos was bigger then the linkedlist length\n");
        exit(EXIT_FAILURE);
    }

    return getNodeAtPos(ls, pos);
}

const LinkedNode *datatypes_linkedList_find(LinkedList *ls, Value val)
{
    LinkedNode *curNode = ls->first;
    int i;
    for (i = 0; i < ls->length; i++)
    {
        if (ls->cmpFunc(curNode->value, val, ls->cmpFuncAdditionalArgs))
        {
            return curNode;
        }
        curNode = curNode->next;
    }
    return NULL;
}