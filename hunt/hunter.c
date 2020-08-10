////////////////////////////////////////////////////////////////////////
// COMP2521 20T2 ... the Fury of Dracula
// hunter.c: your "Fury of Dracula" hunter AI.
//
// 2014-07-01   v1.0    Team Dracula <cs2521@cse.unsw.edu.au>
// 2017-12-01   v1.1    Team Dracula <cs2521@cse.unsw.edu.au>
// 2018-12-31   v2.0    Team Dracula <cs2521@cse.unsw.edu.au>
// 2020-07-10   v3.0    Team Dracula <cs2521@cse.unsw.edu.au>
//
////////////////////////////////////////////////////////////////////////

#include "Game.h"
#include "hunter.h"
#include "HunterView.h"
#include "Map.h"
#include "GameView.h"

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

//HELPER FUNCTIONS DECLARATION//
PlaceId *storeRedPatrol   ();
PlaceId *storeBluePatrol  ();
PlaceId *storeGreenPatrol ();
PlaceId *storePinkPatrol  ();

int  inZone            (HunterView hv, Player player);
int  searchStorePatrol (PlaceId *patrol, PlaceId location);
void patrolCurrent     (HunterView hv, Player hunter);

void goToDrac (HunterView hv, Player hunter);

PlaceId *ifDracAtCD (HunterView hv, Player hunter, PlaceId dest, Round round, int *pathLength);
void LookForDrac(HunterView hv, PlaceId lastSeenDrac, int* numRetLocs, Player Hunter);
PlaceId *GetConnects(HunterView hv, Player player,
                                bool road, bool rail, bool boat,
                                int *numReturnedLocs, PlaceId location);

void MoveToConnection(HunterView hv, Player hunter);


//STORE PATROL ZONES
PlaceId *storeRedPatrol () {

    PlaceId *patrol = malloc(sizeof (PlaceId) * 13);
    
    patrol[0] = KLAUSENBURG;
    patrol[1] = BUDAPEST;
    patrol[2] = VIENNA;
    patrol[3] = ZAGREB;
    patrol[4] = SARAJEVO;
    patrol[5] = VALONA;
    patrol[6] = SALONICA;
    patrol[7] = SOFIA;
    patrol[8] = VARNA;
    patrol[9] = CONSTANTA;
    patrol[10] = GALATZ;
    patrol[11] = BUCHAREST;
    patrol[12] = KLAUSENBURG;

    return patrol;
}

PlaceId *storeBluePatrol () {

    PlaceId *patrol = malloc(sizeof (PlaceId) * 14);
    
    patrol[0] = AMSTERDAM;
    patrol[1] = BRUSSELS;
    patrol[2] = PARIS;
    patrol[3] = GENEVA;
    patrol[4] = ZURICH;
    patrol[5] = MILAN;
    patrol[6] = VENICE;
    patrol[7] = MUNICH;
    patrol[8] = VIENNA;
    patrol[9] = PRAGUE;
    patrol[10] = BERLIN;
    patrol[11] = HAMBURG;     
    patrol[12] = COLOGNE;
    patrol[13] = AMSTERDAM;

    return patrol;
}

PlaceId *storeGreenPatrol () {

    PlaceId *patrol = malloc(sizeof (PlaceId) * 13);
    
    patrol[0] = MARSEILLES;
    patrol[1] = GENOA;
    patrol[2] = VENICE;
    patrol[3] = MUNICH;
    patrol[4] = NUREMBURG;
    patrol[5] = FRANKFURT;
    patrol[6] = COLOGNE;
    patrol[7] = BRUSSELS;
    patrol[8] = LE_HAVRE;
    patrol[9] = NANTES;
    patrol[10] = BORDEAUX;
    patrol[11] = TOULOUSE;
    patrol[12] = MARSEILLES; 

    return patrol;
}

PlaceId *storePinkPatrol () {

    PlaceId *patrol = malloc(sizeof (PlaceId) * 7);
    
    patrol[0] = LISBON;
    patrol[1] = CADIZ;
    patrol[2] = GRANADA;
    patrol[3] = ALICANTE;
    patrol[4] = SARAGOSSA;
    patrol[5] = SANTANDER;
    patrol[6] = LISBON;

    return patrol;
}
int inZone (HunterView hv, Player player) {
    
    int check = 0;
    int *round = malloc(sizeof(int));
    
    PlaceId playerLoc = NOWHERE;
    if (player == PLAYER_DRACULA) {
        playerLoc = HvGetLastKnownDraculaLocation(hv, round);
    } else {
        playerLoc = HvGetPlayerLocation (hv, player);
    }

    if (searchStorePatrol (storeRedPatrol(),   playerLoc) != -1) check = 1;
    else if (searchStorePatrol (storeBluePatrol(),  playerLoc) != -1) check = 2;
    else if (searchStorePatrol (storeGreenPatrol(), playerLoc) != -1) check = 3;
    else if (searchStorePatrol (storePinkPatrol(),  playerLoc) != -1) check = 4;

    free(round);            
    return check;
}
int searchStorePatrol (PlaceId *patrol, PlaceId location) {
    //use the fact that the last thing in the array is equal to the first thing
    int i = 0;
    int check = -1;

    if (patrol[i] == location) {
        return i;
    }
    i ++;
    
    while (patrol[0] != patrol[i]) {
        if (patrol[i] == location) {
            return i;
        }
        i++;
    }

    return check;
}

