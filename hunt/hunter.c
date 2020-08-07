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
#include "Map.h"


//REMOVE TIS:
#include <stdio.h>
#include <stdlib.h>

//HELPER FUNCTIONS DECLARATION//
PlaceId *redPatrol ();
PlaceId *greenPatrol ();
PlaceId *pinkPatrol ();
PlaceId *bluePatrol ();

void patrol (HunterView hv, Player hunter);
//void preSixPositioning (HunterView hv, Player hunter);
PlaceId *ifDracAtCD (HunterView hv, Player hunter, PlaceId dest, Round round, int *pathLength);

//STORE PATROL ZONES
PlaceId *redPatrol () {

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

PlaceId *greenPatrol () {

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

PlaceId *pinkPatrol () {

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

PlaceId *bluePatrol () {

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

void decideHunterMove(HunterView hv)
{	


	Player player = HvGetPlayer(hv);
	
	if (player == PLAYER_DRACULA) return;

	//To keep track of drack encounters?
	//int *dracEncounterCount = malloc(sizeof(int *));

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
                registerBestPlay(placeIdToAbbrev(LISBON), "See you in HEL dracula");
                return;
            case PLAYER_MINA_HARKER:
                registerBestPlay(placeIdToAbbrev(AMSTERDAM), "Down to hunt!");
                return;
			case PLAYER_DRACULA:
				return;
        }
	}
	//If Dracula's last known position is unknown, 
	//The hunters would rest to find the location 
	
	if ((lastSeenDrac == CITY_UNKNOWN || lastSeenDrac == SEA_UNKNOWN) && round % 7 == 0 ) {
		registerBestPlay(placeIdToAbbrev(HvGetPlayerLocation(hv, player)), 
						 "Rest y'all, we gotta find the blood sucking villain");
		return;
	}

	//Rest a round if health is too low 
	if (HvGetHealth(hv, player) <= 2) {
		registerBestPlay(placeIdToAbbrev(HvGetPlayerLocation(hv, player)), 
						 "Health is too low, have to rest");
		return;
	}
	//If Dracula has not been seen yet
	if ((lastSeenDrac == CITY_UNKNOWN || lastSeenDrac == SEA_UNKNOWN) &&
		 round % 6 != 0) {
		
		patrol(hv, player);
		return;
	}
	
	//IF dracula was last seen at Castle Dracula, 
	//The hunters will advance to CD
	
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

//Setting up a patrol zonme for the hunters to start their round with
void patrol(HunterView hv, Player hunter)
{
	if (hunter == PLAYER_LORD_GODALMING) {

		PlaceId *patrol = redPatrol();
		for (int i = 0; i < 12; i++) {
			if (HvGetPlayerLocation(hv, hunter) == patrol[i]) {
				char *nextlocation = placeIdToAbbrev(patrol[i + 1]);
				registerBestPlay(nextlocation, "Looking for him");
				free(patrol);
				break;
			}	
		}
	}
	if (hunter == PLAYER_DR_SEWARD) {

		PlaceId *patrol = greenPatrol();
		for (int i = 0; i < 12; i++) {
			if (HvGetPlayerLocation(hv, hunter) == patrol[i]) {
				char *nextlocation = placeIdToAbbrev(patrol[i + 1]);
				registerBestPlay(nextlocation, "Looking for him");
				free(patrol);
				break;
			}	
		}
	}
	if (hunter == PLAYER_VAN_HELSING) {	
  
		PlaceId *patrol = pinkPatrol();
		for (int i = 0; i < 6; i++) {
			if (HvGetPlayerLocation(hv, hunter) == patrol[i]) {
				char *nextlocation = placeIdToAbbrev(patrol[i + 1]);
				registerBestPlay(nextlocation, "Looking for him");
				free(patrol);				
				break;
			}
		}
	}
	if (hunter == PLAYER_MINA_HARKER) {	

		PlaceId *patrol = bluePatrol();
		for (int i = 0; i < 13; i++) {
			if (HvGetPlayerLocation(hv, hunter) == patrol[i]) {
				char *nextlocation = placeIdToAbbrev(patrol[i + 1]);
				registerBestPlay(nextlocation, "Looking for him");
				free(patrol);
				break;
			}
		}
	}
}

//If Dracula moves to CD by a teleport move.
//All Hunters will make their move to CD
PlaceId *ifDracAtCD(HunterView hv, Player hunter, PlaceId dest, Round round, int *pathLength)
{	
	PlaceId *shortestPath = HvGetShortestPathTo(hv, hunter, dest, pathLength);
	return shortestPath;
}


//If Dracula lands in the same location as a hunter
//Each of the hunter should Converge into Dracula's position

//Should each hunter take n steps away?
//Try to go to the reachable locations from Drac's known location?
void ReachDrac(HunterView hv, PlaceId dest, PlaceId lastSeenDrac) {

	Player hunter = HvGetPlayer(hv);
	PlaceId location = HvGetPlayerLocation(hv, hunter);
	

	//If dracula's last known position is the same as that of the
	//current hunter, The hunter should try to get to one of the connected locations
	//Connected by road and sea only)

	//The current hunter 
	//If the Drac's last known position is the same as the hunter's
	if (lastSeenDrac == location) {
		PlaceId *numRetLocs = malloc(sizeof(PlaceId));

		//Add in a condition to check Dracula's health.
		//If his health is too low, he might avoid the sea
		//Necessary?

		PlaceId *possiblelocations = HvWhereCanTheyGoByType(hv, hunter, true, false,true, numRetLocs);
		PlaceId location = possiblelocations[0];
		registerBestPlay(placeIdToAbbrev(location), "Found Drac, moving to another position to track");
		return;
	}
	
	
}

