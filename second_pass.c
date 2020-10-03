/**
    This file contains the main part of the algorithm.
    After first pass finishes without errors, each line is parsed again for final data and instruction encoding
**/

#include "definitions.h"

/* This function reads the file line by line */
void secondFileRead(char* filename, Data* data)
{
    FILE* fp = fopen(filename,"r");
    char buffer[MAX_LINE_LENGTH];
    int i;
    while(fgets(buffer,MAX_LINE_LENGTH,fp) != NULL)
    {
        i = 0;
        data->line_counter++;
        char* wordArray[MAX_WORDS_PER_LINE];
        char* ptr = strtok(buffer," \t\n");
        while(ptr != NULL)
        {
            wordArray[i] = (char*)malloc(strlen(ptr)+1);
            strcpy(wordArray[i],ptr);
            ptr = strtok(NULL," \t\n");
            i += 1;
        }
        if(i == 0 || !strcmp(wordArray[0],";")) /*The line is empty or a comment - do nothing*/
        {
            continue;
        }
        else
        {
            buildInstruction(wordArray,i,data);
        }
        freeWordsMemory(wordArray,i);
    }
}

/* This function encodes each line by it's type either command or data also it calls error checking functions */
void buildInstruction(char* words[], unsigned int numOfWords, Data* data)
{
    int i = 0;
    int tmpCounter;
    if(isLabel(words[i])) i++;
    if(isCommand(words[i]))
    {
        tmpCounter = 1;
        data->instructionArr = (Instruction*)realloc(data->instructionArr,sizeof(Instruction)*(data->instruction_counter+1));
        data->instructionArr[data->instruction_counter].myType = INSTRUCTION_WORD;
        commandType currentCommand = returnCommandType(words[i]);
        data->instructionArr[data->instruction_counter].Ins.opcode = getOPCODE(currentCommand);
        data->instructionArr[data->instruction_counter].Ins.funct = getFunct(words[i]);
        if(currentCommand >= mov && currentCommand <= lea) /* 2 Operands*/
        {
            checkTwoOperands(words,&i,&numOfWords,data, currentCommand);
            createTwoOpInstruction(words, data, i,tmpCounter);
        }
        else if(currentCommand >= clr && currentCommand <= prn) /* 1 operand */
        {
            checkOneOperand(words,i+1,data,currentCommand);
            createOneOpInstruction(words, data, i, tmpCounter);
        }
        else if(currentCommand >= rts && currentCommand <= stop) /* No operands */
        {
            createZeroOpInstruction(words, data);
        }
    }
    else if(isDirective(words[i]))
    {
        directiveType currentDirective = returnDirectiveType(words[i]);
        switch(currentDirective)
        {
        case DATA: /* data directive need to push data into the data array */
            pushNumbers(words,i+1,numOfWords,data);
            break;
        case STRING: /* string directive need to push characters to data array */
            pushString(words, i+1, data);
            break;
        case ENTRY:
            data->entryArr = (Label*)realloc(data->entryArr,sizeof(Label)*(data->entry_counter+1));
            strcpy(data->entryArr[data->entry_counter].name,words[i+1]);
            data->entryArr[data->entry_counter].address = getLabelAddress(words[i+1],data);
            data->entryArr[data->entry_counter].type = DIRECTIVE;
            data->entryArr[data->entry_counter].isExternal = FALSE;
            data->entry_counter++;
            break;
        }
    }
}

