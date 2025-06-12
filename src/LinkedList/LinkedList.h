#pragma once

typedef void *Value;
typedef int (*ComparisonFunction)(const void *key1, const void *key2, void *additionalArgs);
typedef void (*FreeValueFunction)(Value value, void *AdditionalArgs);

typedef struct LinkedNode
{
    Value value;
    struct LinkedNode *next;
} LinkedNode;

typedef struct LinkedList
{
    unsigned int length;
    LinkedNode *first;
    LinkedNode *last;

    ComparisonFunction cmpFunc;
    void *cmpFuncAdditionalArgs;
    FreeValueFunction freeValueFunc;
    void *freeValueFuncAdditionalArgs;

} LinkedList;

/*
    takes as a parameter a function to compare objects and a function to free an object,
    and any additional arguments that are passed as a void*
*/
LinkedList *datatypes_linkedList_create(ComparisonFunction cmpFunc, FreeValueFunction freeValueFunc, void *cmpFuncAdditionalArgs,
                                        void *freeValueFuncAdditionalArgs);
void datatypes_linkedList_destroy(LinkedList *ls);

void datatypes_linkedList_append(LinkedList *ls, Value val);
void datatypes_linkedList_add(LinkedList *ls, Value val, int pos);
void datatypes_linkedList_delete(LinkedList *ls, int pos);
int datatypes_linkedList_remove(LinkedList *ls, Value val);
int datatypes_linkedList_add_if_doesnt_exists(LinkedList *ls, Value val);

int datatypes_linkedList_exists(LinkedList *ls, Value val);

int datatypes_linkedList_length(LinkedList *ls);
const LinkedNode *datatypes_linkedList_getNode(LinkedList *ls, int pos);
const LinkedNode *datatypes_linkedList_find(LinkedList *ls, Value val);
