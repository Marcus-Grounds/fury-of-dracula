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

#define isDoubleBack(move) (move >= DOUBLE_BACK_1 && move <= DOUBLE_BACK_5)
#define CAN_DRACULA_RAIL false 

struct draculaView {
	GameView gv;
};

bool DvIsRepeat(PlaceId new, PlaceId *reachableLocations, 
				int *numReturnedLocs);
bool DvFoundDoubleBack(PlaceId *validMoves, int *numValidMoves);
bool DvFoundHide(PlaceId *validMoves, int *numValidMoves);
PlaceId *DvAddPlaceId(PlaceId new, PlaceId *reachableLocations,
					  int *numReturnedLocs);
PlaceId *DvRemoveLocation(PlaceId *reachableLocations, PlaceId whichLocation, 
						  int *numReturnedLocs);
PlaceId *DvAddRealPlaces(PlaceId *reachableLocations, PlaceId *validMoves,
					     int *numReturnedLocs, int *numValidMoves);


////////////////////////////////////////////////////////////////////////
// Constructor/Destructor

DraculaView DvNew(char *pastPlays, Message messages[])
{
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
	GvFree (dv->gv);
	free(dv);
}

////////////////////////////////////////////////////////////////////////
// Game State Information

Round DvGetRound(DraculaView dv)
{
	return GvGetRound(dv->gv);
}

int DvGetScore(DraculaView dv)
{
	return GvGetScore(dv->gv);
}

int DvGetHealth(DraculaView dv, Player player)
{
	return GvGetHealth(dv->gv, player);
}

PlaceId DvGetPlayerLocation(DraculaView dv, Player player)
{
	return GvGetPlayerLocation(dv->gv, player);
}

PlaceId DvGetVampireLocation(DraculaView dv)
{
	return GvGetVampireLocation(dv->gv);
}

PlaceId *DvGetTrapLocations(DraculaView dv, int *numTraps)
{
	*numTraps = 0;
	return GvGetTrapLocations(dv->gv, numTraps);
}

////////////////////////////////////////////////////////////////////////
// Making a Move

PlaceId *DvGetValidMoves(DraculaView dv, int *numReturnedMoves)
{
	*numReturnedMoves = 0;
	int numTrail = 0; 
	bool trailCanFree = false;
	PlaceId *trail = GvGetLastMoves(dv->gv, PLAYER_DRACULA, TRAIL_SIZE, 
	                                &numTrail, &trailCanFree);
	if (numTrail == 0) return NULL;
	
	Round round = GvGetRound(dv->gv);
	PlaceId currLoc = GvGetPlayerLocation(dv->gv, PLAYER_DRACULA);
	int numReachable = 0;
	PlaceId *reachable = GvGetReachable(dv->gv, PLAYER_DRACULA, round, 
	                                    currLoc, &numReachable);

	int numHistory = 0; 
	bool historyCanFree = false;
	PlaceId *locHistory = GvGetLocationHistory(dv->gv, PLAYER_DRACULA, 
	                                           &numHistory, &historyCanFree);

	// Adding the reachable move into validMoves if it is not in the trail
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
			PlaceId doubleBackLoc = locHistory[numHistory - 1 - 
			                                   (curr - DOUBLE_BACK_1)];

			if (DvIsRepeat(doubleBackLoc, reachable, &numReachable)) {
				(*numReturnedMoves)++;
				validMoves = realloc(validMoves, sizeof(PlaceId) * 
				                    (*numReturnedMoves));
				assert (validMoves != NULL);
				validMoves[(*numReturnedMoves) - 1] = curr;
			}
		}
	}

	// Adding HIDE to validMoves if it has not appeared in trail, 
	// if Dracula's current location is not the SEA and 
	// if there has been more than 1 move in the trail
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
	// If no moves have been made yet, return NULL.
	if (location == NOWHERE) return NULL;

	// Get all valid moves.
	int numValidMoves = 0;
	PlaceId *validMoves = DvGetValidMoves(dv, &numValidMoves);
	if (numValidMoves == 0) return NULL;

	// If double back is a valid move, we can return all adjacent locations 
	// reachable by Dracula.
	if (DvFoundDoubleBack(validMoves, &numValidMoves)) {
		Round round = DvGetRound(dv);
		return GvGetReachable(dv->gv, PLAYER_DRACULA, round, location, 
							  numReturnedLocs);
	}

	// Double back is not a valid move.

	// If hide is also not a valid move, or Dracula is at sea, the only valid 
	// moves are his location moves.
	PlaceId *reachableLocations = NULL;
	reachableLocations = DvAddRealPlaces(reachableLocations, validMoves, 
									   numReturnedLocs, &numValidMoves);
	if (!DvFoundHide(validMoves, &numValidMoves) || placeIsSea(location)) {
		return reachableLocations;
	}

	// Double back is invalid, hide is a valid, current location is not at sea

	// Another valid move is staying at his current location, given that his 
	// current location is not at sea.
	reachableLocations = DvAddPlaceId(location, reachableLocations, 
								      numReturnedLocs);
	return reachableLocations;
}