void patrol (HunterView hv, Player hunter, PlaceId *path)
{	
	int j = 1;

    while (path[0] != path[j]) {
        j++;
    }

	if ((hunter == PLAYER_LORD_GODALMING) || (hunter == PLAYER_VAN_HELSING)) {

		int i = 0;
		PlaceId first = NOWHERE;
		while (path[i] != first) {
			if (HvGetPlayerLocation(hv, hunter) == path[i]) {
				char *nextlocation = placeIdToAbbrev(path[i + 1]);
				registerBestPlay(nextlocation, "Looking for him");
				free(path);
				return;
			}
			first = path[0];  
			i ++;
		}
	
	} else {

		PlaceId first = NOWHERE;
		while (path[j] != first) {
			if (HvGetPlayerLocation(hv, hunter) == path[j]) {
				char *nextlocation = placeIdToAbbrev(path[j - 1]);
				registerBestPlay(nextlocation, "Looking for him");
				free(path);
				return;
			}
			first = path[0];  
			j --;
		}
	}
}

void patrolCurrent (HunterView hv, Player hunter)
{   
    if (inZone(hv, hunter) == 1) {
        patrol (hv, hunter, storeRedPatrol());
		return;
    } 
	if (inZone(hv, hunter) == 2) {
        patrol (hv, hunter, storeBluePatrol());     
 		return;
    }
	if (inZone(hv, hunter) == 3) {
        patrol (hv, hunter, storeGreenPatrol());
		return;
    }
	if (inZone(hv, hunter) == 4) {
        patrol (hv, hunter, storePinkPatrol());
		return;
	}
}
void goToDrac (HunterView hv, Player hunter)
{
	Round *round = malloc(sizeof (int));
	PlaceId destDrac = HvGetLastKnownDraculaLocation (hv, round);

    if (inZone(hv, PLAYER_DRACULA) == 1) {
        destDrac = VARNA;
    } else if (inZone(hv, PLAYER_DRACULA) == 2) {
        destDrac = PRAGUE;
    } else if (inZone(hv, PLAYER_DRACULA) == 3) {
        destDrac = GENOA;
    } else if (inZone(hv, PLAYER_DRACULA) == 4) {
        destDrac = ALICANTE;
    }

	if (inZone(hv, PLAYER_DRACULA) == inZone(hv, hunter)) {
	//if (inZone(hv, PLAYER_DRACULA) == inZone(hv, hunter)) {
		patrolCurrent (hv, hunter);
		return;
	}

    int *pathLength = malloc(sizeof(int));
    PlaceId *shortestPath = malloc(sizeof(PlaceId));
    
    shortestPath = HvGetShortestPathTo(hv, hunter, destDrac, pathLength);
    PlaceId nextLoc = shortestPath[0];
    
    registerBestPlay(placeIdToAbbrev(nextLoc), "Rush to the castle!");  

    free(pathLength);
    free(shortestPath);
}


