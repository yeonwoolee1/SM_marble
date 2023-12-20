//
//  main.c
//  SMMarble
//
//  Created by Yeonwoo Lee on 2023/12/21.
//

#include <time.h>
#include <string.h>
#include "smm_object.h"
#include "smm_database.h"
#include "smm_common.h"

#define BOARDFILEPATH "marbleBoardConfig.txt"
#define FOODFILEPATH "marbleFoodConfig.txt"
#define FESTFILEPATH "marbleFestivalConfig.txt"



//board configuration parameters
static int board_nr;
static int food_nr;
static int festival_nr;

static int player_nr;

//
typedef struct player{
	int energy;
	int position;
	char name[MAX_CHARNAME];
	int accumCredit;
	int flag_graduate;
}player_t;

static player_t*cur_player;



//function prototypes
#if 0
int isGraduated(void); //check if any player is graduated
void printGrades(int player);  //print grade history of the player
void goForward(int player, int step); //make player go "step" steps on the board (check if player is graduated)
void printPlayerStatus(void); //print all player status at the beginning of each turn
float calcAverageGrade(int player); //calculate average grade of the player
smmGrade_e takeLecture(int player, char *lectureName, int credit); //take the lecture (insert a grade of the player)
void* findGrade(int player, char *lectureName); //find the grade from the player's grade history
void printGrades(int player); //print all the grade history of the player
#endif

//
void printGrades(int player)
{
	int i;
	void *gradePtr;
	for(i=0;i<smmdb_len(LISTNO_OFFSET_GRADE+player);i++)
	{
		gradePtr=smmdb_getData(LISTNO_OFFSET_GRADE+player, i);
		printf("%s : %i\n", smmObj_getNodeName(gradePtr), smmObj_getNodeGrade(gradePtr));
	}
}
		//내가 그 과목을 들었는지도 확인할 수 있는 코딩에 응용 가능
		//조건문으로 막아야 함, 성적표를 뒤지면서 판단하는 함수 필요
		 
//
void printPlayerStatus(void)
{
	int i;
	for(i=0;i<player_nr;i++)
	{
		printf("%s : credit %i, energy %i, position %i\n", 
                      cur_player[i].accumCredit,
                      cur_player[i].energy,
                      cur_player[i].position,
					  cur_player[i].name);
	}
}
void generatePlayers(int n, int initEnergy) //generate a new player
{
	int i; 
	//n time loop
	for(i=0; i<n; i++)
	{
		//input name
		printf("input player %i's name:",i);
		scanf("%s",cur_player[i].name);
		fflush(stdin);
		
		//set position
	   //player_position[i]=0;
		cur_player[i].position=0;
		
		//set energy
		//player_energy[i]=initEnergy;
		cur_player[i].energy= initEnergy;
		
		//set accumCredit
		cur_player[i].accumCredit=0;
		cur_player[i].flag_graduate=0;
		
	}
	
}

int rolldie(int player)
{
    char c;
    printf(" Press any key to roll a die (press g to see grade): ");
    c = getchar();
    fflush(stdin);
    
	#if 0
    if (c == 'g')
        printGrades(player);
	#endif
    
    return (rand()%MAX_DIE + 1);
}

#if 0
//action code when a player stays at a node
void actionNode(int player)
{
	void*boardPtr=smmdb_getData(LISTNO_NODE, cur_player[player].position);
	//int type=smmObj_getNodeType(cur_player[player].position);
	int type = smmObj_getNodeType(boardPtr);
    char *name = smmObj_getNodeName(boardPtr);
    void *gradePtr;
    
    switch(type)
    {
        //case lecture:
        case SMMNODE_TYPE_LECTURE:
        	cur_player[player].accumCredit += smmObj_getNodeCredit(boardPtr);
            cur_player[player].energy -= smmObj_getNodeEnergy(boardPtr);
            
            //grade generation
            gradePtr = smmObj_genObject(name, smmObjType_grade, 0, smmObj_getNodeCredit(boardPtr), 0, rand()%9);
            //
			smmdb_addTail(LISTNO_OFFSET_GRADE + player, gradePtr);
        
            break;
            #if 0
		case SMMNODE_TYPE_RESTAURANT:
			break;
		
		case SMMNODE_TYPE_LABORATORY:
			break;
			
		case SMMNODE_TYPE_HOME:
			break;
			
		case SMMNODE_TYPE_GOTOLAB:
			break;
			
		case SMMNODE_TYPE_FOODCHANCE:
			break;
			
		case SMMNODE_TYPE_FESTIVAL:
			break;
		#endif
		
		default:	
			break;	
    }
}
//강의를 수강한 후에 성적이 산출되는 것이므로  
#endif