PlaceId *DvWhereCanIGoByType(DraculaView dv, bool road, bool boat,
                             int *numReturnedLocs)
{
	*numReturnedLocs = 0;
	PlaceId location = DvGetPlayerLocation(dv, PLAYER_DRACULA);
	// If no moves have been made, return NULL;
	if (location == NOWHERE) return NULL;

	int numValidMoves = 0;
	PlaceId *validMoves = DvGetValidMoves(dv, &numValidMoves);
	// If no valid moves available, return NULL;
	if (numValidMoves == 0) return NULL;

	// Get all locations reachable by given transport types.
	Round round = DvGetRound(dv);
	PlaceId *reachableLocations = GvGetReachableByType(dv->gv, 
													   PLAYER_DRACULA, round, 
													   location, road, 
													   CAN_DRACULA_RAIL, 
													   boat, numReturnedLocs);

	// If double back is a valid move, we can return all adjacent locations for 
	// given transport type
	if (DvFoundDoubleBack(validMoves, &numValidMoves)) 
		return reachableLocations;

	// Double back is not a valid move.

	// If, additionally, hide is not a valid move or Dracula's location is at 
	// sea, the only valid moves are his location moves.  Eliminate any 
	// reachable locations that aren't a valid location move. 
	if (!DvFoundHide(validMoves, &numValidMoves) || placeIsSea(location)) {
		for (int i = 0; i < *numReturnedLocs; i++) {
			if (!DvIsRepeat(reachableLocations[i], validMoves, 
			                &numValidMoves)) {
				// The current location in reachableLocation[i] array is not a 
				// valid location move.
				reachableLocations = DvRemoveLocation(reachableLocations, 
													  reachableLocations[i], 
													  numReturnedLocs);
				i--;

				if (*numReturnedLocs == 0) {
					// No valid locations left in array.
					free(reachableLocations);
					return NULL;
				}
			}
		}
		return reachableLocations;
	}

	// Double back is invalid, hide is valid and Dracula is not at sea.

	// Another valid move is staying at his current location.
	// Remove all non-location moves from reachable locations array, except his 
	// current location.
	for (int i = 0; i < *numReturnedLocs; i++) {
		if (reachableLocations[i] == location) continue;
		if (!DvIsRepeat(reachableLocations[i], validMoves, &numValidMoves)) {
			// The current location in reachableLocation[i] array is not a 
			// valid location move.
			reachableLocations = DvRemoveLocation(reachableLocations, 
												  reachableLocations[i], 
												  numReturnedLocs);
			i--;
			if (*numReturnedLocs == 0) {
				// No valid locations left in array.
				free(reachableLocations);
				return NULL;
			}
		}
	}
	
	return reachableLocations;
}

PlaceId *DvWhereCanTheyGo(DraculaView dv, Player player,
                          int *numReturnedLocs)
{
	return DvWhereCanTheyGoByType(dv, player, true, true, true, 
								  numReturnedLocs);
}

PlaceId *DvWhereCanTheyGoByType(DraculaView dv, Player player,
                                bool road, bool rail, bool boat,
                                int *numReturnedLocs)
{
	*numReturnedLocs = 0;
	if (player == PLAYER_DRACULA) return DvWhereCanIGoByType(dv, road, boat, 
	             											 numReturnedLocs);
	// Handle case when player is hunter.
	PlaceId location = DvGetPlayerLocation(dv, player);
	if (location == NOWHERE) return NULL;

	// Return the valid moves for the player in the next round.
	Round round = DvGetRound(dv) + 1;
	return GvGetReachableByType(dv->gv, player, round, location, road, rail, 
	     						boat, numReturnedLocs);
}

////////////////////////////////////////////////////////////////////////
// Interface functions
// Tests if a location is in an array of locations.
bool DvIsRepeat(PlaceId new, PlaceId *reachableLocations, int *numReturnedLocs) 
{
	for (int i = 0; i < (*numReturnedLocs); i++) {
		if (reachableLocations[i] == new) return true;
	}
	return false;
}

// Tests if double back is a valid move.
bool DvFoundDoubleBack(PlaceId *validMoves, int *numValidMoves) 
{
	for (int i = 0; i < *numValidMoves; i++) {
		if (isDoubleBack(validMoves[i])) return true;
	}
	return false;
}

// Tests if hide is a valid move.
bool DvFoundHide(PlaceId *validMoves, int *numValidMoves) 
{
	for (int i = 0; i < *numValidMoves; i++) {
		if (validMoves[i] == HIDE) return true;
	}
	return false;
}

// Adds location to an array, given location is not already in array.
PlaceId *DvAddPlaceId(PlaceId new, PlaceId *reachableLocations,
					int *numReturnedLocs) {
	if (DvIsRepeat(new, reachableLocations, numReturnedLocs)) 
		return reachableLocations;
	(*numReturnedLocs)++;
	reachableLocations = realloc(reachableLocations, 
								 (*numReturnedLocs) * sizeof(PlaceId));
	assert(reachableLocations != NULL);
	reachableLocations[(*numReturnedLocs) - 1] = new;
	return reachableLocations;
}

// Removes a location from an array, if the location is in that array.
PlaceId *DvRemoveLocation(PlaceId *reachableLocations, PlaceId whichLocation, 
						  int *numReturnedLocs) 
{

	int i = 0;
	// Look for location in array.
	for (i = 0; i < *numReturnedLocs; i++) {
		if (reachableLocations[i] == whichLocation) break;
	}

	// If location was not found in array, return old array.
	if (i == *numReturnedLocs) return reachableLocations;
	
	// Remove location.
	(*numReturnedLocs)--;

	while (i < *numReturnedLocs) {
		reachableLocations[i] = reachableLocations[i + 1];
		i++;
	}

	// Reduce size of array.
	reachableLocations = realloc(reachableLocations,
								 (*numReturnedLocs) * sizeof(PlaceId));
	
	return reachableLocations;
}

// Returns all real places from the array of valid moves.
PlaceId *DvAddRealPlaces(PlaceId *reachableLocations, PlaceId *validMoves,
					   int *numReturnedLocs, int *numValidMoves) 
{
	for (int i = 0; i < *numValidMoves; i++) {
		if (placeIsReal(validMoves[i])) {
			reachableLocations = DvAddPlaceId(validMoves[i], 
											  reachableLocations, 
											  numReturnedLocs);
		}
	}
	return reachableLocations;
}

