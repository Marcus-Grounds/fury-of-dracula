////////////////////////////////////////////////////////////////////////
// COMP2521 20T2 ... the Fury of Dracula
// DraculaView.c: the DraculaView ADT implementation
//
// 2014-07-01	v1.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2017-12-01	v1.1	Team Dracula <cs2521@cse.unsw.edu.au>
// 2018-12-31	v2.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2020-07-10	v3.0	Team Dracula <cs2521@cse.unsw.edu.au>
//
////////////////////////////////////////////////////////////////////////

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "DraculaView.h"
#include "Game.h"
#include "GameView.h"
#include "Map.h"
// add your own #includes here
// TODO: ADD YOUR OWN STRUCTS HERE
#define isDoubleBack(move) (move == DOUBLE_BACK_1 || move == DOUBLE_BACK_2 || move == DOUBLE_BACK_3 || move == DOUBLE_BACK_4 || move == DOUBLE_BACK_5)
#define CAN_DRACULA_RAIL false 

struct draculaView {
	GameView gv;
};

////////////////////////////////////////////////////////////////////////
// Constructor/Destructor

DraculaView DvNew(char *pastPlays, Message messages[])
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	DraculaView new = malloc(sizeof(*new));
	if (new == NULL) {
		fprintf(stderr, "Couldn't allocate DraculaView\n");
		exit(EXIT_FAILURE);
	}

	new->gv = GvNew(pastPlays, messages);
	return new;
}

void DvFree(DraculaView dv)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	free(dv);
}

////////////////////////////////////////////////////////////////////////
// Game State Information

Round DvGetRound(DraculaView dv)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	return GvGetRound(dv->gv);
}

int DvGetScore(DraculaView dv)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	return 0;
}

int DvGetHealth(DraculaView dv, Player player)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	return 0;
}

PlaceId DvGetPlayerLocation(DraculaView dv, Player player)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	return GvGetPlayerLocation(dv->gv, player);
}

PlaceId DvGetVampireLocation(DraculaView dv)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	return NOWHERE;
}

PlaceId *DvGetTrapLocations(DraculaView dv, int *numTraps)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	*numTraps = 0;
	return NULL;
}

////////////////////////////////////////////////////////////////////////
// Making a Move
bool DvIsRepeat(PlaceId new, PlaceId *reachableLocations, int *numReturnedLocs) 
{
	for (int i = 0; i < (*numReturnedLocs); i++) {
		if (reachableLocations[i] == new) return true;
	}
	return false;
}

bool foundDoubleBack(PlaceId *validMoves, int *numValidMoves) 
{
	for (int i = 0; i < *numValidMoves; i++) {
		if (isDoubleBack(validMoves[i])) return true;
	}
	return false;
}

bool foundHide(PlaceId *validMoves, int *numValidMoves) 
{
	for (int i = 0; i < *numValidMoves; i++) {
		if (validMoves[i] == HIDE) return true;
	}
	return false;
}

PlaceId *DvAddPlaceId(PlaceId new, PlaceId *reachableLocations,
					int *numReturnedLocs) {
	if (DvIsRepeat(new, reachableLocations, numReturnedLocs)) return reachableLocations;
	(*numReturnedLocs)++;
	reachableLocations = realloc(reachableLocations, 
								 (*numReturnedLocs) * sizeof(PlaceId));
	assert(reachableLocations != NULL);
	reachableLocations[(*numReturnedLocs) - 1] = new;
	return reachableLocations;
}

PlaceId *removeLocation(PlaceId *reachableLocations, PlaceId whichLocation, int *numReturnedLocs) 
{
	int i = 0;
	// Look for location in array.
	for (int i = 0; i < *numReturnedLocs; i++) {
		if (reachableLocations[i] == whichLocation) break;
	}

	// Location was not found in array.
	if (i == *numReturnedLocs) return reachableLocations;

	// Remove location.
	(*numReturnedLocs)--;

	while (i < *numReturnedLocs) {
		reachableLocations[i] = reachableLocations[i + 1];
		i++;
	}

	// Reduce size of array.
	reachableLocations = realloc(reachableLocations, (*numReturnedLocs) * sizeof(PlaceId));

	return reachableLocations;
}

