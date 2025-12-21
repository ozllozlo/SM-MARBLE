//
//  main.c
//  SMMarble
//
//  Created by Juyeop Kim on 2023/11/05.
//

#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "smm_object.h"
#include "smm_database.h"
#include "smm_common.h"

#define BOARDFILEPATH "marbleBoardConfig.txt"
#define FOODFILEPATH "marbleFoodConfig.txt"
#define FESTFILEPATH "marbleFestivalConfig.txt"

#define BOARDLIST      0
#define FOODLIST       1
#define FESTLIST       2


//board configuration parameters
static int smm_board_nr;
static int smm_food_nr;
static int smm_festival_nr;
static int smm_player_nr;

typedef struct{
    char name[MAX_CHARNAME];
    int pos;
    int credit;
    int energy;    
    int flag_graduated;
    int course_count;
    int grade;
    int is_experimenting;
    int experiment_target;
} smm_player_t;

smm_player_t *smm_players;


//function prototypes

int takeLecture(int player, char *lectureName, int credit) //take the lecture (insert a grade of the player)
{
           int grade = rand()%SMMNODE_MAX_GRADE;
           void* ptr = smmObj_genObject(lectureName, SMMNODE_OBJTYPE_GRADE, 0, credit, 0, grade);
           smmdb_addTail(LISTNO_OFFSET_GRADE + player, ptr);
           
           smm_players[player].course_count++;
           
           return grade;
           }

void* findGrade(int player, char *lectureName) //find the grade from the player's grade history
{
      int size = smmdb_len(LISTNO_OFFSET_GRADE+player);
      int i;
      
      for(i=0;i<size;i++)
      {
        void *ptr = smmdb_getData(LISTNO_OFFSET_GRADE+player, i);          
        if(strcmp(smmObj_getObjectName(ptr), lectureName) == 0)
        {
          return ptr;                                     
        }       
      }
      return NULL;
}
int isGraduated(void)
{
    int i;
    for(i=0;i<smm_player_nr;i++)
    {
     if (smm_players[i].credit >= GRADUATE_CREDIT && smm_players[i].pos == 0)
     {
     return i+1;
     }  
     }
    
    return 0;
}

void goForward(int player, int step)
{
   int i;
   void *ptr = smmdb_getData(LISTNO_NODE, smm_players[player].pos);
   printf("\n Moving from %i to forward %i steps...\n", smm_players[player].pos, step);  
   
   for(i=0;i<step;i++){
                                          
    smm_players[player].pos = (smm_players[player].pos + 1)%smm_board_nr; 
    void *nodePtr = smmdb_getData(LISTNO_NODE, smm_players[player].pos);      
    printf(" =>moved to %i(%s)\n", smm_players[player].pos, 
                                  smmObj_getObjectName(nodePtr));
                                  
    if(smm_players[player].pos==0){
    int recharge = smmObj_getObjectEnergy(nodePtr);
    smm_players[player].energy += recharge;
    printf("Passed Home! Energy +%i (Current: %i)\n", recharge, smm_players[player].energy);
    }                                                                        
    }
    }

void printGrades(int player)
{
    int list_index = LISTNO_OFFSET_GRADE + player;
    
    int count = smm_players[player].course_count;
    
    int i;
    for (i = 0; i < count; i++) 
    {
        void* ptr = smmdb_getData(list_index, i);
        
        char* name = smmObj_getObjectName(ptr);
        int credit = smmObj_getObjectCredit(ptr);
        int grade_idx = smmObj_getObjectGrade(ptr);
        
        printf("%s\t: %i credit, Grade %s\n", smm_players[player].name, credit, smmObj_getNodeGradeName(grade_idx));
    }
    printf("-----------------------------------\n");
}

void printPlayerStatus(void)
{    int i;
     for (i=0;i<smm_player_nr;i++)
     {   void *nodePtr = smmdb_getData(LISTNO_NODE, smm_players[i].pos);
         printf("%s - position: %i(%s), credit: %i, energy: %i\n",
         smm_players[i].name, smm_players[i].pos, smmObj_getObjectName(nodePtr), smm_players[i].credit, smm_players[i].energy);
     }
}

float calcAverageGrade(int player)
{
 
    int list_index = LISTNO_OFFSET_GRADE + player;
   
    int count = smm_players[player].course_count;
    
    if (count == 0) return 0.0;

    float total_score = 0.0;
    int total_credit = 0; 
    
    int i;
    for (i = 0; i < count; i++)
    {
        void* ptr = smmdb_getData(list_index, i);
        
        int credit = smmObj_getObjectCredit(ptr);
        int grade_idx = smmObj_getObjectGrade(ptr);
        
        total_score += smmObj_getNodeScore(grade_idx) * credit;
        
        total_credit += credit;
    }
   
    if (total_credit == 0) return 0.0;
    
    return total_score / total_credit;
}

