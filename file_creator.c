/**
    This file contains all functions required for creating all output files
**/

#include "definitions.h"

/* This function is called when no errors are found and output files are created */
void createOutputFiles(char* filename, Data* data, unsigned int DCF, unsigned int ICF)
{
    createOBFile(removeExtention(filename),data,DCF,ICF);
    if(data->entry_counter > 0)
        createEntFile(removeExtention(filename),data);
    if(data->extern_counter > 0)
        createExtFile(removeExtention(filename),data);
}

/* This function creates the instruction file(.ob) with the given file name using the required data structures */
void createOBFile(char* filename, Data* data, unsigned int DCF, unsigned int ICF)
{
    int i;
    char ObName[MAX_FILE_NAME];
    data->IC = 100;
    data->DC = 0;
    strcpy(ObName,filename);
    strcat(ObName,".ob");
    FILE* obFile = fopen(ObName,"w+");
    fprintf(obFile,"%5d%5d\n",ICF-data->IC,DCF);
    for(i=0; i<data->instruction_counter;i++)
    {
        writeInstructionToFile(obFile,data->instructionArr[i],data);
    }
    for(i=0;i<DCF;i++)
    {
        writeDataToFile(obFile,data->dataArr[i].Value.value,data);
    }
    printf("Created %s file\n", ObName);
    fclose(obFile);
}

/* This function creates the entry file(.ent) with the given file name using the required data structures */
void createEntFile(char* filename, Data* data)
{
    int i;
    char entName[MAX_FILE_NAME];
    strcpy(entName,filename);
    strcat(entName,".ent");
    FILE* entFile = fopen(entName,"w+");
    for(i=0;i<data->entry_counter;i++)
        fprintf(entFile,"%-10s%07d\n",data->entryArr[i].name,data->entryArr[i].address);
    printf("Created %s file\n", entName);
    fclose(entFile);
}

/* This function creates the externals file(.ext) with the given file name using the required data structures */
void createExtFile(char* filename, Data* data)
{
    int i;
    char extName[MAX_FILE_NAME];
    strcpy(extName,filename);
    strcat(extName,".ext");
    FILE* extFile = fopen(extName,"w+");
    for(i=0;i<data->extern_counter;i++)
        fprintf(extFile,"%-10s%07d\n",data->externArr[i].name,data->externArr[i].address);
    printf("Created %s file\n", extName);
    fclose(extFile);
}

/* This function removes the extension from the passed filename passed as a string  */
char *removeExtention(char* myStr)
{
    char *retStr;
    char *lastExt;
    if (myStr == NULL) return NULL;
    if ((retStr = malloc (strlen (myStr) + 3)) == NULL) return NULL;
    strcpy (retStr, myStr);
    lastExt = strrchr (retStr, '.');
    if (lastExt != NULL)
        *lastExt = '\0';
    return retStr;
}

/* This function is called for the creation of ob file, to write each instruction on a new line*/
void writeInstructionToFile(FILE* fp,Instruction currentInstruction, Data* data)
{
    char binaryNumber[WORD_BIT_LENGTH] = {0};
    char hexNumber[WORD_HEX_LENGTH] = {0};
    switch(currentInstruction.myType)
    {
        case INSTRUCTION_WORD:
            decToBitString(currentInstruction.Ins.opcode,binaryNumber,6);
            decToBitString(currentInstruction.Ins.source_addressing,binaryNumber,2);
            decToBitString(currentInstruction.Ins.source_register,binaryNumber,3);
            decToBitString(currentInstruction.Ins.destination_addressing,binaryNumber,2);
            decToBitString(currentInstruction.Ins.destination_register,binaryNumber,3);
            decToBitString(currentInstruction.Ins.funct,binaryNumber,5);
            decToBitString(currentInstruction.Ins.A,binaryNumber,1);
            decToBitString(currentInstruction.Ins.R,binaryNumber,1);
            decToBitString(currentInstruction.Ins.E,binaryNumber,1);
            break;
        case EXTRA_WORD:
            decToBitString(currentInstruction.Extra.address,binaryNumber,21);
            decToBitString(currentInstruction.Extra.A,binaryNumber,1);
            decToBitString(currentInstruction.Extra.R,binaryNumber,1);
            decToBitString(currentInstruction.Extra.E,binaryNumber,1);
            break;
    }
    binaryToHex(binaryNumber,hexNumber);
    fprintf(fp,"%07d\t%s\n",data->IC,hexNumber);
    data->IC++;
}

/* This function is called for the creation of ob file, to write each data value on a new line*/
void writeDataToFile(FILE* fp, unsigned int number, Data* data)
{
    char binaryNumber[WORD_BIT_LENGTH] = {0};
    char hexNumber[WORD_HEX_LENGTH] = {0};
    decToBitString(number,binaryNumber,24);
    binaryToHex(binaryNumber,hexNumber);
    fprintf(fp,"%07d\t%s\n",data->IC+data->DC,hexNumber);
    data->DC++;
}

/* This function is called in order to convert each  */
void decToBitString(int num, char* str, int numOfBits)
{
    char tmp[WORD_BIT_LENGTH] = {0};
    while(numOfBits > 0)
    {
        if(num&1)strcat(tmp,"1");
        else strcat(tmp,"0");
        num >>= 1;
        numOfBits--;
    }
    strcat(str,strrev(tmp));
}

void binaryToHex(char *inStr, char *outStr)
{
    char hex[] = "0123456789ABCDEF";
    int len = strlen(inStr) / 4;
    int i = strlen(inStr) % 4;
    char current = 0;
    if(i) {
        while(i--) {
            current = (current << 1) + (*inStr - '0');
            inStr++;
        }
        *outStr = hex[current];
        ++outStr;
    }
    while(len--) {
        current = 0;
        for(i = 0; i < 4; ++i) {
            current = (current << 1) + (*inStr - '0');
            inStr++;
        }
        *outStr = hex[current];
        ++outStr;
    }
    *outStr = 0;
}











