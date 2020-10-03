/**
    This file is not part of the final project, functions here are used only to visualize data
**/

#include "definitions.h"

/* DELETE from final version */

void printLabeblTable(Data* data)
{
    int n = data->label_counter;
    int i;
    printf("All labels after first run\n");
    printf("%-10s%-10s%-10s%-10s\n","Name","Address","Type","Is External");
    for(i=0; i<n; i++)
    {
        printf("%-10s%-10d%-10d%-10d\n",data->labelArr[i].name,data->labelArr[i].address,data->labelArr[i].type,
               data->labelArr[i].isExternal);
    }
}

void printInstructionArray(Data* data)
{
    int i;
    printf("Instructions after second run(not by format)\n");
    for(i=0; i<data->instruction_counter; i++)
    {
        switch(data->instructionArr[i].myType)
        {
        case 0:
            printf("Command\n%-10d%-10d%-10d%-10d%-10d%-10d%-10d%-10d%-10d\n", data->instructionArr[i].Ins.opcode,
                   data->instructionArr[i].Ins.source_addressing, data->instructionArr[i].Ins.source_register,
                   data->instructionArr[i].Ins.destination_addressing, data->instructionArr[i].Ins.destination_register,
                   data->instructionArr[i].Ins.funct, data->instructionArr[i].Ins.A, data->instructionArr[i].Ins.R, data->instructionArr[i].Ins.E);
            break;
        case 1:
            printf("Extra\n%-10d%-10d%-10d%-10d\n",data->instructionArr[i].Extra.address, data->instructionArr[i].Extra.A,
                   data->instructionArr[i].Extra.R, data->instructionArr[i].Extra.E);
            break;
        }
    }
}

void printDataArray(Data* data)
{
    int i;
    printf("Data array\n");
    for(i=0;i<data->DC;i++)
    {
        printf("%-10d\n",data->dataArr[i].Value.value);
    }
}

void printEntryArray(Data* data)
{
    int i;
    printf("Entry array\n");
    printf("%-10s%-10s%-10s%-10s\n","Name","Address","Type","Is External");
    for(i=0; i<data->entry_counter;i++)
        printf("%-10s%-10d%-10d%-10d\n",data->entryArr[i].name,data->entryArr[i].address,data->entryArr[i].type,
               data->entryArr[i].isExternal);
}










