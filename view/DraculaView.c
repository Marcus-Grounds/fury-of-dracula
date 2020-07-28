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
	return GvGetScore(dv->gv);
}

int DvGetHealth(DraculaView dv, Player player)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	return GvGetHealth(dv->gv, player);
}

PlaceId DvGetPlayerLocation(DraculaView dv, Player player)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	return GvGetPlayerLocation(dv->gv, player);
}

PlaceId DvGetVampireLocation(DraculaView dv)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	return GvGetVampireLocation(dv->gv);
}

PlaceId *DvGetTrapLocations(DraculaView dv, int *numTraps)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	PlaceId *TrapLocations = GvGetTrapLocations(dv->gv, numTraps);
	return TrapLocations;
}

////////////////////////////////////////////////////////////////////////
// Making a Move
bool DvIsRepeat(PlaceId *lastPlaces, PlaceId newLocation, int *numReturnedLocs) 
{
	for (int i = 0; i < (*numReturnedLocs); i++) {
		if (lastPlaces[i] == newLocation) return true;
	}
	return false;
}

bool foundDoubleBack(PlaceId *validMoves, int *pNumValidMoves) 
{
	for (int i = 0; i < (*pNumValidMoves); i++) {
		if (isDoubleBack(validMoves[i])) return true;
	}
	return false;
}

bool foundHide(PlaceId *validMoves, int *pNumValidMoves) 
{
	for (int i = 0; i < (*pNumValidMoves); i++) {
		if (validMoves[i] == HIDE) return true;
	}
	return false;
}

PlaceId *DvAddPlaceId(PlaceId new, PlaceId *reachableLocations,
					int *numReturnedLocs) {
	(*numReturnedLocs)++;
	PlaceId *oldArray = reachableLocations;
	reachableLocations = realloc(reachableLocations, 
								 (*numReturnedLocs) * sizeof(PlaceId));
	reachableLocations[(*numReturnedLocs) - 1] = new;
	free(oldArray);
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
	PlaceId *oldArray = reachableLocations;
	reachableLocations = realloc(reachableLocations, (*numReturnedLocs) * sizeof(PlaceId));
	free(oldArray);

	return reachableLocations;
}

PlaceId *addRealPlaces(PlaceId *reachableLocations, PlaceId *validMoves,
					   int *numReturnedLocs, int *pNumValidMoves) 
{
	for (int i = 0; i < (*pNumValidMoves); i++) {
		if (placeIsReal(validMoves[i])) {
			reachableLocations = DvAddPlaceId(validMoves[i], reachableLocations, 
											numReturnedLocs);
		}
	}
	return reachableLocations;
}

PlaceId *DvGetValidMoves(DraculaView dv, int *numReturnedMoves)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	*numReturnedMoves = 0;
	return NULL;
}

PlaceId *DvWhereCanIGo(DraculaView dv, int *numReturnedLocs)
{
	*numReturnedLocs = 0;
	PlaceId location = DvGetPlayerLocation(dv, PLAYER_DRACULA);
	if (location == NOWHERE) return NULL;

	int numValidMoves = 0;
	int *pNumValidMoves = &numValidMoves;
	PlaceId *validMoves = DvGetValidMoves(dv, pNumValidMoves);
	if (*pNumValidMoves == 0) return NULL;

	// If double back is a valid move, we can return all adjacent locations reachable by
	// Dracula.
	if (foundDoubleBack(validMoves, pNumValidMoves)) {
		Round round = DvGetRound(dv);
		return GvGetReachable(dv->gv, PLAYER_DRACULA, round, location, numReturnedLocs);
	}

	// If hide and double back is not a valid move, the only valid moves are his 
	// location moves.
	PlaceId *reachableLocations = NULL;
	reachableLocations = addRealPlaces(reachableLocations, validMoves, 
									   numReturnedLocs, pNumValidMoves);
	if (!foundHide(validMoves, pNumValidMoves)) return reachableLocations;

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
	int *pNumValidMoves = &numValidMoves;
	PlaceId *validMoves = DvGetValidMoves(dv, pNumValidMoves);
	if (*pNumValidMoves == 0) return NULL;

	Round round = DvGetRound(dv);
	PlaceId *reachableLocations = GvGetReachableByType(dv->gv, PLAYER_DRACULA, round, 
													   location, road, CAN_DRACULA_RAIL, 
													   boat, numReturnedLocs);

	// If double back is a valid move, we can return all adjacent locations for given transport type
	if (foundDoubleBack(validMoves, pNumValidMoves)) return reachableLocations;

	// If hide and double back is not a valid move, the only valid moves are his 
	// location moves. Eliminate any reachable locations that aren't a valid location move.
	if (!foundHide(validMoves, pNumValidMoves)) {
		for (int i = 0; i < *pNumValidMoves; i++) {
			if (!DvIsRepeat(validMoves, reachableLocations[i], pNumValidMoves)) {
				reachableLocations = removeLocation(reachableLocations, reachableLocations[i], numReturnedLocs);
			}
		}
		return reachableLocations;
	}

	// If hide is a valid move but not double back, another possible valid move
	// is staying at his current location, given that his current location is not at
	// sea.
	if (!placeIsSea(location)) {
		for (int i = 0; i < *pNumValidMoves; i++) {
		if (reachableLocations[i] == location) continue;
		if (!DvIsRepeat(validMoves, reachableLocations[i], pNumValidMoves)) {
				reachableLocations = removeLocation(reachableLocations, reachableLocations[i], numReturnedLocs);
			}
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
