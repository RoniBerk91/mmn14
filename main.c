/**
    This project was written by Author1: and Author2:
    It implements the 2 pass assembler algorithm by going through all line argument files.
    First each file is passed to create a symbol table and to calculate DC and IC values.
    Second each line is translated to am instruction for creating final output.
    Finally if no errors were found the .ext, .ent and .ob files are created.
**/

#include "definitions.h"

int main(int argc, char* argv[])
{
    char filename[MAX_FILE_NAME];
    int i;
    unsigned int DCF,ICF;
    FILE* fp;
    if(argc==1)
    {
        printf("Error: No files to compile\n");
        return 1;
    }
    else
    {
        for(i=1; i<argc; i++)
        {
            strcpy(filename,argv[i]);
            fp = fopen(filename, "r");
            if (!fp)
            {
                printf("Error: The file named %s %s doesn't exist\n",argv[0],filename);
                printf("____________________________________________________________________\n");
                continue;
            }
            printf("Opened file: %s\n", filename);
            /* First Pass */
            Data* newFileData = (Data*)malloc(sizeof(Data));
            initGobalData(newFileData);
            firstFileRead(filename,newFileData);
            DCF = newFileData->DC;
            ICF = newFileData->IC;
            if(newFileData->number_of_errors > 0)
            {
                printf("Error: First pass found errors, program terminated\n");
                printf("____________________________________________________________________\n");
                continue;
            }
            /* Second Pass */
            newFileData->DC = 0;
            newFileData->IC = 100;
            newFileData->line_counter = 0;
            secondFileRead(filename,newFileData);
            if(newFileData->number_of_errors > 0)
            {
                printf("Error: Second pass found errors, program terminated\n");
                printf("____________________________________________________________________\n");
                continue;
            }
            /* End of validation section */
            else
            {
                createOutputFiles(filename,newFileData,DCF,ICF);
                printf("Program terminated successfully\n");
            }
            freeAllData(newFileData);
            free(newFileData);
            fclose(fp);
            printf("____________________________________________________________________\n");
        }
    }
}

void initGobalData(Data* data)
{
    data->number_of_errors = 0;
    data->line_counter = 0;
    data->label_counter = 0;
    data->instruction_counter = 0;
    data->entry_counter = 0;
    data->extern_counter = 0;
    data->DC = 0;
    data->IC = 100;
    data->labelArr = NULL;
    data->instructionArr = NULL;
    data->entryArr = NULL;
    data->dataArr = NULL;
    data->externArr = NULL;
}

void freeAllData(Data* data)
{
    if(data->label_counter > 0)
        free(data->labelArr);
    if(data->instruction_counter > 0)
        free(data->instructionArr);
    if(data->DC > 0)
        free(data->dataArr);
    if(data->entry_counter > 0)
        free(data->entryArr);
    if(data->extern_counter > 0)
        free(data->externArr);
}






