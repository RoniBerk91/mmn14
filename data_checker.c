/**
    This File includes various error and string checking functions for first and second passes
**/

#include "definitions.h"

/* Local saved words for checks */
char* register_names[] = {"r0","r1","r2","r3","r4","r5","r6","r7"};
char* command_names[] = {"mov","cmp","add","sub","lea","clr","not","inc","dec","jmp","bne","jsr","red","prn","rts","stop"};
char* directive_names[] = {".data",".string",".entry",".extern"};
char* all_saved_words[] = {"r0","r1","r2","r3","r4","r5","r6","r7",
                "mov","cmp","add","sub","lea","clr","not","inc","dec","jmp","bne","jsr","red","prn","rts","stop",
                ".data",".string",".entrty",".extern"};

boolean isLabel(char* word)
{
    int n = strlen(word);
    if(word[n-1] == ':')
    {
        return TRUE;
    }
    return FALSE;
}

void isLabelOk(char* word, Data* data)
{
    int i, n = strlen(word);
    if(n > MAX_LABEL_LENGTH)
    {
        printf("Error: Illegal label %s at line %d - exceeds max length limit\n", word, data->line_counter);
        data->number_of_errors++;
        return FALSE;
    }
    else if(!isalpha(word[0]))
    {
        printf("Error: Illegal label %s at line %d - illegal first character\n", word, data->line_counter);
        data->number_of_errors++;
        return FALSE;
    }
    for(i=0; i<NUM_OF_SAVED_WORDS; i++)
    {
        if(!strcmp(all_saved_words[i],word))
        {
            printf("Error: Illegal label %s at line %d - label can't match a saved word\n", word, data->line_counter);
            data->number_of_errors++;
            return FALSE;
        }
    }
    for(i=0; i<data->label_counter; i++)
    {
        if(!strcmp(word,data->labelArr[i].name))
        {
            printf("Error: Illegal label %s at line %d - label name already exists\n", word, data->line_counter);
            data->number_of_errors++;
            return FALSE;
        }
    }
    return TRUE;
}

boolean isRegister(char* word)
{
    int i;
    for(i=0;i<NUM_OF_REGISTERS;i++)
    {
        if(!strcmp(word,register_names[i])) return TRUE;
    }
    return FALSE;
}

Register getRegister(char* word)
{
    int i,n = strlen(word);
    if(word[n-1] == ',') word[n-1] = '\0';
    for(i=0;i<NUM_OF_REGISTERS;i++)
    {
        if(!strcmp(word,register_names[i])){ return i; }
    }
}

boolean isOperandANumber(char* word)
{
    if (word[0] == '#')
        word++;
    if(atoi(word)) return TRUE;
    return FALSE;
}

int getNumberFromOperand(char* word)
{
    if(word[0] == '#')
        word++;
    return atoi(word);
}

boolean isCommand(char* word)
{
    int i;
    for(i=0; i<NUM_OF_COMMANDS; i++)
    {
        if(!strcmp(word,command_names[i])) return TRUE;
    }
    return FALSE;
}

boolean isDirective(char* word)
{
    int i;
    for(i=0; i<NUM_OF_DIRECTIVE_TYPES; i++)
    {
        if(!strcmp(word,directive_names[i])) return TRUE;
    }
    return FALSE;
}

boolean isEntry(char* word)
{
    if(!strcmp(word,".entry")) return TRUE;
    return FALSE;
}

boolean isExternal(char* word)
{
    if(!strcmp(word,".extern")) return TRUE;
    return FALSE;
}

commandType returnCommandType(char* word)
{
    int i;
    for(i=0;i<NUM_OF_COMMANDS;i++)
        if(!strcmp(word,command_names[i])) return i;
    return -1;
}

directiveType returnDirectiveType(char* word)
{
    int i;
    for(i=0;i<NUM_OF_DIRECTIVE_TYPES;i++)
        if(!strcmp(word,directive_names[i])) return i;
    return -1;
}

unsigned int checkICForTwoOperands(char* op1, char* op2)
{
    unsigned int retVal = 3;
    int i;
    for(i=0;i<NUM_OF_REGISTERS;i++)
    {
        if(!strcmp(op1,register_names[i])){ retVal -= 1; break;}
    }
    for(i=0;i<NUM_OF_REGISTERS;i++)
    {
        if(!strcmp(op2,register_names[i])){ retVal -= 1; break;}
    }
    return retVal;
}

unsigned int checkICForOneOperand(char* op)
{
    unsigned int retVal = 2;
    int i;
    for(i=0;i<NUM_OF_REGISTERS;i++)
    {
        if(!strcmp(op,register_names[i])){ retVal -= 1; break;}
    }
    return retVal;
}

unsigned int checkDCForData(char* words[],int i ,int n)
{
    unsigned int retVal = 0;
    for(i=0;i<n;i++)
    {
        if(atoi(words[i]))retVal++;
    }
    return retVal;
}

unsigned int checkDCForString(char* str)
{
    int n = strlen(str);
    if(str[0] == '"' && str[n-1] == '"') return n-1;
}

unsigned int getOPCODE(commandType command)
{
    switch(command)
    {
        case 2:
        case 3:
            return 2;
            break;
        case 5:
        case 6:
        case 7:
        case 8:
            return 5;
            break;
        case 9:
        case 10:
        case 11:
            return 9;
            break;
        default:
            return command;
            break;
    }
}

unsigned int getFunct(char* commandName)
{
    int i;
    for(i=0;i<NUM_OF_COMMANDS;i++)
        if(!strcmp(commandName,command_names[i])) break;
    switch(i)
    {
        case 2:
        case 5:
        case 9:
            return 1;
            break;
        case 3:
        case 6:
        case 10:
            return 2;
            break;
        case 7:
        case 11:
            return 3;
            break;
        case 8:
            return 4;
            break;
        default:
            return 0;
            break;
    }
}