/* This function creates instructions with 2 operands */
void createTwoOpInstruction(char* words[], Data* data, int wordIndex, int tmpCounter)
{
    data->instructionArr[data->instruction_counter].Ins.source_addressing = getAddressingForSourceOperand(words[wordIndex+1],data);
    switch(data->instructionArr[data->instruction_counter].Ins.source_addressing)
    {
        case IMMEDIATE_ADDRESS: /* A number requires to create a word of type number at next position*/
            data->instructionArr[data->instruction_counter].Ins.source_register = 0;
            data->instructionArr = (Instruction*)realloc(data->instructionArr,sizeof(Instruction)*(data->instruction_counter+2));
            createExtraDataWord(words[wordIndex+1],data,data->instruction_counter+tmpCounter);
            tmpCounter++;
            break;
        case DIRECT_ADDRESS: /* A label, requires to create  a word of type extra at next position*/
            data->instructionArr[data->instruction_counter].Ins.source_register = 0;
            data->instructionArr = (Instruction*)realloc(data->instructionArr,sizeof(Instruction)*(data->instruction_counter+2));
            createExtraWord(words[wordIndex+1],data,data->instruction_counter+tmpCounter);
            if(getLabel(words[wordIndex+1],data)->isExternal) addToExternArr(words[wordIndex+1],1,data);
            tmpCounter++;
            break;
        case REGISTER_ADDRESS: /* A register, no need for another word, just write its number */
            data->instructionArr[data->instruction_counter].Ins.source_register = getRegister(words[wordIndex+1]);
            break;
        default:
            printf("Error: Illegal addressing for source operand at line: %d\n", data->line_counter);
            data->number_of_errors++;
            break;
    }
    data->instructionArr[data->instruction_counter].Ins.destination_addressing = getAddressingForDestinationOperand(words[wordIndex+2],data);
    switch(data->instructionArr[data->instruction_counter].Ins.destination_addressing)
    {
        case IMMEDIATE_ADDRESS: /* A number requires to create a word of type extra-data at next position*/
            data->instructionArr[data->instruction_counter].Ins.destination_register = 0;
            data->instructionArr = (Instruction*)realloc(data->instructionArr,sizeof(Instruction)*(data->instruction_counter+3));
            createExtraDataWord(words[wordIndex+2],data,data->instruction_counter+tmpCounter);
            tmpCounter++;
            break;
        case DIRECT_ADDRESS: /* A label, requires to create  a word of type extra at next position*/
            data->instructionArr[data->instruction_counter].Ins.destination_register = 0;
            data->instructionArr = (Instruction*)realloc(data->instructionArr,sizeof(Instruction)*(data->instruction_counter+3));
            createExtraWord(words[wordIndex+2],data,data->instruction_counter+tmpCounter);
            if(getLabel(words[wordIndex+2],data)->isExternal)
            {
                if(data->instructionArr[data->instruction_counter].Ins.source_addressing != REGISTER_ADDRESS)
                    addToExternArr(words[wordIndex+2],2,data);
                else
                    addToExternArr(words[wordIndex+2],1,data);
            }
            tmpCounter++;
            break;
        case REGISTER_ADDRESS: /* A register, no need for another word, just write its number */
            data->instructionArr[data->instruction_counter].Ins.destination_register = getRegister(words[wordIndex+2]);
            break;
        default:
            printf("Error: Illegal addressing for destination operand at line: %d\n", data->line_counter);
            data->number_of_errors++;
            break;
    }
    /* ARE Check*/
    if(data->instructionArr[data->instruction_counter].Ins.source_addressing == IMMEDIATE_ADDRESS
        || data->instructionArr[data->instruction_counter].Ins.destination_addressing == IMMEDIATE_ADDRESS)
    {
        Label* tmpLabel1 = getLabel(words[wordIndex+1],data);
        Label* tmpLabel2 = getLabel(words[wordIndex+2],data);
        if((tmpLabel1!=NULL && tmpLabel1->isExternal)
             || (tmpLabel2!=NULL && tmpLabel2->isExternal))
        {
            data->instructionArr[data->instruction_counter].Ins.A = 0;
            data->instructionArr[data->instruction_counter].Ins.R = 0;
            data->instructionArr[data->instruction_counter].Ins.E = 1;
        }
        else
        {
            data->instructionArr[data->instruction_counter].Ins.A = 0;
            data->instructionArr[data->instruction_counter].Ins.R = 1;
            data->instructionArr[data->instruction_counter].Ins.E = 0;
        }
    }
    else
    {
        data->instructionArr[data->instruction_counter].Ins.A = 1;
        data->instructionArr[data->instruction_counter].Ins.R = 0;
        data->instructionArr[data->instruction_counter].Ins.E = 0;
    }
    data->instruction_counter += tmpCounter;
    data->IC += tmpCounter;
}

