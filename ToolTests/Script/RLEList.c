#include <stdlib.h>

#include "/home/mtm/public/2122b/ex1/RLEList.h"

struct RLEList_t
{
    char value;
    int times;
    struct RLEList_t *next;
};

static int getPowerOfTen(int power)
{
    if (power == 0)
    {
        return 1;
    }

    int number = 10;

    for (int i = 1; i < power; i++)
    {
        number *= 10;
    }

    return number;
}
static int getSizeOfIntToString(int number)
{
    int size = 1;
    while (number > 9)
    {
        number /= 10;
        size++;
    }
    return size;
}

static int getSizeOfStringFromList(RLEList list)
{
    int size = 1; // '\0'
    RLEList currentNode = list;
    while (currentNode)
    {
        size++; // the char
        size += getSizeOfIntToString(currentNode->times);
        size++; // '\n'
        currentNode = currentNode->next;
    }
    return size;
}

static int getNumberOfNodes(RLEList list)
{
    int numberOfNodes = 0;
    RLEList currentNode = list;

    while (currentNode)
    {
        numberOfNodes++;
        currentNode = currentNode->next;
    }

    return numberOfNodes;
}

static int isIndexInBounds(RLEList list, int index)
{
    if (list->times == -1 || index < 0 || index >= RLEListSize(list))
    {
        return 0;
    }

    return 1;
}

static RLEList getNodeByIndex(RLEList list, int index)
{

    if (!isIndexInBounds(list, index))
    {
        return NULL;
    }

    RLEList currentNode = list;
    while (currentNode)
    {
        index -= currentNode->times;
        if (index < 0)
        {
            return currentNode;
        }
        currentNode = currentNode->next;
    }

    return currentNode;
}

RLEList RLEListCreate()
{
    RLEList newRleList = malloc(sizeof(*newRleList));
    if (!newRleList)
    {
        return NULL;
    }

    newRleList->times = -1;
    newRleList->value = '\0';
    newRleList->next = NULL;

    return newRleList;
}

void RLEListDestroy(RLEList list)
{
    while (list)
    {
        RLEList nodeToDelete = list;
        list = list->next;
        free(nodeToDelete);
    }
}

RLEListResult RLEListAppend(RLEList list, char value)
{
    if (!list || !value)
    {
        return RLE_LIST_NULL_ARGUMENT;
    }

    // if is the first node to append.
    if (list->times == -1)
    {
        list->times = 1;
        list->value = value;
        list->next = NULL;
        return RLE_LIST_SUCCESS;
    }

    RLEList lastNode = list;
    while (lastNode->next)
    {
        lastNode = lastNode->next;
    }

    if (lastNode->value == value)
    {
        lastNode->times++;
    }
    else
    {
        RLEList newNode = RLEListCreate();
        if (!newNode)
        {
            return RLE_LIST_OUT_OF_MEMORY;
        }
        lastNode->next = newNode;
        newNode->value = value;
        newNode->times = 1;
        newNode->next = NULL;
    }
    return RLE_LIST_SUCCESS;
}

int RLEListSize(RLEList list)
{
    if (!list)
    {
        return -1;
    }

    int numberOfNodes = 0;

    if (list->times == -1)
    {
        return numberOfNodes;
    }

    RLEList currentNode = list;
    while (currentNode)
    {
        numberOfNodes += currentNode->times;
        currentNode = currentNode->next;
    }

    return numberOfNodes;
}

RLEListResult RLEListRemove(RLEList list, int index)
{
    if (!list)
    {
        return RLE_LIST_NULL_ARGUMENT;
    }

    // TODO: BUG isIndexInBounds deletes the first node of list.
    if (!isIndexInBounds(list, index))
    {
        return RLE_LIST_INDEX_OUT_OF_BOUNDS;
    }

    if (index == 0 || index < list->times)
    {
        if (list->times > 1)
        {
            list->times--;
        }

        else
        {
            if (list->next == NULL)
            {
                list->times = -1;
                list->value = '\0';
                list->next = NULL;
            }
            if (list->next != NULL)
            {
                RLEList tempNode = list->next;
                *list = *(list->next);
                free(tempNode);
            }
        }

        return RLE_LIST_SUCCESS;
    }

    RLEList nodeBeforeDeletedNode = getNodeByIndex(list, index - 1);
    RLEList nodeToDelete = getNodeByIndex(list, index);
    RLEList nodeAfterDeletedNode = getNodeByIndex(list, index + 1);

    if (nodeToDelete->times > 1)
    {
        nodeToDelete->times--;
    }
    else
    {
        if (nodeAfterDeletedNode && nodeBeforeDeletedNode->value == nodeAfterDeletedNode->value)
        {
            nodeBeforeDeletedNode->times += nodeAfterDeletedNode->times;
            nodeBeforeDeletedNode->next = nodeAfterDeletedNode->next;
            free(nodeAfterDeletedNode);
            free(nodeToDelete);
            return RLE_LIST_SUCCESS;
        }
        else
        {
            nodeBeforeDeletedNode->next = nodeAfterDeletedNode;
        }

        free(nodeToDelete);
    }

    return RLE_LIST_SUCCESS;
}