void generatePlayers(int n, int initEnergy)
{
     int i;
     
     smm_players = (smm_player_t*)malloc(n*sizeof(smm_player_t));
     
     for (i=0;i<n;i++)
     {
         smm_players[i].pos = 0;
         smm_players[i].credit = 0;
         smm_players[i].energy = initEnergy;
         smm_players[i].flag_graduated = 0;
         smm_players[i].is_experimenting = 0; 
         smm_players[i].experiment_target = 0;
         smm_players[i].course_count = 0;
         
         
         printf("Input %d-th player name:", i+1);
         scanf("%s", &smm_players[i].name[0]);
        
     }
}

int rolldie(int player)
{
    char c;
    printf("Press any key to roll a die (press g to see grade): ");
    c = getchar();
    
    if (c == 'g'){
        printGrades(player);
        getchar();
        }
       
        int result = (rand()%MAX_DIE+1);
        printf("\n Die result: %i\n", result);
        return result;
    
}


//action code when a player stays at a node
void actionNode(int player)
{
    void *ptr = smmdb_getData(LISTNO_NODE, smm_players[player].pos);
    
    int type = smmObj_getObjectType(ptr);
    int credit = smmObj_getObjectCredit(ptr);
    int energy = smmObj_getObjectEnergy(ptr);
    int grade;
    void *gradePtr;
    
    printf("\n --> player %s pos: %i, type: %s, credit: %i, energy: %i\n",
           smm_players[player].name, smm_players[player].pos, smmObj_getObjectTypeName(ptr), credit, energy);
           
    switch(type)
    {
        case SMMNODE_TYPE_LECTURE:
             if (findGrade(player, smmObj_getObjectName(ptr)) == NULL && smm_players[player].energy >= energy)
             {
             printf("Take lecture %s? (y/n):\n", smmObj_getObjectName(ptr));
             
             char c;
             scanf(" %c", &c);
         
             if (c == 'y') {
             smm_players[player].credit += credit;
             smm_players[player].energy -= energy;

            grade = rand() % SMMNODE_MAX_GRADE;
            gradePtr = smmObj_genObject(smmObj_getObjectName(ptr), 
                           SMMNODE_OBJTYPE_GRADE, type, credit, energy, grade);
            smmdb_addTail(LISTNO_OFFSET_GRADE + player, gradePtr);
            printf("  -> Course registered.\n");
            }
            else 
            {
            printf("  -> Course skipped.\n");
            } } 
            else if (smm_players[player].energy < energy)
            {
            printf("  -> Not enough energy to take this lecture!\n");
            }
            break;
             
        case SMMNODE_TYPE_RESTAURANT:
             smm_players[player].energy += energy;
             break;
             
        case SMMNODE_TYPE_LABORATORY:
            
             
             if (smm_players[player].is_experimenting) {
             smm_players[player].energy -= energy; 
        
             int die = rand() % MAX_DIE + 1;
             printf("Experimenting... Die rolled: %i (Target: %i)\n", die, smm_players[player].experiment_target);
        
             if (die >= smm_players[player].experiment_target) {
             printf("SUCCESS! You can move in the next turn.\n");
             smm_players[player].is_experimenting = 0;
             } else {
             printf("FAILED. You stay in the laboratory.\n");
             }
             } else {
             printf("Just visiting the laboratory.\n");
             }
             break;
             
        case SMMNODE_TYPE_HOME:
             smm_players[player].energy += energy;
             if(smm_players[player].credit>= GRADUATE_CREDIT)
             { 
               smm_players[player].flag_graduated = 1;                             
             }
             break;
             
        case SMMNODE_TYPE_GOTOLAB:
             
             printf(" >> Mission: Go to laboratory! Setting experiment target...\n");
             smm_players[player].is_experimenting = 1; 
             smm_players[player].experiment_target = (rand() % MAX_DIE) + 1; 
             
             int i;
             for (i=0; i < smm_board_nr; i++) {
             void *searchNode = smmdb_getData(LISTNO_NODE, i);
             if (smmObj_getObjectType(searchNode) == SMMNODE_TYPE_LABORATORY) {
             smm_players[player].pos = i;
             printf("Immediately moved to node %i (Laboratory).\n", i);
            break;
             }
             }
             break;
             
        case SMMNODE_TYPE_FOODCHANGE:
             {
             if(smm_food_nr > 0){
              int cardIdx = rand()%smm_food_nr;
              void *foodCard = smmdb_getData(FOODLIST, cardIdx);
              
             if(foodCard != NULL){
              char* foodName = smmObj_getObjectName(foodCard);
              int foodEnergy = smmObj_getObjectEnergy(foodCard);
              
              printf("\n[CHANCE] food: %s! energy: %i\n", foodName, foodEnergy);
              smm_players[player].energy += foodEnergy;
              printf("%s's energy: %i\n", smm_players[player].name, smm_players[player].energy);
              }
             }else
              {
              printf("No Food Card!\n");        
              }                   
             }
             
             break;
             
        case SMMNODE_TYPE_FESTIVAL:
               if(smm_festival_nr > 0){
               
               int cardIdx = rand()%smm_festival_nr;
               void *festCard = smmdb_getData(FESTLIST, cardIdx);
               
               if(festCard != NULL){
               printf("[FESTIVAL] %s MISSION\n", smmObj_getObjectName(festCard));
               }
               }
             break;

        default:
            break;
    }
}



