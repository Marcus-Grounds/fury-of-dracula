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
void patrol (HunterView hv, Player hunter);
PlaceId *ifDracAtCD (HunterView hv, Player hunter, PlaceId dest, Round round, int *pathLength);
//                            //

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
	//IF dracula was last seen at Castle Dracula, 
	//The hunters will advance to CD
	
	if (lastSeenDrac == CASTLE_DRACULA) {
		
		PlaceId *shortestPath = malloc(sizeof(PlaceId)); 
		int *pathLength = malloc(sizeof(int));
		
		shortestPath = ifDracAtCD(hv, player, CASTLE_DRACULA, round, pathLength);
		
		PlaceId nextLoc = shortestPath[0];
		char *nextlocation = placeIdToAbbrev(nextLoc);

		registerBestPlay(nextlocation, "Rush to the castle!");

		free(shortestPath);
		free(pathLength);
		
		return;
	}

	//If Dracula has not been seen yet
	if ((lastSeenDrac == CITY_UNKNOWN || lastSeenDrac == SEA_UNKNOWN) &&  round % 6 != 0) {
		patrol(hv, player);
		return;
	}
}

void patrol(HunterView hv, Player hunter)
{
	if (hunter == PLAYER_LORD_GODALMING) {
		
		PlaceId zonePathGod[13];

		zonePathGod[0] = BUDAPEST;
		zonePathGod[1] = VIENNA;
		zonePathGod[2] = ZAGREB;
		zonePathGod[3] = SARAJEVO;
		zonePathGod[4] = VALONA;
		zonePathGod[5] = SALONICA;
		zonePathGod[6] = SOFIA;
		zonePathGod[7] = VARNA;
		zonePathGod[8] = CONSTANTA;
		zonePathGod[9] = GALATZ;
		zonePathGod[10] = BUCHAREST;
		zonePathGod[11] = KLAUSENBURG;
		zonePathGod[12] = BUDAPEST;  
		
		for (int i = 0; i < 12; i++) {
			if (HvGetPlayerLocation(hv, hunter) == zonePathGod[i]) {
				char *nextlocation = placeIdToAbbrev(zonePathGod[i + 1]);
				registerBestPlay(nextlocation, "Looking for him");
				break;
			}	
		}
	}
	if (hunter == PLAYER_DR_SEWARD) {

		PlaceId zonePathSew[13];

		zonePathSew[0] = MARSEILLES;
		zonePathSew[1] = GENOA;
		zonePathSew[2] = MILAN;
		zonePathSew[3] = MUNICH;
		zonePathSew[4] = VALONA;
		zonePathSew[5] = NUREMBURG;
		zonePathSew[6] = COLOGNE;
		zonePathSew[7] = BRUSSELS;
		zonePathSew[8] = LE_HAVRE;
		zonePathSew[9] = NANTES;
		zonePathSew[10] = BORDEAUX;
		zonePathSew[11] = TOULOUSE;
		zonePathSew[12] = MARSEILLES; 

		for (int i = 0; i < 12; i++) {
			if (HvGetPlayerLocation(hv, hunter) == zonePathSew[i]) {
				char *nextlocation = placeIdToAbbrev(zonePathSew[i + 1]);
				registerBestPlay(nextlocation, "Looking for him");
				break;
			}	
		}
	}
	if (hunter == PLAYER_VAN_HELSING) {	

		PlaceId zonePathVan[9];

		zonePathVan[0] = LISBON;
		zonePathVan[1] = CADIZ;
		zonePathVan[2] = GRANADA;
		zonePathVan[3] = ALICANTE;
		zonePathVan[4] = BARCELONA;
		zonePathVan[5] = TOULOUSE;
		zonePathVan[6] = SARAGOSSA;
		zonePathVan[7] = SANTANDER;
		zonePathVan[8] = LISBON;	  

		for (int i = 0; i < 8; i++) {
			if (HvGetPlayerLocation(hv, hunter) == zonePathVan[i]) {
				char *nextlocation = placeIdToAbbrev(zonePathVan[i + 1]);
				registerBestPlay(nextlocation, "Looking for him");
				break;
			}
		}
	}
	if (hunter == PLAYER_MINA_HARKER) {	
		
		PlaceId zonePathMin[11];

		zonePathMin[0] = AMSTERDAM;
		zonePathMin[1] = BRUSSELS;
		zonePathMin[2] = PARIS;
		zonePathMin[3] = GENEVA;
		zonePathMin[4] = ZURICH;
		zonePathMin[5] = MILAN;
		zonePathMin[6] = NUREMBURG;
		zonePathMin[7] = LEIPZIG;
		zonePathMin[8] = BERLIN;
		zonePathMin[9] = HAMBURG;
		zonePathMin[10] = AMSTERDAM;	  

		for (int i = 0; i < 10; i++) {
			if (HvGetPlayerLocation(hv, hunter) == zonePathMin[i]) {
				char *nextlocation = placeIdToAbbrev(zonePathMin[i + 1]);
				registerBestPlay(nextlocation, "Looking for him");
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