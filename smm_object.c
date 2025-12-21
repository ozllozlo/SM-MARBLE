//
//  smm_node.c
//  SMMarble
//
//  Created by Juyeop Kim on 2023/11/05.
//

#include "smm_common.h"
#include "smm_object.h"
#include <string.h>
#include <stdlib.h>

#define MAX_NODENR      100
#define MAX_NODETYPE    7


#define OBJTYPE_BOARD   0
#define OBJTYPE_GRADE   1
#define OBJTYPE_FOOD    2
#define OBJTYPE_FEST    3

#define GRADE_AP        0
#define GRADE_A0        1
#define GRADE_AM        2
#define GRADE_BP        3
#define GRADE_B0        4
#define GRADE_BM        5
#define GRADE_CP        6
#define GRADE_C0        7
#define GRADE_CM        8
#define GRADE_DP        9
#define GRADE_D0        10
#define GRADE_DM        11
#define GRADE_F         12

static char smmObj_nodeName[MAX_NODETYPE][MAX_CHARNAME] = {
     "lecture",
     "restaurant",
     "laboratory",
     "home",
     "gotoLab",
     "foodChance",
     "festival"
       };
       
static char smmObj_gradeName[SMMNODE_MAX_GRADE][MAX_CHARNAME] = {
     "AP",
     "A0",
     "AM",
     "BP",
     "B0",
     "BM",
     "CP",
     "C0",
     "CM",
     "DP",
     "D0",
     "DM",
     "F"
       };

//structure type definition

typedef struct {
        char name[MAX_CHARNAME];
        int objType;
        int type;
        int credit;
        int energy;
        int grade;
} smmObj_object_t;


//object generation
void* smmObj_genObject(char* name, int objType, int type, int credit, int energy, int grade)
{
    smmObj_object_t* ptr;
    ptr = (smmObj_object_t*)malloc(sizeof(smmObj_object_t));
    
    strcpy(ptr->name, name);
    ptr->type = type;
    ptr->energy = energy;
    ptr->credit = credit;
    ptr->objType = objType;
    ptr->grade = grade;

   return((void*)ptr);
}



//member retrieving
char* smmObj_getObjectName(void *ptr)
{
 smmObj_object_t* objPtr = (smmObj_object_t*)ptr;     
 
 return(objPtr->name);     
}

int smmObj_getObjectType(void *ptr)
{
 smmObj_object_t* objPtr = (smmObj_object_t*)ptr;     
 
 return(objPtr->type);   
}

int smmObj_getObjectCredit(void *ptr)
{
 smmObj_object_t* objPtr = (smmObj_object_t*)ptr;     
 
 return(objPtr->credit);
}

int smmObj_getObjectEnergy(void *ptr)
{
 smmObj_object_t* objPtr = (smmObj_object_t*)ptr;     
 
 return(objPtr->energy);     
}

char* smmObj_getObjectTypeName(void *ptr)
{
 smmObj_object_t* objPtr = (smmObj_object_t*)ptr;     
 
 return(smmObj_nodeName[objPtr->type]);     
}


char* smmObj_getGradeName(void *ptr)
{
 smmObj_object_t* objPtr = (smmObj_object_t*)ptr;
 return (smmObj_gradeName[objPtr->grade]);
}
