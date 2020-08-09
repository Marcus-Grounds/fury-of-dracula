////////////////////////////////////////////////////////////////////////
// COMP2521 20T2 ... the Fury of Dracula
// hunter.c: your "Fury of Dracula" hunter AI.
//
// 2014-07-01	v1.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2017-12-01	v1.1	Team Dracula <cs2521@cse.unsw.edu.au>
// 2018-12-31	v2.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2020-07-10	v3.0	Team Dracula <cs2521@cse.unsw.edu.au>
//
////////////////////////////////////////////////////////////////////////

#include "Game.h"
#include "hunter.h"
#include "HunterView.h"

//REMOVE TIS:
#include <stdio.h>
#include <stdlib.h>

//HELPER FUNCTIONS DECLARATION//
PlaceId *storeRedPatrol ();
PlaceId *storeBluePatrol ();
PlaceId *storeGreenPatrol ();
PlaceId *storePinkPatrol ();

void patrolInitialise (HunterView hv, Player hunter);
int searchStorePatrol (PlaceId *patrol, PlaceId location);
void goToandPatrol (HunterView hv, Player hunter, PlaceId *storePatrol);

void playerPositioningStage1 (HunterView hv, Player hunter);
void playerPositioningStage2 (HunterView hv, Player hunter);

PlaceId *ifDracAtCD (HunterView hv, Player hunter, PlaceId dest, Round round, int *pathLength);

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
	patrol[10]  = BERLIN;
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
	}

	return check;
}

void patrolInitialise (HunterView hv, Player hunter) {
	
	if (hunter == PLAYER_LORD_GODALMING) {
		goToandPatrol (hv, hunter, storeRedPatrol());
	}
	if (hunter == PLAYER_DR_SEWARD) {
		goToandPatrol  (hv, hunter, storeBluePatrol());
	}
	if (hunter == PLAYER_VAN_HELSING) {
		goToandPatrol (hv, hunter, storeGreenPatrol());
	}
	if (hunter == PLAYER_MINA_HARKER) {
		goToandPatrol  (hv, hunter, storePinkPatrol());
	}
}

void goToandPatrol (HunterView hv, Player hunter, PlaceId *storePatrol)
{	
	int i = 0;
	int check = 1;
	PlaceId first = NOWHERE;

	PlaceId *patrol = storePatrol;
	int *pathLength = malloc(sizeof(int));
	PlaceId *shortestPath = malloc(sizeof(PlaceId));
	
	while (patrol[i] != first) {
		if (HvGetPlayerLocation(hv, hunter) == patrol[i]) {
			char *nextlocation = placeIdToAbbrev(patrol[i + 1]);
			registerBestPlay(nextlocation, "Looking for him");
			free(patrol);
			check = 0;
			break;
		}
		first = patrol[0];	
		i ++;
	}

	if (check != 0) {
		shortestPath = HvGetShortestPathTo(hv, hunter, patrol[0], pathLength);
		PlaceId nextLoc = shortestPath[0];
		registerBestPlay(placeIdToAbbrev(nextLoc), "Rush to the castle!");	
	}
	free(pathLength);
	free(shortestPath);
}