/* This function creates instructions with a single operand */
void createOneOpInstruction(char* words[], Data* data, int wordIndex, int tmpCounter)
{
    data->instructionArr[data->instruction_counter].Ins.source_addressing = 0;
    data->instructionArr[data->instruction_counter].Ins.source_register = 0;
    data->instructionArr[data->instruction_counter].Ins.destination_addressing = getAddressingForDestinationOperand(words[wordIndex+1],data);
    switch(data->instructionArr[data->instruction_counter].Ins.destination_addressing)
    {
        case IMMEDIATE_ADDRESS: /* A number requires to create a word of type data at next position*/
            data->instructionArr[data->instruction_counter].Ins.destination_register = 0;
            data->instructionArr = (Instruction*)realloc(data->instructionArr,sizeof(Instruction)*(data->instruction_counter+2));
            createExtraDataWord(words[wordIndex+1],data,data->instruction_counter+tmpCounter);
            tmpCounter++;
            break;
        case DIRECT_ADDRESS: /* A label, requires to create  a word of type extra at next position*/
            data->instructionArr[data->instruction_counter].Ins.destination_register = 0;
            data->instructionArr = (Instruction*)realloc(data->instructionArr,sizeof(Instruction)*(data->instruction_counter+2));
            createExtraWord(words[wordIndex+1],data,data->instruction_counter+tmpCounter);
            if(getLabel(words[wordIndex+1],data)->isExternal) addToExternArr(words[wordIndex+1],1,data);
            tmpCounter++;
            break;
        case RELATIVE_ADDRESS: /* A label address, requires to create a word of type extra (fur jump) at next position*/
            data->instructionArr[data->instruction_counter].Ins.destination_register = 0;
            data->instructionArr = (Instruction*)realloc(data->instructionArr,sizeof(Instruction)*(data->instruction_counter+2));
            createJumpExtraWord(words[wordIndex+1],data,data->instruction_counter+tmpCounter);
            tmpCounter++;
            break;
        case REGISTER_ADDRESS: /* A register, no need for another word, just write its number */
            data->instructionArr[data->instruction_counter].Ins.destination_register = getRegister(words[wordIndex+1]);
            break;
        default:
            printf("Error: Illegal addressing for destination operand at line: %d\n", data->line_counter);
            data->number_of_errors++;
            break;
    }
    /* ARE Check*/
    if(data->instructionArr[data->instruction_counter].Ins.destination_addressing == DIRECT_ADDRESS ||
       data->instructionArr[data->instruction_counter].Ins.destination_addressing == RELATIVE_ADDRESS)
    {
        Label* tmpLabel = getLabel(words[wordIndex+1],data);
        if(tmpLabel != NULL && (tmpLabel->isExternal==TRUE
                                 || data->instructionArr[data->instruction_counter].Ins.destination_addressing == RELATIVE_ADDRESS))
        {
            data->instructionArr[data->instruction_counter].Ins.A = 1;
            data->instructionArr[data->instruction_counter].Ins.R = 0;
            data->instructionArr[data->instruction_counter].Ins.E = 0;
            data->instruction_counter += tmpCounter;
            data->IC += tmpCounter;
            return;
        }
    }
    data->instructionArr[data->instruction_counter].Ins.A = 1;
    data->instructionArr[data->instruction_counter].Ins.R = 0;
    data->instructionArr[data->instruction_counter].Ins.E = 0;
    data->instruction_counter += tmpCounter;
    data->IC += tmpCounter;
}

/* This function creates instructions with no operands */
void createZeroOpInstruction(char* words[], Data* data)
{
    data->instructionArr[data->instruction_counter].Ins.source_addressing = 0;
    data->instructionArr[data->instruction_counter].Ins.source_register = 0;
    data->instructionArr[data->instruction_counter].Ins.destination_addressing = 0;
    data->instructionArr[data->instruction_counter].Ins.destination_register = 0;
    data->instructionArr[data->instruction_counter].Ins.A = 1;
    data->instructionArr[data->instruction_counter].Ins.R = 0;
    data->instructionArr[data->instruction_counter].Ins.E = 0;
    data->instruction_counter++;
    data->IC++;
}

