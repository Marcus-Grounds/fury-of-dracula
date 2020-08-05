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
PlaceId *ifDracAtCD(HunterView hv, Player hunter, PlaceId dest, Round round, int *pathLength);
void patrol(HunterView hv, Player hunter);



void decideHunterMove(HunterView hv)
{
	// TODO: Replace this with something better!
	Round round = HvGetRound(hv);
	Player player = HvGetPlayer(hv);
	Round *roundLastseen = malloc(sizeof(Round));
	PlaceId lastSeenDrac = HvGetLastKnownDraculaLocation(hv, roundLastseen);

	if (round == 0) {
		switch (player) {
            case PLAYER_LORD_GODALMING:
                registerBestPlay(placeIdToAbbrev(BUDAPEST), "Godalming to rescue");
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

	if (player == PLAYER_DRACULA) return;

	//If Dracula's last known position is unknown, 
	//The hunters would rest to find the location 
	if (lastSeenDrac == UNKNOWN && round % 6 == 0 ) {
		registerBestPlay(placeIdToAbbrev(HvGetPlayerLocation(hv, player)),
				"Rest y'all, we gotta find the blood sucking villain");
	}

	//Rest a round if health is too low 
	//TODO: Add conditions
	if (HvGetHealth(hv, player) <= 2) {
		registerBestPlay(placeIdToAbbrev(HvGetPlayerLocation(hv, player)), "Health is too low, have to rest");
	}


	//IF dracula was last seen at Castle Dracula, 
	//The hunters will advance to CD
	if (lastSeenDrac == CASTLE_DRACULA) {
		PlaceId *shortestPath = malloc(sizeof(PlaceId)); 
		int *pathLength = malloc(sizeof(int));
		shortestPath = ifDracAtCD(hv, player, CASTLE_DRACULA, round, pathLength);
		PlaceId nextLoc = 0;

		nextLoc = shortestPath[0];
		char *nextlocation = placeIdToAbbrev(nextLoc);

		registerBestPlay(nextlocation, "Rush to the castle!");
		
	}

	//If Dracula has not been seen yet
	if ((lastSeenDrac == CITY_UNKNOWN || lastSeenDrac == SEA_UNKNOWN) &&  round % 6 != 0) {
			patrol(hv, player);

	}
}


//If Dracula moves to CD by a teleport move.
//All Hunters will make their move to CD
PlaceId *ifDracAtCD(HunterView hv, Player hunter, PlaceId dest, Round round, int *pathLength) {
	
	PlaceId *shortestPath = HvGetShortestPathTo(hv, hunter, dest, pathLength);
	return shortestPath;
}

void patrol(HunterView hv, Player hunter) {
	//In case the hunter is Godalming
	if (hunter == PLAYER_LORD_GODALMING) {

		PlaceId *zonePath =  malloc(sizeof(PlaceId *));

		zonePath[0] = BUDAPEST;
		zonePath[1] = VIENNA;
		zonePath[2] = ZAGREB;
		zonePath[3] = SARAJEVO;
		zonePath[4] = VALONA;
		zonePath[5] = SALONICA;
		zonePath[6] = SOFIA;
		zonePath[7] = VARNA;
		zonePath[8] = CONSTANTA;
		zonePath[9] = GALATZ;
		zonePath[10] = BUCHAREST;
		zonePath[11] = KLAUSENBURG;
		zonePath[12] = BUDAPEST;  

		for (int i = 0; i < 13; i++) {
			if (HvGetPlayerLocation(hv, hunter) == zonePath[i]) {
				char *nextlocation = placeIdToAbbrev(zonePath[i]);
				registerBestPlay(nextlocation, "Looking for him");
				break;
			}
			
		}

	}

	//In case the hunter is Dr.Seward
	if (hunter == PLAYER_DR_SEWARD) {

		PlaceId *zonePath =  malloc(sizeof(PlaceId *));

		zonePath[0] = MARSEILLES;
		zonePath[1] = GENOA;
		zonePath[2] = MILAN;
		zonePath[3] = MUNICH;
		zonePath[4] = VALONA;
		zonePath[5] = NUREMBURG;
		zonePath[6] = COLOGNE;
		zonePath[7] = BRUSSELS;
		zonePath[8] = LE_HAVRE;
		zonePath[9] = NANTES;
		zonePath[10] = BORDEAUX;
		zonePath[11] = TOULOUSE;
		zonePath[12] = MARSEILLES;  

		for (int i = 0; i < 13; i++) {
			if (HvGetPlayerLocation(hv, hunter) == zonePath[i]) {
				char *nextlocation = placeIdToAbbrev(zonePath[i]);
				registerBestPlay(nextlocation, "Looking for him");
				break;
			}
			
		}

		free(zonePath);

	}

	//In case the hunter is Dr.Seward
	if (hunter == PLAYER_DR_SEWARD) {

		PlaceId *zonePath =  malloc(sizeof(PlaceId *));

		zonePath[0] = MARSEILLES;
		zonePath[1] = GENOA;
		zonePath[2] = MILAN;
		zonePath[3] = MUNICH;
		zonePath[4] = VALONA;
		zonePath[5] = NUREMBURG;
		zonePath[6] = COLOGNE;
		zonePath[7] = BRUSSELS;
		zonePath[8] = LE_HAVRE;
		zonePath[9] = NANTES;
		zonePath[10] = BORDEAUX;
		zonePath[11] = TOULOUSE;
		zonePath[12] = MARSEILLES;  

		for (int i = 0; i < 13; i++) {
			if (HvGetPlayerLocation(hv, hunter) == zonePath[i]) {
				char *nextlocation = placeIdToAbbrev(zonePath[i]);
				registerBestPlay(nextlocation, "Looking for him");
				break;
			}
			
		}

		free(zonePath);

	}


	//In case the hunter is Van Helsing
	if (hunter == PLAYER_VAN_HELSING) {

		PlaceId *zonePath =  malloc(sizeof(PlaceId *));

		zonePath[0] = LISBON;
		zonePath[1] = CADIZ;
		zonePath[2] = GRANADA;
		zonePath[3] = ALICANTE;
		zonePath[4] = BARCELONA;
		zonePath[5] = TOULOUSE;
		zonePath[6] = SARAGOSSA;
		zonePath[7] = SANTANDER;
		zonePath[8] = LISBON;
		  
		for (int i = 0; i < 13; i++) {
			if (HvGetPlayerLocation(hv, hunter) == zonePath[i]) {
				char *nextlocation = placeIdToAbbrev(zonePath[i]);
				registerBestPlay(nextlocation, "Looking for him");
				break;
			}
			
		}

		free(zonePath);

	}

	//In case the hunter is Mina Harker
	if (hunter == PLAYER_MINA_HARKER) {

		PlaceId *zonePath =  malloc(sizeof(PlaceId *));

		zonePath[0] = AMSTERDAM;
		zonePath[1] = BRUSSELS;
		zonePath[2] = PARIS;
		zonePath[3] = GENEVA;
		zonePath[4] = ZURICH;
		zonePath[5] = MILAN;
		zonePath[6] = NUREMBURG;
		zonePath[7] = LEIPZIG;
		zonePath[8] = BERLIN;
		zonePath[9] = HAMBURG;
		zonePath[10] = AMSTERDAM;
		  
		for (int i = 0; i < 13; i++) {
			if (HvGetPlayerLocation(hv, hunter) == zonePath[i]) {
				char *nextlocation = placeIdToAbbrev(zonePath[i]);
				registerBestPlay(nextlocation, "Looking for him");
				break;
			}
			
		}

		free(zonePath);

	}
	
}