/**
    This file includes functions for the first pass on .as file
    And creates the basic data structures required for the second pass
**/

#include "definitions.h"

/* This function goes through the file line by line and breaks it into words ignoring all types of delimiters */
void firstFileRead(char* filename, Data* data)
{
    FILE* fp = fopen(filename,"r");
    char buffer[MAX_LINE_LENGTH];
    int i;
    while(fgets(buffer,MAX_LINE_LENGTH,fp) != NULL)
    {
        data->line_counter++;
        i = 0;
        char* wordArray[MAX_WORDS_PER_LINE];
        char* ptr = strtok(buffer," \r\t\n,");
        while(ptr != NULL)
        {
            wordArray[i] = (char*)malloc(strlen(ptr)+1);
            strcpy(wordArray[i],ptr);
            ptr = strtok(NULL," \r\t\n,");
            i += 1;
        }
        if(i == 0 || !strcmp(wordArray[0],";")) /*The line is empty or a comment - do nothing*/
        {
            continue;
        }
        else
        {
            checkWords(wordArray,i,data);
        }
        freeWordsMemory(wordArray,i);
    }
    updateDataAddresses(data);
    if(data->line_counter > MAX_NO_LINES)
    {
        printf("Error - file exceeds max line count\n");
        data->number_of_errors++;
    }
    if(data->DC > MAX_MEMORY_ADDRESS)
    {
        printf("Error - memory leak, file takes too much space\n");
        data->number_of_errors++;
    }
}

/* This function clears the word array memory allocated by previous function */
void freeWordsMemory(char* words[], int n)
{
    int i;
    for(i=0; i<n; i++)
        free(words[i]);
}

/* This function checks how much memory is required by each operation (1-3 cells) */
void checkWords(char* words[], int n, Data* data)
{
    if(isLabel(words[0]))
    {
        addToLabelTable(words, n, data);
    }
    else if(isExternal(words[0]))
    {
        data->labelArr = (Label*)realloc(data->labelArr,sizeof(Label)*(data->label_counter+1));
        strcpy(data->labelArr[data->label_counter].name,words[1]);
        data->labelArr[data->label_counter].address = 0;
        data->labelArr[data->label_counter].isExternal = TRUE;
        data->labelArr[data->label_counter].type = DIRECTIVE;
        data->label_counter++;
    }
    else if(isCommand(words[0]))
    {
        commandType tmp = returnCommandType(words[0]);
        if(tmp >= mov && tmp <= lea) data->IC += checkICForTwoOperands(words[1],words[2]);
        else if(tmp >= clr && tmp <= prn) data->IC += checkICForOneOperand(words[1]);
        else if(tmp >= rts && tmp <= stop) data->IC++;
    }

    else if(isDirective(words[0]))
    {
        directiveType tmp = returnDirectiveType(words[0]);
        switch(tmp)
        {
            case DATA:
                data->DC += checkDCForData(words,0,n);
                break;
            case STRING:
                data->DC += checkDCForString(words[1]);
                break;
        }
    }
    else
    {
        printf("Error - illegal line at line: %d\n", data->line_counter);
        data->number_of_errors++;
        return;
    }
}

/* This function constructs the symbol table required for the second pass */
void addToLabelTable(char* words[], int numOfWords, Data* data)
{
    data->labelArr = (Label*)realloc(data->labelArr,sizeof(Label)*(data->label_counter+1));
    words[0][strlen(words[0])-1] = 0;
    isLabelOk(words[0],data);
    strcpy(data->labelArr[data->label_counter].name,words[0]);
    data->labelArr[data->label_counter].isExternal = FALSE;
    if(isCommand(words[1]))
    {
        data->labelArr[data->label_counter].type = COMMAND;
        data->labelArr[data->label_counter].address = data->IC;
        commandType tmp = returnCommandType(words[1]);
        if(tmp >= mov && tmp <= lea) data->IC += checkICForTwoOperands(words[2],words[3]);
        else if(tmp >= clr && tmp <= prn) data->IC += checkICForOneOperand(words[2]);
        else if(tmp >= rts && tmp <= stop) data->IC++;
    }
    else if(isDirective(words[1]))
    {
        data->labelArr[data->label_counter].type = DIRECTIVE;
        data->labelArr[data->label_counter].address = data->DC;
        directiveType tmp = returnDirectiveType(words[1]);
        switch(tmp)
        {
            case DATA:
                data->DC += checkDCForData(words,2,numOfWords);
                break;
            case STRING:
                data->DC += checkDCForString(words[2]);
                break;
        }
    }
    data->label_counter++;
}

/* This function updates all addresses for each label */
void updateDataAddresses(Data* data)
{
    int i;
    for(i=0;i<data->label_counter;i++)
    {
        if(data->labelArr[i].type == DIRECTIVE && data->labelArr[i].isExternal == FALSE)
        {
            data->labelArr[i].address += data->IC;
        }
    }
}
















