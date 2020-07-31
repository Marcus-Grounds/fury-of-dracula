////////////////////////////////////////////////////////////////////////
// COMP2521 20T2 ... the Fury of Dracula
// HunterView.c: the HunterView ADT implementation
//
// 2014-07-01	v1.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2017-12-01	v1.1	Team Dracula <cs2521@cse.unsw.edu.au>
// 2018-12-31	v2.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2020-07-10   v3.0    Team Dracula <cs2521@cse.unsw.edu.au>
//
////////////////////////////////////////////////////////////////////////

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "queue.h"

#include "Game.h"
#include "GameView.h"
#include "HunterView.h"
#include "Map.h"
#include "Places.h"

////////////////////////////////////////////////////////////////////////
bool HvIsRepeat(PlaceId new, PlaceId *reachableLocations, int *numReturnedLocs);
PlaceId *HvAddPlaceId(PlaceId new, PlaceId *reachableLocations,
					  int *numReturnedLocs);
// Taken from testUtils.h to make HvGetShortestPathTo pass tests.
void HvSortPlaces(PlaceId *places, int numPlaces);

struct hunterView {
	GameView gv;
};

////////////////////////////////////////////////////////////////////////

HunterView HvNew(char *pastPlays, Message messages[])
{
	HunterView new = malloc(sizeof(*new));
	if (new == NULL) {
		fprintf(stderr, "Couldn't allocate HunterView!\n");
		exit(EXIT_FAILURE);
	}

	new->gv = GvNew(pastPlays, messages);

	return new;
}

void HvFree(HunterView hv)
{
	GvFree(hv->gv);
	free(hv);
}

////////////////////////////////////////////////////////////////////////

Round HvGetRound(HunterView hv)
{
	return GvGetRound(hv->gv);
}

Player HvGetPlayer(HunterView hv)
{
	return GvGetPlayer(hv->gv);
}

int HvGetScore(HunterView hv)
{
	return GvGetScore(hv->gv);
}

int HvGetHealth(HunterView hv, Player player)
{
	return GvGetHealth(hv->gv, player);
}

PlaceId HvGetPlayerLocation(HunterView hv, Player player)
{
	return GvGetPlayerLocation(hv->gv, player);
}

PlaceId HvGetVampireLocation(HunterView hv)
{
	return GvGetVampireLocation(hv->gv);
}

////////////////////////////////////////////////////////////////////////
// Utility Functions

PlaceId HvGetLastKnownDraculaLocation(HunterView hv, Round *round)
{
	bool canFree = false;
	int numLocs = -1;
	
	PlaceId *locationHistory = GvGetLocationHistory(hv->gv,  PLAYER_DRACULA, 
	         										 &numLocs, &canFree);

	for (int i = numLocs - 1; i >= 0; i--) {
		if (placeIsReal(locationHistory[i])) {
			*round = i;
			return locationHistory[i];
		}
	}
	
	if (canFree) free(locationHistory);

	return NOWHERE;
}

PlaceId *HvGetShortestPathTo(HunterView hv, Player hunter, PlaceId dest,
                             int *pathLength)
{
	*pathLength = 0;
	if (hunter == PLAYER_DRACULA) return NULL;

	PlaceId src = HvGetPlayerLocation(hv, hunter);
	if (src == NOWHERE) return NULL;
	Player currPlayer = HvGetPlayer(hv);
	Round round = hunter >= currPlayer? HvGetRound(hv): HvGetRound(hv) + 1;

	// Initialise visited array to NOWHERE;
	PlaceId visited[NUM_REAL_PLACES];
	for (int i = 0; i < NUM_REAL_PLACES; i++) {
		visited[i] = NOWHERE;
	}

	// Conduct breadth first search from source.
	visited[src] = src;
	Queue q = createQueue();
	enqueue(q, src);
	while (!queueIsEmpty(q)) {
		PlaceId intermediate = dequeue(q);
		if (intermediate != dest) {
			// Calculate the number of rounds from source location required to
			// reach intermediate location.
			PlaceId roundsFromSrc = 0;
			for (PlaceId inBetween = intermediate; 
				 inBetween != src; 
				 inBetween = visited[inBetween]) {
				roundsFromSrc++;
			}

			// Get reachable locations from current location.
			int numLocs = -1;
			PlaceId *intermediateConns = GvGetReachable(hv->gv, hunter, 
														round + roundsFromSrc, 
														intermediate, 
														&numLocs);
			HvSortPlaces(intermediateConns, numLocs);

			// Enqueue reachable locations from current location.
			for (int i = 0; i < numLocs; i++) {
				if (visited[intermediateConns[i]] == NOWHERE) {
					enqueue(q, intermediateConns[i]);
					visited[intermediateConns[i]] = intermediate;
				}
			}
		}
	}

	// Find shortest path in reverse order.
	PlaceId *shortestReversePath = NULL;
	int reversePathLength = 0;
	for (PlaceId intermediate = dest; 
		 intermediate != src; 
		 intermediate = visited[intermediate]) {
		shortestReversePath = HvAddPlaceId(intermediate, shortestReversePath,
										   &reversePathLength);
	}

	// Find shortest path in correct order.
	PlaceId *shortestPath = NULL;
	for (int i = reversePathLength - 1; i >= 0; i--) {
		shortestPath = HvAddPlaceId(shortestReversePath[i], shortestPath, 
									pathLength);
	}

	free(shortestReversePath);
	return shortestPath;
}