char RLEListGet(RLEList list, int index, RLEListResult *result)
{
    if (!list)
    {
        *result = RLE_LIST_NULL_ARGUMENT;
        return 0;
    }

    if (!isIndexInBounds(list, index))
    {
        *result = RLE_LIST_INDEX_OUT_OF_BOUNDS;
        return 0;
    }

    if (result)
    {
        *result = RLE_LIST_SUCCESS;
    }

    return getNodeByIndex(list, index)->value;
}

char *RLEListExportToString(RLEList list, RLEListResult *result)
{
    if (!list)
    {
        *result = RLE_LIST_NULL_ARGUMENT;
        return NULL;
    }

    int numberOfNodes = getNumberOfNodes(list);
    int sizeOfString = getSizeOfStringFromList(list);

    char *stringToExport = malloc(sizeOfString);

    if (!stringToExport)
    {
        *result = RLE_LIST_OUT_OF_MEMORY;
        return NULL;
    }

    RLEList currentNode = list;
    int currentStringIdnex = 0;
    for (int i = 0; i < numberOfNodes; i++)
    {
        stringToExport[currentStringIdnex++] = currentNode->value;

        int currentNumber = currentNode->times;
        for (int j = 0; j < getSizeOfIntToString(currentNode->times); j++)
        {
            int digitsInCurrentNumber = getSizeOfIntToString(currentNumber);
            int currentDigit = currentNumber / getPowerOfTen(digitsInCurrentNumber - 1);
            stringToExport[currentStringIdnex++] = currentDigit + '0';
            currentNumber -= currentDigit * getPowerOfTen(digitsInCurrentNumber - 1);
            for (int m = 1; m < digitsInCurrentNumber - getSizeOfIntToString(currentNumber); m++)
            {
                stringToExport[currentStringIdnex++] = '0';
                j++;
            }
        }
        stringToExport[currentStringIdnex++] = '\n';
        currentNode = currentNode->next;
    }
    stringToExport[currentStringIdnex] = '\0';

    if (result)
    {
        *result = RLE_LIST_SUCCESS;
    }
    return stringToExport;
}

RLEListResult RLEListMap(RLEList list, MapFunction map_function)
{
    if (!list || !map_function)
    {
        return RLE_LIST_NULL_ARGUMENT;
    }

    RLEList currentNode = list;

    // change all chars to mapped value.
    for (int i = 0; i < getNumberOfNodes(list); i++)
    {
        currentNode->value = map_function(currentNode->value);
        currentNode = currentNode->next;
    }

    // remove duplicate values in a row.
    currentNode = list;
    int currentNodeIndex = 0;
    for (int i = 0; i < getNumberOfNodes(list); i++)
    {
        if (isIndexInBounds(list, i) && currentNode->next)
        {
            if (currentNode->value == currentNode->next->value)
            {
                int indexesToDelete = currentNode->next->times;
                currentNode->times += currentNode->next->times;

                for (int j = 0; j < indexesToDelete; j++)
                {
                    RLEListRemove(list, currentNodeIndex + currentNode->times);
                }
                i--;
            }
            else
            {
                currentNodeIndex += currentNode->times;
                currentNode = currentNode->next;
            }
        }
    }

    return RLE_LIST_SUCCESS;
}

int main9()
{
    return 0;
    // RLEListResult *result;
    // RLEList list = RLEListCreate();
    // RLEListAppend(list, 'a');
    // RLEListAppend(list, 'a');
    // RLEListAppend(list, 'b');
    // RLEListAppend(list, 'c');
    // RLEListAppend(list, 'c');
    // char* string = RLEListExportToString(list, result);
    // printf("%s", string);
    // printf("\nEND\n");
    // RLEListDestroy(list);
    // return 0;
}
// int main(){
//     printf("\n%d\n",getSizeOfIntToString(2023123));
//     return 0;
// }
// int main()
// {
//     RLEListResult *result = NULL;
//     RLEList list_remote = RLEListCreate();
//     for(int i; i<1300; i++){
//     RLEListAppend(list_remote, 'a');

//     }

//     // RLEListAppend(list, 'a');
//     RLEListAppend(list_remote, 'b');
//     RLEListAppend(list_remote, 'c');
//     RLEListRemove(list_remote, 0);
//     RLEListAppend(list_remote, 'c');
//     RLEListAppend(list_remote, 'c');
//     char* string = RLEListExportToString(list_remote, result);
//     printf("%s", string);
//     printf("\nEND\n");
//     RLEListDestroy(list_remote);
//     free(result);
//     free(string);
//     return 0;
// }