void decideHunterMove(HunterView hv)
{	
	Player player = HvGetPlayer(hv);
	
	if (player == PLAYER_DRACULA) return;
	
	Round round = HvGetRound(hv);
	Round *roundLastseen = malloc(sizeof(Round));
	PlaceId lastSeenDrac = HvGetLastKnownDraculaLocation(hv, roundLastseen);

	if (round == 0) {
		switch (player) {
            case PLAYER_LORD_GODALMING:
                registerBestPlay(placeIdToAbbrev(KLAUSENBURG), "Godalming to rescue");
                return;
            case PLAYER_DR_SEWARD:
               registerBestPlay(placeIdToAbbrev(MARSEILLES), "Time to go SewWards");
                return;
            case PLAYER_VAN_HELSING:
                registerBestPlay(placeIdToAbbrev(AMSTERDAM), "See you in HEL dracula");
                return;
            case PLAYER_MINA_HARKER:
                registerBestPlay(placeIdToAbbrev(LISBON), "Down to hunt!");
                return;
			case PLAYER_DRACULA:
				return;
        }
	}

	//Rest a round if health is too low 
	if (HvGetHealth(hv, player) <= 2) {
		registerBestPlay(placeIdToAbbrev(HvGetPlayerLocation(hv, player)), 
						 "Health is too low, have to rest");
		return;
	}

	if ((lastSeenDrac == NOWHERE) && round % 7 == 0 ) {
		
		registerBestPlay(placeIdToAbbrev(HvGetPlayerLocation(hv, player)), 
						 "Rest y'all, we gotta find the blood sucking villain");
		return;
	}
	
	//If Dracula has not been seen yet
	if (lastSeenDrac == NOWHERE) {
		
		patrolInitialise (hv, player);
		return;
	}

	//If Dracula has been see, except his last known location is too far behind his current loc
	if (lastSeenDrac != NOWHERE) {
		
		if ((round - *roundLastseen) > 4) {
			patrolInitialise (hv, player); 
		
		} else if (((round - *roundLastseen) < 5) && ((round - *roundLastseen) > 2)) {
			playerPositioningStage1(hv, player);
			
		} else if ((round - *roundLastseen) < 3) {
			playerPositioningStage2(hv, player); //randomise moves in this
		}
	}

	//IF dracula was last seen at Castle Dracula, 
	//The hunters will advance to CD
	//ADD THIS CONDITION IN PINK ROTATE
	if (lastSeenDrac == CASTLE_DRACULA) {
		
		PlaceId *shortestPath = malloc(sizeof(PlaceId)); //does this malloc need to be bigger
		int *pathLength = malloc(sizeof(int));
		
		shortestPath = ifDracAtCD(hv, player, CASTLE_DRACULA, round, pathLength);
		
		PlaceId nextLoc = shortestPath[0];
		char *nextlocation = placeIdToAbbrev(nextLoc);

		registerBestPlay(nextlocation, "Rush to the castle!");

		free(shortestPath);
		free(pathLength);
		
		return;
	}
}

//If Dracula moves to CD by a teleport move.
//All Hunters will make their move to CD
PlaceId *ifDracAtCD(HunterView hv, Player hunter, PlaceId dest, Round round, int *pathLength)
{	
	PlaceId *shortestPath = HvGetShortestPathTo(hv, hunter, dest, pathLength);
	return shortestPath;
}

//STRATEGY FOR IF HUNTERS FALL IN TRAIL DURING PATROL
void playerPositioningStage1 (HunterView hv, Player hunter)
{
	//NEED TO SEE IN WHICH PATROL ZONE WAS LOCATED
	//FOUND IN RED - PINK MOVES TO RED
	//FOUND IN PINK - RED MOVES TO PINK
	//FOUND IN BLUE/GREEN - RED MOVES TO BLUE - PINK MOVES TO GREEN
	//IF IS ALREADY IN THAT SPOT - FOLLOW CORRESPONING PATROL ZONES

	Round *round = malloc(sizeof(int));
	int *pathLength = malloc(sizeof(int));
	PlaceId *shortestPath = malloc(sizeof(PlaceId)); //does this malloc need to be bigger
	PlaceId draculaLoc = (HvGetLastKnownDraculaLocation(hv, round));
	PlaceId hunterLoc  = (HvGetPlayerLocation(hv, hunter));

	if (draculaLoc == NOWHERE) return;

	if (searchStorePatrol(storeRedPatrol(), draculaLoc) != -1) {
				
		if (hunter == PLAYER_MINA_HARKER) {
			
			if ((hunterLoc == VARNA) || 
				(searchStorePatrol(storeRedPatrol(), hunterLoc) != -1)) {
				
				goToandPatrol (hv, hunter, storeRedPatrol());
			
			} else {

				shortestPath = HvGetShortestPathTo(hv, hunter, VARNA, pathLength);
				PlaceId nextLoc = shortestPath[0];
				registerBestPlay(placeIdToAbbrev(nextLoc), "Rush to the castle!");
			}
			return;	
		}
	}
	
	if (searchStorePatrol(storePinkPatrol(), draculaLoc) != -1) {
		
		if (hunter == PLAYER_LORD_GODALMING) {

			if ((hunterLoc == ALICANTE) || 
				(searchStorePatrol(storePinkPatrol(), hunterLoc) != -1)) {
				
				goToandPatrol (hv, hunter, storePinkPatrol());
			
			} else {
			
				shortestPath = HvGetShortestPathTo(hv, hunter, ALICANTE, pathLength);
				PlaceId nextLoc = shortestPath[0];
				registerBestPlay(placeIdToAbbrev(nextLoc), "Rush to the castle!");
			}
			return;	
		}
	}

	if ((searchStorePatrol(storeBluePatrol(), draculaLoc) != -1) || 
		(searchStorePatrol(storeGreenPatrol(), draculaLoc) != -1))  {
		
		if (hunter == PLAYER_LORD_GODALMING) {
			
			if ((hunterLoc == VIENNA) || 
				(searchStorePatrol(storeBluePatrol(), hunterLoc) != -1)) {
				
				goToandPatrol (hv, hunter, storeBluePatrol());
			
			} else {

				shortestPath = HvGetShortestPathTo(hv, hunter, VIENNA, pathLength);
				PlaceId nextLoc = shortestPath[0];
				registerBestPlay(placeIdToAbbrev(nextLoc), "Rush to the castle!");
			}
			
			return;	
		}

		if (hunter == PLAYER_MINA_HARKER) {

			if ((hunterLoc == TOULOUSE) || 
				(searchStorePatrol(storeGreenPatrol(), hunterLoc) != -1)) {
				
				goToandPatrol (hv, hunter, storeGreenPatrol()); //random moves 
			
			} else {

				shortestPath = HvGetShortestPathTo(hv, hunter, TOULOUSE, pathLength);
				PlaceId nextLoc = shortestPath[0];
				registerBestPlay(placeIdToAbbrev(nextLoc), "Rush to the castle!");
			}

			return;	
		}	
	}
	
	free(round);
	free(shortestPath);
	free(pathLength);
}

