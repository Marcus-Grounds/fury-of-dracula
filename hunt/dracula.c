////////////////////////////////////////////////////////////////////////
// COMP2521 20T2 ... the Fury of Dracula
// dracula.c: your "Fury of Dracula" Dracula AI
//
// 2014-07-01	v1.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2017-12-01	v1.1	Team Dracula <cs2521@cse.unsw.edu.au>
// 2018-12-31	v2.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2020-07-10	v3.0	Team Dracula <cs2521@cse.unsw.edu.au>
//
////////////////////////////////////////////////////////////////////////

#include "dracula.h"
#include "DraculaView.h"
#include "Game.h"

#define DRAC_HALF_HEALTH GAME_START_BLOOD_POINTS/2

// Helper function declarations:
bool locationInArray(PlaceId location, PlaceId *array, int numArray)
char *dracLocToMoveAbbrev(DraculaView dv, PlaceId loc);
void lowHealthMove(DraculaView dv);

void decideDraculaMove(DraculaView dv)
{
	
	int numValid = 0;
	PlaceId *validMoves = DvGetValidMoves(dv, &numValid);
	if (numValid == 0) {
		registerBestPlay("TP", "");
		free(validMoves);
		return;
	}

	if (DvGetHealth(dv, PLAYER_DRACULA) <= DRAC_HALF_HEALTH) {
		lowHealthMove(dv);
		return;
	}

	registerBestPlay("CD", "Mwahahahaha");
}

// Returns true if the location exists in the array, and false if
// it doesn't
bool locationInArray(PlaceId location, PlaceId *array, int numArray) {
	for (int i = 0; i < numArray; i++) {
		if (array[i] == location) return true;
	}
	return false;
}

// This func assumes that Dracula has valid moves other than TP
void lowHealthMove(DraculaView dv) {
	// Finding moves which avoid potential encounters 
	// with hunters in the next round

	PlaceId *locAvoidsHunt = NULL;
	int numAvoidsHunt = 0;

	int numDracReachable = 0;
	PlaceId *dracReachable = DvWhereCanIGo(dv, &numDracReachable);

	for (Player p = PLAYER_LORD_GODALMING; 
		 p <= PLAYER_MINA_HARKER; p++) {
		int numHuntReachable = 0;
		PlaceId *huntReachable = DvWhereCanTheyGo(dv, p, &numHuntReachable);
		
		for (int i = 0; i < numDracReachable; i++) {
			if (!locationInArray(dracReachable[i], huntReachable, 
								numHuntReachable)) {
				locAvoidsHunt[numAvoidsHunt] = dracReachable[i];
				numAvoidsHunt++;
			}
		}
		free(huntReachable);
	}

	// Choosing a location for Dracula's move
	PlaceId locChosen = NOWHERE;
	if (numAvoidsHunt == 0) {
		// Next move cannot avoid hunters' reachable locations
		// play random valid move? TODO: consider best total dist
		locChosen = dracReachable[0];
	} else {
		// Play random valid move that avoids the sea
		for (int i = 0; i < numAvoidsHunt; i++) {
			if (placeIsLand(locAvoidsHunt[i])) {
				// TODO: consider best total dist
				locChosen = locAvoidsHunt[i];
				break;
			}
		} 
		if (locChosen == NOWHERE) {
			// No move which avoids the sea
			// Play random sea move which avoids hunters? 
			// TODO: consider best total dist 
			locChosen = locAvoidsHunt[0];
		}
	}
	free(dracReachable);
	registerBestPlay(dracLocToMoveAbbrev(dv, locChosen), "");
	return;
}

char *dracLocToMoveAbbrev(DraculaView dv, PlaceId loc) {
	// Assumes that the location given is reachable by Dracula

	char *abbrev = NULL;
	int numValid = 0;
	PlaceId *validMoves = DvGetValidMoves(dv, &numValid);

	int numDracHist = 0; 
	bool histCanFree = false;
	PlaceId *dracLocHist = DvGetLocationHistory(dv, &numDracHist, &histCanFree);

	for (int i = 0; i < numValid; i++) {
		if (validMoves[i] == loc) {
			abbrev = placeIdToAbbrev(validMoves[i]);
			break;
		}
		if ((validMoves[i] == HIDE) && 
			(DvGetPlayerLocation(dv, PLAYER_DRACULA) == loc)) {
			abbrev = placeIdToAbbrev(validMoves[i]);
			break;
		}

		if ((validMoves[i] >= DOUBLE_BACK_1) && 
			(validMoves[i] <= DOUBLE_BACK_5)) {
			PlaceId doubleBackLoc = dracLocHist[numDracHist - 1 - 
									(validMoves[i] - DOUBLE_BACK_1)];
			if (doubleBackLoc == loc) {
				abbrev = placeIdToAbbrev(validMoves[i]);
				break;
			}
		}
	}
	free(validMoves);
	if (histCanFree) free(dracLocHist);
	return abbrev;
}