PlaceId *addRealPlaces(PlaceId *reachableLocations, PlaceId *validMoves,
					   int *numReturnedLocs, int *numValidMoves) 
{
	for (int i = 0; i < *numValidMoves; i++) {
		if (placeIsReal(validMoves[i])) {
			reachableLocations = DvAddPlaceId(validMoves[i], reachableLocations, 
											numReturnedLocs);
		}
	}
	return reachableLocations;
}

PlaceId *DvGetValidMoves(DraculaView dv, int *numReturnedMoves)
{
	int numTrail = 0; 
	bool trailCanFree = false;
	PlaceId *trail = GvGetLastMoves(dv->gv, PLAYER_DRACULA, TRAIL_SIZE, &numTrail, &trailCanFree);
	
	Round round = GvGetRound(dv->gv);
	PlaceId currLoc = GvGetPlayerLocation(dv->gv, PLAYER_DRACULA);
	int numReachable = 0;
	PlaceId *reachable = GvGetReachable(dv->gv, PLAYER_DRACULA, round, currLoc, &numReachable);

	int numHistory = 0; 
	bool historyCanFree = false;
	PlaceId *locHistory = GvGetLocationHistory(dv->gv, PLAYER_DRACULA, &numHistory, &historyCanFree);

	// Adding the reachable move into validMoves if it is not in the trail
	*numReturnedMoves = 0;
	PlaceId *validMoves = NULL;
	for (int i = 0; i < numReachable; i++) {
		if (DvIsRepeat(reachable[i], trail, &numTrail)) continue;

		(*numReturnedMoves)++;
		validMoves = realloc(validMoves, sizeof(PlaceId) * (*numReturnedMoves));
		assert (validMoves != NULL);
		validMoves[(*numReturnedMoves) - 1] = reachable[i];
	}

	// Checking if DOUBLE_BACK and HIDE moves have already occured in the trail
	bool doubleBackValid = true;
	bool hideValid = true;
	for (int i = 0; i < numTrail; i++) {
		if ((trail[i] >= DOUBLE_BACK_1) && (trail[i] <= DOUBLE_BACK_5)) {
			doubleBackValid = false;
		}
		if (trail[i] == HIDE) hideValid = false;
	}

	// Adding DOUBLE_BACK moves into trail if it has not appeared in the trail, 
	// if it is possible to (trail is long enough), and if the location of the DOUBLE_BACK move is reachable
	if (doubleBackValid == true) {
		for (PlaceId curr = DOUBLE_BACK_1; curr <= DOUBLE_BACK_5; curr++) {
			if (numTrail <= (curr - DOUBLE_BACK_1)) break;

			// Finding the location of the DOUBLE_BACK move
			PlaceId doubleBackLoc = locHistory[numHistory - 1 - (curr - DOUBLE_BACK_1)];

			if (DvIsRepeat(doubleBackLoc, reachable, &numReachable)) {
				(*numReturnedMoves)++;
				validMoves = realloc(validMoves, sizeof(PlaceId) * (*numReturnedMoves));
				assert (validMoves != NULL);
				validMoves[(*numReturnedMoves) - 1] = curr;
			}
		}
	}

	// Adding HIDE to validMoves if it has not appeared in trail, if Dracula's current location is not
	// the SEA and if there has been more than 1 move in the trail
	if ((hideValid == true) && (!placeIsSea(currLoc)) && (numTrail > 0)) { 
		(*numReturnedMoves)++;
		validMoves = realloc(validMoves, sizeof(PlaceId) * (*numReturnedMoves));
		assert (validMoves != NULL);
		validMoves[(*numReturnedMoves) - 1] = HIDE;
	}

	free(reachable);
	if (trailCanFree == true) free(trail);
	if (historyCanFree == true) free(locHistory);
	return validMoves;
}