////////////////////////////////////////////////////////////////////////
// Making a Move

PlaceId *HvWhereCanIGo(HunterView hv, int *numReturnedLocs)
{
	return HvWhereCanIGoByType(hv, true, true, true, numReturnedLocs);
}

PlaceId *HvWhereCanIGoByType(HunterView hv, bool road, bool rail,
                             bool boat, int *numReturnedLocs)
{
	Player player = HvGetPlayer(hv);
	return HvWhereCanTheyGoByType(hv, player, road, rail, boat, 
								  numReturnedLocs);
}

PlaceId *HvWhereCanTheyGo(HunterView hv, Player player,
                          int *numReturnedLocs)
{
	return HvWhereCanTheyGoByType(hv, player, true, true, true, 
								  numReturnedLocs);
}

PlaceId *HvWhereCanTheyGoByType(HunterView hv, Player player,
                                bool road, bool rail, bool boat,
                                int *numReturnedLocs)
{
	*numReturnedLocs = 0;
	Player currPlayer = HvGetPlayer(hv);
	Round round = player >= currPlayer? HvGetRound(hv): HvGetRound(hv) + 1;
	PlaceId location = HvGetPlayerLocation(hv, player);
	if (location == NOWHERE) return NULL;
	if (player != PLAYER_DRACULA || (location != CITY_UNKNOWN && location != SEA_UNKNOWN))
		return GvGetReachableByType(hv->gv, player, round, location, road, rail, boat, 
									numReturnedLocs);
	return NULL;
}

////////////////////////////////////////////////////////////////////////
// Your own interface functions
// Checks if a location is already in reachable locations array.
bool HvIsRepeat(PlaceId new, PlaceId *reachableLocations, int *numReturnedLocs) {
	for (int i = 0; i < (*numReturnedLocs); i++) {
		if (reachableLocations[i] == new) return true;
	}
	return false;
}

// Add a PlaceId to an array of locations.
PlaceId *HvAddPlaceId(PlaceId new, PlaceId *reachableLocations,
					  int *numReturnedLocs) {
	if (HvIsRepeat(new, reachableLocations, numReturnedLocs)) 
		return reachableLocations;
	(*numReturnedLocs)++;
	reachableLocations = realloc(reachableLocations, 
								 (*numReturnedLocs) * sizeof(PlaceId));
	assert(reachableLocations != NULL);
	reachableLocations[(*numReturnedLocs) - 1] = new;
	return reachableLocations;
}

// Functions used to sort places in numeric (and alphabetic) order.
// Taken from testUtils.c to make HvGetShortestPathTo pass tests.
int HvPlaceIdCmp(const void *ptr1, const void *ptr2) {
	PlaceId p1 = *(PlaceId *)ptr1;
	PlaceId p2 = *(PlaceId *)ptr2;
	return p1 - p2;
}

void HvSortPlaces(PlaceId *places, int numPlaces) {
	qsort(places, (size_t)numPlaces, sizeof(PlaceId), HvPlaceIdCmp);
}