int main(int argc, const char * argv[]) {
    
    FILE* fp;
    char name[MAX_CHARNAME];
    int type;
    int credit;
    int energy;
    int turn;
    
    smm_board_nr = 0;
    smm_food_nr = 0;
    smm_festival_nr = 0;
    
    srand(time(NULL));
    
    
    //1. import parameters ---------------------------------------------------------------------------------
    //1-1. boardConfig 
    if ((fp = fopen(BOARDFILEPATH,"r")) == NULL)
    {
        printf("[ERROR] failed to open %s. This file should be in the same directory of SMMarble.exe.\n", BOARDFILEPATH);
        getchar();
        return -1;
    }
    
    printf("Reading board component......\n");
    while (fscanf(fp, "%s %i %i %i", name, &type, &credit, &energy)==4 ) //read a node parameter set
    {
        //store the parameter set
        void* ptr;
        printf("%s %i %i %i\n", name, type, credit, energy);
        ptr = smmObj_genObject(name, SMMNODE_OBJTYPE_BOARD, type, credit, energy, 0);
        /*smm_board_nr = smmdb_addTail(LISTNO_NODE, ptr);*/
        smmdb_addTail(LISTNO_NODE, ptr);
        smm_board_nr++;
    }
    fclose(fp);
    printf("Total number of board nodes : %i\n", smm_board_nr);
    
    
    
    //2. food card config 
    if ((fp = fopen(FOODFILEPATH,"r")) == NULL)
    {
        printf("[ERROR] failed to open %s. This file should be in the same directory of SMMarble.exe.\n", FOODFILEPATH);
        return -1;
    }
    
    printf("\n\nReading food card component......\n");
    while (fscanf(fp, "%s %i", name, &energy)==2 ) //read a node parameter set
    {
        //store the parameter set
        void* ptr;
        printf("%s %i\n", name,  energy);
        ptr = smmObj_genObject(name, SMMNODE_OBJTYPE_FOOD,0,0, energy,0);
        /*smm_food_nr = smmdb_addTail(FOODLIST, ptr);*/
        smmdb_addTail(FOODLIST, ptr);
        smm_food_nr++;
    }
    /*
    while () //read a food parameter set
    {
        //store the parameter set
    }
    */
    fclose(fp);
    printf("Total number of food cards : %i\n", smm_food_nr);

    
    
    //3. festival card config 
    if ((fp = fopen(FESTFILEPATH,"r")) == NULL)
    {
        printf("[ERROR] failed to open %s. This file should be in the same directory of SMMarble.exe.\n", FESTFILEPATH);
        return -1;
    }
    
    printf("\n\nReading festival card component......\n");
    while (fscanf(fp, "%s", name)==1 ) //read a node parameter set
    {
        //store the parameter set
        void* ptr;
        printf("%s\n", name);
        ptr = smmObj_genObject(name, SMMNODE_OBJTYPE_FEST, 0, 0, 0, 0);
        /*smm_festival_nr = smmdb_addTail(FESTLIST, ptr);*/
        smmdb_addTail(FESTLIST, ptr);
        smm_festival_nr++;
    }
    /*
    while () //read a festival card string
    {
        //store the parameter set
    }
    */

    fclose(fp);
    printf("Total number of festival cards : %i\n", smm_festival_nr);
    
 
    
    //2. Player configuration ---------------------------------------------------------------------------------
    
    do
    {
        //input player number to player_nr
        printf("Input player number:");
        scanf("%i",&smm_player_nr);
        fflush(stdin);
        
        if(smm_player_nr <= 0 || smm_player_nr > MAX_PLAYER)
        printf("Invalid player number!\n");
    }
    while (smm_player_nr<=0||smm_player_nr>MAX_PLAYER);
    
    void *homeNode = smmdb_getData(LISTNO_NODE, 0);
    int initEnergy = smmObj_getObjectEnergy(homeNode);
    
    generatePlayers(smm_player_nr, initEnergy);
    
    turn = 0;
    int winnerIndex = -1;
    //3. SM Marble game starts ---------------------------------------------------------------------------------
    while (isGraduated() == 0) //is anybody graduated?
    {
        int die_result;
        
        //4-1. initial printing
        printPlayerStatus();
        
        //4-2. die rolling (if not in experiment)
        if (smm_players[turn].is_experimenting == 0) {
        die_result = rolldie(turn);
        
        //4-3. go forward
        goForward(turn, die_result);
        } else {
        printf("%s is experimenting. Cannot move.\n",
        smm_players[turn].name);
        
        }
        
		//4-4. take action at the destination node of the board
        actionNode(turn);
  
        
        //4-5. next turn
        if(isGraduated() != 0) break;
        
        turn = (turn + 1)%smm_player_nr;
    }
    
    int winner = isGraduated() -1;
    
    printf("==============================\n");
    printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
    printf("=%s graduate! Congratulation!=\n", smm_players[winner].name);
    printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
    printf("==============================\n");
    
    printGrades(winner);
    
    free(smm_players);
    
    system("PAUSE");
    return 0;
}