PlaceId *DvWhereCanIGo(DraculaView dv, int *numReturnedLocs)
{
	*numReturnedLocs = 0;
	PlaceId location = DvGetPlayerLocation(dv, PLAYER_DRACULA);
	if (location == NOWHERE) return NULL;

	int numValidMoves = 0;
	PlaceId *validMoves = DvGetValidMoves(dv, &numValidMoves);
	if (numValidMoves == 0) return NULL;

	// If double back is a valid move, we can return all adjacent locations reachable by
	// Dracula.
	if (foundDoubleBack(validMoves, &numValidMoves)) {
		Round round = DvGetRound(dv);
		return GvGetReachable(dv->gv, PLAYER_DRACULA, round, location, numReturnedLocs);
	}

	// If hide and double back is not a valid move, the only valid moves are his 
	// location moves.
	PlaceId *reachableLocations = NULL;
	reachableLocations = addRealPlaces(reachableLocations, validMoves, 
									   numReturnedLocs, &numValidMoves);
							
	if (!foundHide(validMoves, &numValidMoves) || placeIsSea(location)) {
		return reachableLocations;
	}
	// If hide is a valid move but not double back, another possible valid move
	// is staying at his current location, given that his current location is not at
	// sea.
	if (!placeIsSea(location)) reachableLocations = DvAddPlaceId(location, reachableLocations, numReturnedLocs);
	return reachableLocations;
}

PlaceId *DvWhereCanIGoByType(DraculaView dv, bool road, bool boat,
                             int *numReturnedLocs)
{
	*numReturnedLocs = 0;
	PlaceId location = DvGetPlayerLocation(dv, PLAYER_DRACULA);
	if (location == NOWHERE) return NULL;

	int numValidMoves = 0;
	PlaceId *validMoves = DvGetValidMoves(dv, &numValidMoves);
	if (numValidMoves == 0) return NULL;

	Round round = DvGetRound(dv);
	PlaceId *reachableLocations = GvGetReachableByType(dv->gv, PLAYER_DRACULA, round, 
													   location, road, CAN_DRACULA_RAIL, 
													   boat, numReturnedLocs);

	// If double back is a valid move, we can return all adjacent locations for given transport type
	if (foundDoubleBack(validMoves, &numValidMoves)) return reachableLocations;
	// If hide and double back is not a valid move, the only valid moves are his 
	// location moves. Eliminate any reachable locations that aren't a valid location move.
	if (!foundHide(validMoves, &numValidMoves) || placeIsSea(location)) {
		for (int i = 0; i < numValidMoves; i++) {
			if (!DvIsRepeat(reachableLocations[i], validMoves, &numValidMoves)) {
				reachableLocations = removeLocation(reachableLocations, reachableLocations[i], numReturnedLocs);
			}
			if (*numReturnedLocs == 0) return NULL;
		}
		return reachableLocations;
	}

	// If hide is a valid move but not double back, another possible valid move
	// is staying at his current location, given that his current location is not at
	// sea.
	for (int i = 0; i < numValidMoves; i++) {
		if (reachableLocations[i] == location) continue;
		if (!DvIsRepeat(reachableLocations[i], validMoves, &numValidMoves)) {
			reachableLocations = removeLocation(reachableLocations, reachableLocations[i], numReturnedLocs);
		}
	}
	return reachableLocations;
}

PlaceId *DvWhereCanTheyGo(DraculaView dv, Player player,
                          int *numReturnedLocs)
{
	*numReturnedLocs = 0;
	if (player == PLAYER_DRACULA) return DvWhereCanIGo(dv, numReturnedLocs);
	PlaceId location = DvGetPlayerLocation(dv, player);
	if (location == NOWHERE) return NULL;
	
	Round round = DvGetRound(dv) + 1;
	return GvGetReachable(dv->gv, player, round, location, numReturnedLocs);
}

PlaceId *DvWhereCanTheyGoByType(DraculaView dv, Player player,
                                bool road, bool rail, bool boat,
                                int *numReturnedLocs)
{
	*numReturnedLocs = 0;
	PlaceId location = DvGetPlayerLocation(dv, player);
	if (location == NOWHERE) return NULL;
	if (player == PLAYER_DRACULA) return DvWhereCanIGoByType(dv, road, boat, numReturnedLocs);

	Round round = DvGetRound(dv) + 1;
	return GvGetReachableByType(dv->gv, player, round, location, road, rail, boat, numReturnedLocs);
}

////////////////////////////////////////////////////////////////////////
// Your own interface functions

// TODO
