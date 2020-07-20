////////////////////////////////////////////////////////////////////////
// COMP2521 20T2 ... the Fury of Dracula
// GameView.c: GameView ADT implementation
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
#include "Map.h"
#include "Places.h"

#define TOTAL_PLAYERS 5
#define TOTAL_PLACES 71
// add your own #includes here

typedef struct travel {
	int rail;
	bool road;
	bool sea;
} Travel;

struct gameView {
	Travel **edges;
	int turn;
};

////////////////////////////////////////////////////////////////////////
// Constructor/Destructor

GameView GvNew(char *pastPlays, Message messages[])
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	GameView new = malloc(sizeof(*new));
	if (new == NULL) {
		fprintf(stderr, "Couldn't allocate GameView!\n");
		exit(EXIT_FAILURE);
	}

	return new;
}

void GvFree(GameView gv)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	free(gv);
}

////////////////////////////////////////////////////////////////////////
// Game State Information

Round GvGetRound(GameView gv)
{
	return ((gv->turn++) / TOTAL_PLAYERS);
}

Player GvGetPlayer(GameView gv)
{
	return ((gv->turn++) % TOTAL_PLAYERS);
}

int GvGetScore(GameView gv)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	return 0;
}

int GvGetHealth(GameView gv, Player player)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	return 0;
}

PlaceId GvGetPlayerLocation(GameView gv, Player player)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	return NOWHERE;
}

PlaceId GvGetVampireLocation(GameView gv)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	return NOWHERE;
}

PlaceId *GvGetTrapLocations(GameView gv, int *numTraps)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	*numTraps = 0;
	return NULL;
}

////////////////////////////////////////////////////////////////////////
// Game History

PlaceId *GvGetMoveHistory(GameView gv, Player player,
                          int *numReturnedMoves, bool *canFree)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	*numReturnedMoves = 0;
	*canFree = false;
	return NULL;
}

PlaceId *GvGetLastMoves(GameView gv, Player player, int numMoves,
                        int *numReturnedMoves, bool *canFree)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	*numReturnedMoves = 0;
	*canFree = false;
	return NULL;
}

PlaceId *GvGetLocationHistory(GameView gv, Player player,
                              int *numReturnedLocs, bool *canFree)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	*numReturnedLocs = 0;
	*canFree = false;
	return NULL;
}

PlaceId *GvGetLastLocations(GameView gv, Player player, int numLocs,
                            int *numReturnedLocs, bool *canFree)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	*numReturnedLocs = 0;
	*canFree = false;
	return 0;
}

////////////////////////////////////////////////////////////////////////
// Making a Move

PlaceId *GvGetReachable(GameView gv, Player player, Round round,
                        PlaceId from, int *numReturnedLocs)
{	
	*numReturnedLocs = 0;
	PlaceId *reachableLocations = NULL;
	for (int i = 0; i < TOTAL_PLAYERS; i++){
		if (i == ST_JOSEPH_AND_ST_MARY && player == PLAYER_DRACULA) continue;
		if (gv->edges[from][i].road 
			|| gv->edges[from][i].sea 
			|| (player != PLAYER_DRACULA 
				&& gv->edges[from][i].rail 
				&& gv->edges[from][i].rail <= round % 4)) {
				(*numReturnedLocs)++;
				reachableLocations = realloc(reachableLocations, sizeof(PlaceId));
				reachableLocations[(*numReturnedLocs) - 1] = i;
		}
	}
	return reachableLocations;
}

PlaceId *GvGetReachableByType(GameView gv, Player player, Round round,
                              PlaceId from, bool road, bool rail,
                              bool boat, int *numReturnedLocs)
{
	*numReturnedLocs = 0;*numReturnedLocs = 0;
	PlaceId *reachableLocations = NULL;
	for (int i = 0; i < TOTAL_PLAYERS; i++){
		if (i == ST_JOSEPH_AND_ST_MARY && player == PLAYER_DRACULA) continue;
		if ((road && gv->edges[from][i].road)
			|| (sea && gv->edges[from][i].road)
			|| (player != PLAYER_DRACULA 
				&& rail 
				&& gv->edges[from][i].rail 
				&& gv->edges[from][i].rail <= round % 4)) {
				(*numReturnedLocs)++;
				reachableLocations = realloc(reachableLocations, sizeof(PlaceId));
				reachableLocations[(*numReturnedLocs) - 1] = i;
		}
	}
	return NULL;
}

////////////////////////////////////////////////////////////////////////
// Your own interface functions

// TODO
