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
// Taken from testUtils.h to make HvGetShortestPathTo pass tests.
void HvSortPlaces(PlaceId *places, int numPlaces);

struct hunterView {
	GameView gv;
	int round;
};

////////////////////////////////////////////////////////////////////////
HunterView HvNew(char *pastPlays, Message messages[])
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
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


Round HvGetRound(HunterView hv)
{
	return GvGetRound(hv->gv);
}

Player HvGetPlayer(HunterView hv)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	return GvGetPlayer(hv->gv);
}

int HvGetScore(HunterView hv)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	return GvGetScore(hv->gv);
}

int HvGetHealth(HunterView hv, Player player)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	return GvGetHealth(hv->gv, player);
}

PlaceId HvGetPlayerLocation(HunterView hv, Player player)
{
	
	return GvGetPlayerLocation(hv->gv, player);
}

PlaceId HvGetVampireLocation(HunterView hv)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	return GvGetVampireLocation(hv->gv);
}

////////////////////////////////////////////////////////////////////////
// Utility Functions

PlaceId HvGetLastKnownDraculaLocation(HunterView hv, Round *round)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	bool *space = malloc(sizeof(bool *));
	int *ptr = malloc(sizeof(int *));
	
	PlaceId *location_history = GvGetLocationHistory(hv->gv,  PLAYER_DRACULA, ptr,  space);

	for (int i = *ptr; i >= 0; i--) {
		if (location_history[i] != SEA_UNKNOWN && location_history[i] != CITY_UNKNOWN &&location_history[i] != 0) {
			*round = i;
			printf("LOC::%d   %d\n\n", i, location_history[i]);
			return location_history[i];
		}
	}

	return NOWHERE;
}

PlaceId *HvAddPlaceId(PlaceId new, PlaceId *reachableLocations,
					int *numReturnedLocs) {
	(*numReturnedLocs)++;
	reachableLocations = realloc(reachableLocations, 
								 (*numReturnedLocs) * sizeof(PlaceId));
	assert(reachableLocations != NULL);
	reachableLocations[(*numReturnedLocs) - 1] = new;
	return reachableLocations;
}

PlaceId *HvGetShortestPathTo(HunterView hv, Player hunter, PlaceId dest,
                             int *pathLength)
{
	*pathLength = 0;
	PlaceId src = HvGetPlayerLocation(hv, hunter);
	Round round = HvGetRound(hv);

	// Initialise visited array to 0;
	PlaceId visited[NUM_REAL_PLACES];
	for (int i = 0; i < NUM_REAL_PLACES; i++) {
		visited[i] = NOWHERE;
	}
	visited[src] = src;

	Queue q = createQueue();

	enqueue(q, src);
	while (!queueIsEmpty(q)) {
		PlaceId intermediate = dequeue(q);
		if (intermediate != dest) {
			PlaceId roundsFromSrc = 0;
			for (PlaceId inBetween = intermediate; inBetween != src; inBetween = visited[inBetween]) {
				roundsFromSrc++;
			}
			int numLocs = -1;
			PlaceId *intermediateConns = GvGetReachable(hv->gv, hunter, round + roundsFromSrc, intermediate, &numLocs);
			HvSortPlaces(intermediateConns, numLocs);
			for (int i = 0; i < numLocs; i++) {
				if (visited[intermediateConns[i]] == NOWHERE) {
					enqueue(q, intermediateConns[i]);
					visited[intermediateConns[i]] = intermediate;
				}
			}
		}
	}

	// Find shortest path in reverse order
	PlaceId *shortestReversePath = NULL;
	int reversePathLength = 0;
	for (PlaceId intermediate = dest; intermediate != src; intermediate = visited[intermediate]) {
		shortestReversePath = HvAddPlaceId(intermediate, shortestReversePath, &reversePathLength);
	}

	// Find shortest path in correct order
	PlaceId *shortestPath = NULL;
	for (int i = reversePathLength - 1; i >= 0; i--) {
		shortestPath = HvAddPlaceId(shortestReversePath[i], shortestPath, pathLength);
	}

	free(shortestReversePath);
	return shortestPath;
}


////////////////////////////////////////////////////////////////////////
// Making a Move

// Returns all reachable locations for given hunter player, and NULL
// if player hasn't made a move.
PlaceId *HvWhereCanIGo(HunterView hv, int *numReturnedLocs)
{
	*numReturnedLocs = 0;
	Player player = HvGetPlayer(hv);
	Round round = HvGetRound(hv);
	PlaceId location = HvGetPlayerLocation(hv, player);
	if (location == NOWHERE) return NULL;

	return GvGetReachable(hv->gv, player, round, location, numReturnedLocs);
}

// Returns all reachable locations for current player, 
PlaceId *HvWhereCanIGoByType(HunterView hv, bool road, bool rail,
                             bool boat, int *numReturnedLocs)
{
	*numReturnedLocs = 0;
	Player player = HvGetPlayer(hv);
	Round round = HvGetRound(hv);
	PlaceId location = HvGetPlayerLocation(hv, player);
	if (location == NOWHERE) return NULL;

	return GvGetReachableByType(hv->gv, player, round, location, road, rail, boat, 
								numReturnedLocs);
}

PlaceId *HvWhereCanTheyGo(HunterView hv, Player player,
                          int *numReturnedLocs)
{
	*numReturnedLocs = 0;
	Player currPlayer = HvGetPlayer(hv);
	Round round = player >= currPlayer? HvGetRound(hv): HvGetRound(hv) + 1;
	PlaceId location = HvGetPlayerLocation(hv, player);
	if (location == NOWHERE) return NULL;
	if (player != PLAYER_DRACULA || (location != CITY_UNKNOWN && location != SEA_UNKNOWN))
		return GvGetReachable(hv->gv, player, round, location, numReturnedLocs);
	
	return NULL;
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

// Functions used to sort places in numeric order.
// Taken from testUtils.c to make HvGetShortestPathTo pass tests.
int HvPlaceIdCmp(const void *ptr1, const void *ptr2) {
	PlaceId p1 = *(PlaceId *)ptr1;
	PlaceId p2 = *(PlaceId *)ptr2;
	return p1 - p2;
}

void HvSortPlaces(PlaceId *places, int numPlaces) {
	qsort(places, (size_t)numPlaces, sizeof(PlaceId), HvPlaceIdCmp);
}