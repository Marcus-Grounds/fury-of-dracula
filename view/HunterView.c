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

#include "Game.h"
#include "GameView.h"
#include "HunterView.h"
#include "Map.h"
#include "Places.h"
// add your own #includes here

// TODO: ADD YOUR OWN STRUCTS HERE

struct hunterView {
	GameView gv;
};

////////////////////////////////////////////////////////////////////////
// Constructor/Destructor

HunterView HvNew(char *pastPlays, Message messages[])
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	HunterView new = malloc(sizeof(*new));
	if (new == NULL) {
		fprintf(stderr, "Couldn't allocate HunterView!\n");
		exit(EXIT_FAILURE);
	}

	return new;
}

void HvFree(HunterView hv)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	free(hv);
}

////////////////////////////////////////////////////////////////////////
// Game State Information

Round HvGetRound(HunterView hv)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
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
	return 0;
}

int HvGetHealth(HunterView hv, Player player)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	return 0;
}

PlaceId HvGetPlayerLocation(HunterView hv, Player player)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	return NOWHERE;
}

PlaceId HvGetVampireLocation(HunterView hv)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	return NOWHERE;
}

////////////////////////////////////////////////////////////////////////
// Utility Functions

PlaceId HvGetLastKnownDraculaLocation(HunterView hv, Round *round)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	*round = 0;
	return NOWHERE;
}

PlaceId *HvGetShortestPathTo(HunterView hv, Player hunter, PlaceId dest,
                             int *pathLength)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	*pathLength = 0;
	return NULL;
}

////////////////////////////////////////////////////////////////////////
// Making a Move
bool canRevealDracula(HunterView hv) {
	PlaceId draculaLoc = GvGetPlayerLocation(hv->gv, PLAYER_DRACULA);
	if (draculaLoc == CASTLE_DRACULA) return true;
	for (Player i = PLAYER_LORD_GODALMING; i < PLAYER_MINA_HARKER; i++) {
		if (draculaLoc == GvGetPlayerLocation(hv->gv, i)) return true; 
	}
	return false;
}

PlaceId *HvWhereCanIGo(HunterView hv, int *numReturnedLocs)
{
	*numReturnedLocs = 0;
	Player player = HvGetPlayer(hv);
	Round round = HvGetRound(hv);
	PlaceId location = HvGetPlayerLocation(hv, player);
	if (location == NOWHERE) return NULL;

	return GvGetReachable(hv->gv, player, round, location, numReturnedLocs);
}

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

// TODO
