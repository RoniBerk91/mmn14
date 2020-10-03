/**
    This header file contains all program definitions and datatypes required
**/

#ifndef DEFINITIONS_H_
#define DEFINITIONS_H_

#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

/* Macros */
#define MAX_MEMORY_ADDRESS (pow(2,21)-1)
#define WORD_BIT_LENGTH 24
#define WORD_HEX_LENGTH 6
#define FIRST_ADDRESS 100

/* addressing methods */
#define IMMEDIATE_ADDRESS 0
#define DIRECT_ADDRESS 1
#define RELATIVE_ADDRESS 2
#define REGISTER_ADDRESS 3

/* Limits */
#define MAX_FILE_NAME 100
#define MAX_NO_LINES 500
#define MAX_LINE_LENGTH 80
#define MAX_WORDS_PER_LINE 80
#define MAX_LABEL_LENGTH 31
#define NUM_OF_SAVED_WORDS 28
#define NUM_OF_REGISTERS 8
#define NUM_OF_COMMANDS 16
#define NUM_OF_DIRECTIVE_TYPES 4

/* Data Structures and types */
typedef enum {FALSE,TRUE} boolean;
typedef enum {COMMAND,DIRECTIVE} labelType;
typedef enum {DATA,STRING,ENTRY,EXTERN} directiveType;
typedef enum {mov,cmp,add,sub,lea,clr,not,inc,dec,jmp,bne,jsr,red,prn,rts,stop} commandType;
typedef enum {r0,r1,r2,r3,r4,r5,r6,r7} Register;
typedef enum {INSTRUCTION_WORD,EXTRA_WORD,DATA_WORD}instructionType;

/* Structure to store a label */
typedef struct{
    char name[MAX_LABEL_LENGTH];
    unsigned int address;
    labelType type;
    boolean isExternal;
}Label;

/* Structure to store various types of instructions */
typedef struct{
    instructionType myType;
    union
    {
        struct
        {
            unsigned int E : 1;
            unsigned int R : 1;
            unsigned int A : 1;
            unsigned int funct : 5;
            unsigned int destination_register : 3;
            unsigned int destination_addressing : 2;
            unsigned int source_register : 3;
            unsigned int source_addressing : 2;
            unsigned int opcode : 6;
        }Ins;
        struct
        {
            unsigned int E : 1;
            unsigned int R : 1;
            unsigned int A : 1;
            unsigned int address : 21;
        }Extra;
        struct
        {
            unsigned int value : 24;
        }Value;
    };
}Instruction;

/* Structure to store all data from file in dynamic arrays, each array has it's own counter */
typedef struct{
    unsigned int number_of_errors;
    unsigned int line_counter;
    unsigned int label_counter;
    unsigned int instruction_counter;
    unsigned int entry_counter;
    unsigned int extern_counter;
    unsigned int DC;
    unsigned int IC;
    Label* labelArr;
    Label* entryArr;
    Label* externArr;
    Instruction* instructionArr;
    Instruction* dataArr;
}Data;

/*-----Function declerations----- */
/* main.c */
void initGobalData(Data* data);
void freeAllData(Data* data);

/* first_pass.c */
void firstFileRead(char* filename, Data* data);
void freeWordsMemory(char* words[], int n);
void checkWords(char* words[], int numOfWords, Data* data);
void addToLabelTable(char* words[], int numOfWords, Data* data);
void updateDataAddresses(Data* data);

/* second_pass.c */
void secondFileRead(char* filename, Data* data);
void buildInstruction(char* words[], unsigned int numOfWords, Data* data);
void createTwoOpInstruction(char* words[], Data* data, int wordIndex, int tmpCounter);
void createOneOpInstruction(char* words[], Data* data, int wordIndex, int tmpCounter);
void createZeroOpInstruction(char* words[], Data* data);
void createExtraWord(char* operand, Data* data, unsigned int index);
void createExtraDataWord(char* operand, Data* data, unsigned int index);
void createJumpExtraWord(char* operand, Data* data, unsigned int index);
void pushNumbers(char* words[],unsigned int firstIndex, unsigned int numOfWords, Data* data);
void pushString(char* words[], unsigned int firstIndex, Data* data);
void addToExternArr(char* name, int index, Data* data);

/* data_checker.c */
boolean isLabel(char* words);
void isLabelOk(char* word, Data* data);
boolean isRegister(char* word);
Register getRegister(char* word);
boolean isOperandANumber(char* word);
int getNumberFromOperand(char* word);
boolean isCommand(char* word);
boolean isDirective(char* word);
boolean isEntry(char* word);
boolean isExternal(char* word);
commandType returnCommandType(char* word);
directiveType returnDirectiveType(char* word);
unsigned int checkICForTwoOperands(char* op1, char* op2);
unsigned int checkICForOneOperand(char* op);
unsigned int checkDCForData(char* words[],int i ,int n);
unsigned int checkDCForString(char* str);
unsigned int getOPCODE(commandType command);
unsigned int getFunct(char* commandName);
unsigned int getAddressingForSourceOperand(char* op, Data* data);
unsigned int getAddressingForDestinationOperand(char* op, Data* data);
boolean doesLabelExist(char* label, Data* data);
int getLabelAddress(char* label, Data* data);
Label* getLabel(char* name, Data* data);
void checkTwoOperands(char* words[], int* commandIndex, int* numOfWords, Data* data, commandType currentCommand);
void checkOneOperand(char* words[], int operandIndex, Data* data, commandType currentCommand);
boolean is_valid_int(char *str);

/* file_creator.c */
void createOutputFiles(char* filename, Data* data, unsigned int DCF, unsigned int ICF);
void createOBFile(char* filename, Data* data, unsigned int DCF, unsigned int ICF);
void createEntFile(char* filename, Data* data);
void createExtFile(char* filename, Data* data);
char *removeExtention(char* myStr);
void writeInstructionToFile(FILE* fp,Instruction currentInstruction, Data* data);
void writeDataToFile(FILE* fp, unsigned int number, Data* data);
void decToBitString(int num, char* str, int numOfBits);

#endif // DEFINITIONS_H_