void playerPositioningStage2 (HunterView hv, Player hunter)
{
	//ALL PLAYERS MOVE TO A SINGLE PATROL ZONE

	Round *round = malloc(sizeof(int));
	int *pathLength = malloc(sizeof(int));
	PlaceId *shortestPath = malloc(sizeof(PlaceId)); //does this malloc need to be bigger
	PlaceId draculaLoc = (HvGetLastKnownDraculaLocation(hv, round));
	PlaceId hunterLoc  = (HvGetPlayerLocation(hv, hunter));

	if (draculaLoc == NOWHERE) return;

	if (searchStorePatrol(storeRedPatrol(), draculaLoc) != -1) {
				
		if (hunter != PLAYER_LORD_GODALMING) {
			
			if ((hunterLoc == VARNA) || 
				(searchStorePatrol(storeRedPatrol(), hunterLoc) != -1)) {
				
				goToandPatrol (hv, hunter, storeRedPatrol());
			
			} else {

				shortestPath = HvGetShortestPathTo(hv, hunter, VARNA, pathLength);
				PlaceId nextLoc = shortestPath[0];
				registerBestPlay(placeIdToAbbrev(nextLoc), "Rush to red!");
			}
			return;	
		}
	}
	
	if (searchStorePatrol(storePinkPatrol(), draculaLoc) != -1) {
		
		if (hunter != PLAYER_MINA_HARKER) {

			if ((hunterLoc == ALICANTE) || 
				(searchStorePatrol(storePinkPatrol(), hunterLoc) != -1)) {
				
				goToandPatrol (hv, hunter, storePinkPatrol());
			
			} else {
			
				shortestPath = HvGetShortestPathTo(hv, hunter, ALICANTE, pathLength);
				PlaceId nextLoc = shortestPath[0];
				registerBestPlay(placeIdToAbbrev(nextLoc), "Rush to the castle!");
			}
			return;	
		}
	}

	if (searchStorePatrol(storeBluePatrol(), draculaLoc) != -1) {
		
		if (hunter != PLAYER_DR_SEWARD) {
			
			if ((hunterLoc == VIENNA) || 
				(searchStorePatrol(storeBluePatrol(), hunterLoc) != -1)) {
				
				goToandPatrol (hv, hunter, storeBluePatrol());
			
			} else {

				shortestPath = HvGetShortestPathTo(hv, hunter, VIENNA, pathLength);
				PlaceId nextLoc = shortestPath[0];
				registerBestPlay(placeIdToAbbrev(nextLoc), "Rush to the castle!");
			}
			
			return;	
		}
	}

	if (searchStorePatrol(storeGreenPatrol(), draculaLoc) != -1)  {
		
		if (hunter != PLAYER_VAN_HELSING) {

			if ((hunterLoc == TOULOUSE) || 
				(searchStorePatrol(storeGreenPatrol(), hunterLoc) != -1)) {
				
				goToandPatrol (hv, hunter, storeGreenPatrol());
			
			} else {

				shortestPath = HvGetShortestPathTo(hv, hunter, TOULOUSE, pathLength);
				PlaceId nextLoc = shortestPath[0];
				registerBestPlay(placeIdToAbbrev(nextLoc), "Rush to the castle!");
			}

			return;	
		}	
	}
	
	free(round);
	free(shortestPath);
	free(pathLength);
}