//
//  smm_object.h
//  SMMarble object
//
//  Created by Juyeop Kim on 2023/11/05.
//

#ifndef smm_object_h
#define smm_object_h
#endif
/* node type :
    lecture,
    restaurant,
    laboratory,
    home,
    experiment,
    foodChance,
    festival
*/
#define SMMNODE_TYPE_LECTURE             0
#define SMMNODE_TYPE_RESTAURANT          1
#define SMMNODE_TYPE_LABORATORY          2
#define SMMNODE_TYPE_HOME                3
#define SMMNODE_TYPE_GOTOLAB             4
#define SMMNODE_TYPE_FOODCHANGE          5
#define SMMNODE_TYPE_FESTIVAL            6

#define SMMNODE_OBJTYPE_BOARD   0
#define SMMNODE_OBJTYPE_GRADE   1
#define SMMNODE_OBJTYPE_FOOD    2
#define SMMNODE_OBJTYPE_FEST    3

/* grade :
    AP,
    A0,
    AM,
    BP,
    B0,
    BM,
    CP,
    C0,
    CM
*/
#define SMMNODE_MAX_GRADE       13


//object generation
void* smmObj_genObject(char* name, int odjType, int type, int credit, int energy, int grade);
char* smmObj_getObjectName(void *ptr);
int smmObj_getObjectType(void *ptr);
int smmObj_getObjectEnergy(void *ptr);
int smmObj_getObjectCredit(void *ptr);
char* smmObj_getObjectTypeName(void *ptr);
char* smmObj_getGradeName(void *ptr);
/* smm_object_h */