void goForward(int player, int step)
{	
	int i;
	void*boardPtr;
	
	for(i=0;i<step;i++)
	{	
		if(cur_player[player].position<board_nr-1)
			cur_player[player].position+=1;
		else
			cur_player[player].position=0;
	}
		
	
	cur_player[player].position +=step;
	 boardPtr = smmdb_getData(LISTNO_NODE, cur_player[player].position );
     
     printf("%s go to node %i (name: %s)\n", 
                cur_player[player].name, cur_player[player].position,
                smmObj_getNodeName(boardPtr));
}

int main(int argc, const char * argv[]) {
    
    FILE* fp;
    char name[MAX_CHARNAME];
    int type;
    int credit;
    int energy;
    int i;
    int initEnergy;
    int turn=0;
    
    board_nr = 0;
    food_nr = 0;
    festival_nr = 0;
    
    srand(time(NULL));
    
    #if 0
    //1. import parameters ---------------------------------------------------------------------------------
    
	//1-1. boardConfig 
    if ((fp = fopen(BOARDFILEPATH,"r")) == NULL)
    {
        printf("[ERROR] failed to open %s. This file should be in the same directory of SMMarble.exe.\n", BOARDFILEPATH);
        getchar();
        return -1;
    }
    
    printf("Reading board component......\n");
    while (fscanf(fp, "%s %i %i %i", name, &type, &credit, &energy)==4) //read a node parameter set
    {
		//store the parameter set
		// mmObj_genobject(char*name, int type_e type, int credit, int energy, int type_e grade )
    	void* boardObj= smmObj_genObject(name, smmObjType_board, type, credit, energy, 0);
    	smmdb_addTail(LISTNO_NODE, boardObj); 
		
		if(type==SMMNODE_TYPE_HOME)
			initEnergy=energy;
    	board_nr++;
    }
    fclose(fp);
    printf("Total number of board nodes : %i\n", board_nr);
    
    //board node?
    for (i=0; i<board_nr;i++)
    {
    	 printf("node %i : %s, %i(%s), credit %i, energy %i\n", 
                     i, smmObj_getNodeName(boardObj), 
                     smmObj_getNodeType(boardObj), smmObj_getTypeName(smmObj_getNodeType(boardObj)),
                     smmObj_getNodeCredit(boardObj), smmObj_getNodeEnergy(boardObj));
    }
    
    
    //2. food card config 
    if ((fp = fopen(FOODFILEPATH,"r")) == NULL)
    {
        printf("[ERROR] failed to open %s. This file should be in the same directory of SMMarble.exe.\n", FOODFILEPATH);
        return -1;
    }
    
    printf("\n\nReading food card component......\n");
    while () //read a food parameter set
    {
        //store the parameter set
    }
    fclose(fp);
    printf("Total number of food cards : %i\n", food_nr);
    
    
    
    //3. festival card config 
    if ((fp = fopen(FESTFILEPATH,"r")) == NULL)
    {
        printf("[ERROR] failed to open %s. This file should be in the same directory of SMMarble.exe.\n", FESTFILEPATH);
        return -1;
    }
    
    printf("\n\nReading festival card component......\n");
    while () //read a festival card string
    {
        //store the parameter set
    }
    fclose(fp);
    printf("Total number of festival cards : %i\n", festival_nr);
    # endif
    
    //2. Player configuration ---------------------------------------------------------------------------------
    
    do
    {
        //input player number to player_nr
        printf("input player no.:");
        scanf("%d", &player_nr);
        fflush(stdin);
    }
    while (player_nr < 0 || player_nr >  MAX_PLAYER);
    
    cur_player=(player_t*)malloc(player_nr*sizeof(player_t));
    generatePlayers(player_nr, initEnergy);
    
    #if 0
    //3. SM Marble game starts ---------------------------------------------------------------------------------
    while () //is anybody graduated?
    {
        int die_result;
        
        //4-1. initial printing
        //printPlayerStatus();
        
        //4-2. die rolling (if not in experiment)
        die_result=rolldie(turn);
        
        
        //4-3. go forward
        goForward();

		//4-4. take action at the destination node of the board
        actionNode(turn);
        
        //4-5. next turn
        turn=(turn+1)%player_nr;
        
    }
	#endif

    free(cur_player);
    return 0;
}