void decideHunterMove(HunterView hv)
{   
    Round round = HvGetRound(hv);
    Player player = HvGetPlayer(hv);
   
    if (player == PLAYER_DRACULA) return;

    Round *roundLastseen = malloc(sizeof(Round));
    PlaceId lastSeenDrac = HvGetLastKnownDraculaLocation(hv, roundLastseen);
    PlaceId currDracLocation = HvGetPlayerLocation(hv, PLAYER_DRACULA);
    
    if (round == 0) {
        switch (player) {
            case PLAYER_LORD_GODALMING:
                registerBestPlay(placeIdToAbbrev(KLAUSENBURG), "Godalming to rescue");
                return;
            case PLAYER_DR_SEWARD:
               registerBestPlay(placeIdToAbbrev(MARSEILLES), "Time to go SewWards");
                return;
            case PLAYER_VAN_HELSING:
                registerBestPlay(placeIdToAbbrev(LISBON), "See you in HEL dracula");
                return;
            case PLAYER_MINA_HARKER:
                registerBestPlay(placeIdToAbbrev(AMSTERDAM), "Down to hunt!");
                return;
            case PLAYER_DRACULA:
                return;
        }
    }
	 
    if (HvGetPlayerLocation(hv, player) == ST_JOSEPH_AND_ST_MARY) {
        registerBestPlay("SJ", "OopS");
        return;
    }

    //If hunter is in the same location as dracula and does not have  enough health
    //To survive a trap and dracula himself
    //He must go to an adjacent city
    if (HvGetPlayerLocation(hv, player) == currDracLocation && HvGetHealth(hv, player) < 7) {
        MoveToConnection(hv, player);
        return;
    }


    //Rest a round if health is too low 
    if (HvGetHealth(hv, player) <= 4) {
        
        registerBestPlay(placeIdToAbbrev(HvGetPlayerLocation(hv, player)), 
                         "Health is too low, have to rest");
        return;
    }
	
    if ((round - *roundLastseen > 8) && (lastSeenDrac != NOWHERE)) {
        
        registerBestPlay(placeIdToAbbrev(HvGetPlayerLocation(hv, player)), 
                         "Rest y'all, we gotta find the blood sucking villain");
        return;
    }
	
    //If Dracula has not been seen yet
    if (lastSeenDrac == NOWHERE) {
        
        patrolCurrent (hv, player);
		return;
    }

    if (lastSeenDrac != NOWHERE) {
		if (inZone (hv, PLAYER_DRACULA) != 0) {
			goToDrac (hv, player);
			return;
		} else {
			
		}
    }

    //If dracula was last seen at Castle Dracula, 
    //The hunters will advance to CD
    if (lastSeenDrac == CASTLE_DRACULA) {
    
        PlaceId *shortestPath = malloc(sizeof(PlaceId)); //does this malloc need to be bigger
        int *pathLength = malloc(sizeof(int));


        if(HvGetPlayerLocation(hv, player) == CASTLE_DRACULA) {
            if(HvGetHealth(hv, player) < 4) {
            MoveToConnection(hv, player);
            return;
            }

            registerBestPlay(placeIdToAbbrev(CASTLE_DRACULA), "Staying at castle!");
            return;
        }
        
        shortestPath = ifDracAtCD(hv, player, CASTLE_DRACULA, round, pathLength);
    
        
        PlaceId nextLoc = shortestPath[0];
        char *nextlocation = placeIdToAbbrev(nextLoc);

        registerBestPlay(nextlocation, "Rush to the castle!");

        free(shortestPath);
        free(pathLength);
        
        return;
    }
    ////CHECK THE IMPLEMENTATION BEHIND THIS..////

    //If the hunter and dracula have the same location
    //The hunter will move to an adjacent location of it.
    
    //PossiblePlaces is an array that consists of the adjacent locations
    //Which are possible places
    int *numRetLocs = malloc(sizeof(int *));
    if (currDracLocation == HvGetPlayerLocation(hv, HvGetPlayer(hv))) {
        LookForDrac(hv, lastSeenDrac, numRetLocs, player );
        return;
    }

    ////////............................////////
    MoveToConnection(hv, player);
}

//If Dracula moves to CD by a teleport move.
//All Hunters will make their move to CD
PlaceId *ifDracAtCD(HunterView hv, Player hunter, PlaceId dest, Round round, int *pathLength)
{   
    PlaceId *shortestPath = HvGetShortestPathTo(hv, hunter, dest, pathLength);
    return shortestPath;
}

//Back up case in case no condition is satisfied.
void MoveToConnection(HunterView hv, Player hunter) {

    int *moves = malloc(sizeof(int *));
    PlaceId *whereto = HvWhereCanIGo(hv, moves);

    int i = rand()% *moves; 
    registerBestPlay(placeIdToAbbrev(whereto[i]), "Gotta move Gotta move Gotta MOVE");

}

//If Dracula lands in the same location as a hunter
//Each of the hunter should Converge into Dracula's position
//Should each hunter take n steps away?
//Try to go to the reachable locations from Drac's known location
//Return an array of possible locations.
void LookForDrac(HunterView hv, PlaceId lastSeenDrac, int *pathLength, Player hunter) {

    
    //If dracula's last known position is the same as that of the
    //current hunter, The hunter should try to get to one of the connected locations
    //Connected by road and sea only)
    //The current hunter 
    //If the Drac's last known position is the same as the hunter's

    //Add in a condition to check Dracula's health.
    //If his health is too low, he might avoid the sea
        //Necessary?

        PlaceId *possibleLocations = HvWhereCanIGoByType(hv, true, false,true, pathLength);

    if (HvGetPlayerLocation(hv, hunter) == lastSeenDrac) {
        PlaceId newlocation = possibleLocations[0];
        registerBestPlay(placeIdToAbbrev(newlocation), "Found Drac, moving to another position to track");
        return;
    }
    
    PlaceId *shortestPath = HvGetShortestPathTo(hv, hunter, possibleLocations[1], pathLength);

    for (int i = 0; i < *pathLength; i++) {
        PlaceId newlocation = shortestPath[0];
        registerBestPlay(placeIdToAbbrev(newlocation), "Found Drac, moving to another position to track");
        break;
    }


    return;
}