/* This function creates a word if the operand is a label */
void createExtraWord(char* operand, Data* data, unsigned int index)
{
    data->instructionArr[index].myType = EXTRA_WORD;
    data->instructionArr[index].Extra.address = getLabelAddress(operand,data);
    if(getLabel(operand,data)->isExternal == TRUE)
    {
        data->instructionArr[index].Extra.A = 0;
        data->instructionArr[index].Extra.R = 0;
        data->instructionArr[index].Extra.E = 1;
    }
    else
    {
        data->instructionArr[index].Extra.A = 0;
        data->instructionArr[index].Extra.R = 1;
        data->instructionArr[index].Extra.E = 0;
    }
}
/* This function creates a data word that represents a number */
void createExtraDataWord(char* operand, Data* data, unsigned int index)
{
    data->instructionArr[index].myType = EXTRA_WORD;
    data->instructionArr[index].Extra.A = 1;
    data->instructionArr[index].Extra.R = 0;
    data->instructionArr[index].Extra.E = 0;
    data->instructionArr[index].Extra.address = atoi(++operand);
}

/* This function creates a word if the operand is a label address */
void createJumpExtraWord(char* operand, Data* data, unsigned int index)
{
   data->instructionArr[index].myType = EXTRA_WORD;
   data->instructionArr[index].Extra.address = getLabelAddress(++operand,data) - data->IC;
    if(getLabel(operand,data)->isExternal == TRUE)
    {
        data->instructionArr[index].Extra.A = 0;
        data->instructionArr[index].Extra.R = 0;
        data->instructionArr[index].Extra.E = 1;
    }
    else
    {
        data->instructionArr[index].Extra.A = 0;
        data->instructionArr[index].Extra.R = 1;
        data->instructionArr[index].Extra.E = 0;
    }
}

/* This function pushes numbers after .data command is called */
void pushNumbers(char* words[],unsigned int firstIndex, unsigned int numOfWords, Data* data)
{
    int i;
    char* ptr;
    for(i=firstIndex;i<numOfWords;i++)
    {
        ptr = strtok(words[i],",");
        while(ptr != NULL)
        {
            if(is_valid_int(ptr))
            {
                data->dataArr = (Instruction*)realloc(data->dataArr, sizeof(Instruction)*(data->DC + 1));
                data->dataArr[data->DC].myType = DATA_WORD;
                data->dataArr[data->DC].Value.value = atoi(ptr);
                data->DC++;
                ptr = strtok(NULL,",");
            }
            else
            {
                printf("Error: Illegal input for directive .data at line: %d\n", data->line_counter);
                data->number_of_errors++;
                return;
            }
        }
    }
}

/* This function pushes characters after .string command is called */
void pushString(char* words[], unsigned int firstIndex, Data* data)
{
    int n = strlen(words[firstIndex]);
    if(words[firstIndex][0] != '"' || words[firstIndex][n-1] != '"')
    {
        printf("Error: Illegal input for directive .string at line: %d\n", data->line_counter);
        data->number_of_errors++;
        return;
    }
    int i;
    for(i=1;i<n-1;i++)
    {
        data->dataArr = (Instruction*)realloc(data->dataArr, sizeof(Instruction)*(data->DC + 1));
        data->dataArr[data->DC].myType = DATA_WORD;
        data->dataArr[data->DC].Value.value = words[firstIndex][i];
        data->DC++;
    }
    data->dataArr = (Instruction*)realloc(data->dataArr, sizeof(Instruction)*(data->DC + 1));
    data->dataArr[data->DC].myType = DATA_WORD;
    data->dataArr[data->DC].Value.value = '\0';
    data->DC++;
}

/* This function adds extern labels when used by instructions to the extern array */
void addToExternArr(char* name, int index, Data* data)
{
    data->externArr = (Label*)realloc(data->externArr,sizeof(Label)*(data->extern_counter+1));
    strcpy(data->externArr[data->extern_counter].name,name);
    data->externArr[data->extern_counter].address = data->IC + index;
    data->extern_counter++;
}


