unsigned int getAddressingForSourceOperand(char* op, Data* data)
{
    if(isOperandANumber(op)) return IMMEDIATE_ADDRESS;
    else if(isRegister(op)) return REGISTER_ADDRESS;
    else if(doesLabelExist(op,data)) return DIRECT_ADDRESS;
    else return -1; /* Error */
}

unsigned int getAddressingForDestinationOperand(char* op, Data* data)
{
    if(isOperandANumber(op)) return IMMEDIATE_ADDRESS;
    else if(doesLabelExist(op,data)) return DIRECT_ADDRESS;
    else if(op[0] == '&' && doesLabelExist(++op,data)) return RELATIVE_ADDRESS;
    else if(isRegister(op)) return REGISTER_ADDRESS;
    else return -1; /* Error */
}

boolean doesLabelExist(char* label, Data* data)
{
    int i = 0;
    for(i=0; i<data->label_counter; i++)
        if(!strcmp(label,data->labelArr[i].name))return TRUE;
    return FALSE;
}

int getLabelAddress(char* label, Data* data)
{
    int i = 0;
    for(i=0; i<data->label_counter; i++)
        if(!strcmp(label,data->labelArr[i].name)) return data->labelArr[i].address;
    printf("Error: Illegal input for directive .entry at line: %d\n", data->line_counter);
    data->number_of_errors++;
    return;
}

Label* getLabel(char* name, Data* data)
{
    int i;
    if(name[0] == '&')
        name++;
    for(i=0;i<data->label_counter;i++)
        if(!strcmp(data->labelArr[i].name,name)) return &(data->labelArr[i]);
    return NULL;
}

void checkTwoOperands(char* words[], int* commandIndex, int* numOfWords, Data* data, commandType currentCommand)
{
    char *op1 = NULL, *op2 = NULL;
    int n = strlen(words[*commandIndex+1]);
    if(*numOfWords-*commandIndex == 2)
    {
        op1 = strtok(words[*commandIndex+1],",");
        op2 = strtok(NULL,",");
        if(op1 == NULL)
        {
            printf("Error: Source operand does not exist at line: %d\n", data->line_counter);
            data->number_of_errors++;
            return;
        }
        if(op2 == NULL)
        {
            printf("Error: Destination operand does not exist at line: %d\n", data->line_counter);
            data->number_of_errors++;
            return;
        }
        words[*commandIndex+2] = malloc(strlen(op2)+1);
        strcpy(words[*commandIndex+2],op2);
        *numOfWords++;
    }

    else if(words[*commandIndex+1][n-1] == ',')
    {
        words[*commandIndex+1][n-1] = 0;
        op1 = words[*commandIndex+1];
        op2 = words[*commandIndex+2];

    }
    switch(currentCommand)
    {
        case 0:
        case 2:
        case 3:
            if(!doesLabelExist(op1,data) && !isRegister(op1) && !isOperandANumber(op1)){
                printf("Error: Illegal source operand at line: %d\n", data->line_counter);
                data->number_of_errors++;
            }
            if(!doesLabelExist(op2,data) && !isRegister(op2)){
                printf("Error: Illegal destination operand at line: %d\n", data->line_counter);
                data->number_of_errors++;
            }
            break;
        case 1:
            if(!doesLabelExist(op1,data) && !isRegister(op1) && !isOperandANumber(op1)){
                printf("Error: Illegal source operand at line: %d\n", data->line_counter);
                data->number_of_errors++;
            }
            if(!doesLabelExist(op2,data) && !isRegister(op2) && !isOperandANumber(op2)){
                printf("Error: Illegal destination operand at line: %d\n", data->line_counter);
                data->number_of_errors++;
            }
            break;
        case 4:
            if(!doesLabelExist(op1,data)){
                printf("Error: Illegal source operand at line: %d\n", data->line_counter);
                data->number_of_errors++;
            }
            if(!doesLabelExist(op2,data) && !isRegister(op2)){
                printf("Error: Illegal destination operand at line: %d\n", data->line_counter);
                data->number_of_errors++;
            }
            break;
    }
}

void checkOneOperand(char* words[], int operandIndex, Data* data, commandType currentCommand)
{
    unsigned int currentAddressing = getAddressingForDestinationOperand(words[operandIndex],data);
    switch(currentCommand)
    {
        case 5:
        case 6:
        case 7:
        case 8:
        case 12:
            if(currentAddressing != DIRECT_ADDRESS && currentAddressing != REGISTER_ADDRESS)
            {
                printf("Error: Illegal destination operand at line: %d\n", data->line_counter);
                data->number_of_errors++;
                return;
            }
            break;
        case 9:
        case 10:
        case 11:
            if(currentAddressing != DIRECT_ADDRESS && currentAddressing != RELATIVE_ADDRESS)
            {
                printf("Error: Illegal destination operand at line: %d\n", data->line_counter);
                data->number_of_errors++;
                return;
            }
            break;
        case 13:
            if(currentAddressing != DIRECT_ADDRESS && currentAddressing != REGISTER_ADDRESS && currentAddressing != IMMEDIATE_ADDRESS)
            {
                printf("Error: Illegal destination operand at line: %d\n", data->line_counter);
                data->number_of_errors++;
                return;
            }
            break;
    }
}

/* This function checks if a given number is a valid integer */
boolean is_valid_int(char *str)
{
   if (*str == '-')
      ++str;
   if (!*str)
      return FALSE;
   while (*str)
   {
      if (!isdigit(*str))
         return FALSE;
      else
         ++str;
   }
   return TRUE;
}














