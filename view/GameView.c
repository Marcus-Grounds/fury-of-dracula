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
#include <string.h>

#include "Game.h"
#include "GameView.h"
#include "Map.h"
#include "Places.h"
// add your own #includes here

// Helper Function Declarations:
void initialisePlayers(GameView gv);
PlaceId *newMoveHistory(void);
void storePastPlays(GameView gv, char *pastPlays);
void storeMoveHistory(GameView gv, char *play, Player player);
Player initialToId(char initial);

// TODO: ADD YOUR OWN STRUCTS HERE

typedef struct playerData {
	int historyCount;
	PlaceId *history; // Move history of player
	int health;
} PlayerData;

struct gameView {
	// TODO: ADD FIELDS HERE
	int nTurns;
	PlayerData players[NUM_PLAYERS];
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
	initialisePlayers(new);


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
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	return 0;
}

Player GvGetPlayer(GameView gv)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	return PLAYER_LORD_GODALMING;
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

	*canFree = false;
	*numReturnedMoves = (gv->players[player]).historyCount;
	return (gv->players[player]).history;
}

PlaceId *GvGetLastMoves(GameView gv, Player player, int numMoves,
                        int *numReturnedMoves, bool *canFree)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	//*numReturnedMoves = 0;
	// *canFree = false;
	// return NULL;
	if (numMoves >= (gv->players[player]).historyCount) {
		return GvGetMoveHistory(gv, player, numReturnedMoves, canFree);
	}
	// else: // TODO
	*canFree = true;
	return NULL;
}

PlaceId *GvGetLocationHistory(GameView gv, Player player,
                              int *numReturnedLocs, bool *canFree)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	/* *numReturnedLocs = 0;
	*canFree = false;
	return NULL; */
	if (player != PLAYER_DRACULA) {
		return GvGetMoveHistory(gv, player, numReturnedLocs, canFree);
	}
	// Else: player is dracula // TODO
	// need to traverse through moveHistory array again 
	// after already doing it in storePastPlays
	// - not time efficient? test to see

	*canFree = true;
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
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	*numReturnedLocs = 0;
	return NULL;
}

PlaceId *GvGetReachableByType(GameView gv, Player player, Round round,
                              PlaceId from, bool road, bool rail,
                              bool boat, int *numReturnedLocs)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	*numReturnedLocs = 0;
	return NULL;
}

////////////////////////////////////////////////////////////////////////
// Your own interface functions

// TODO

// TODO: need comment that describes functions
void initialisePlayers(GameView gv) {
	for (Player curr = PLAYER_LORD_GODALMING; curr <= PLAYER_DRACULA; curr++) {
		(gv->players[curr]).historyCount = 0;
		(gv->players[curr]).history = newMoveHistory(); 
		(gv->players[curr]).health = 0;
	}
	return;
}

// creates new empty History 
PlaceId *newMoveHistory(void) {
	PlaceId *new = malloc(sizeof(*new)); //todo: sizeof PlaceId or *new?
	if (new == NULL) {
		fprintf(stderr, "Couldn't allocate move history array!\n"); //TODO check msg
		exit(EXIT_FAILURE);
	}
	return new;
}

// TODO: need function to free move history?


// store move history of players into gameview struct
void storePastPlays(GameView gv, char *pastPlays) {
	// store move history:
	
	/* int count = 0;
	PlaceId *moveHistory = NULL;*/

	// tokenise pastPlays string with strsep
	char *freeTmp = strdup(pastPlays);
	char *tmp = freeTmp;

	Player currPlayer;
	char *play;
	while ((play = strsep(&tmp, " ")) != NULL) {
		// extract information from current play & store into gv
		// play is a string (7 chars long)

		/* is it time efficient to check which player is moving for every move?
			^(using initialToId)
			since if they take turns moving -> currPlayer is enough?
			so only need one: dont need initialToId or dont need currPlayer
			useful for debug for now to check that currPlayer is giving right player? */
		assert(initialToId(play[0]) == currPlayer);

		storeMoveHistory(gv, play, currPlayer);
		// TODO: add in health info, traps, encounters etc
		currPlayer++;
	}
	free(freeTmp);
}

void storeMoveHistory(GameView gv, char *play, Player player) {
	// TODO
	assert ((gv->players[player]).history != NULL);

	char placeAbbrev[3]; //TODO: abbrev len 2? or 3 for null terminator? is that included
	strncpy(placeAbbrev, play + 1, 2); // check: need to define numbers for readability?
	placeAbbrev[2] = '\0'; //TODO: is this necessary? 
	// printf("DEBUG: placeAbbrev is %s\n", placeAbbrev);
	// PlaceId place = placeAbbrevToId(placeAbbrev);
	// TODO: Incomplete
	

}

// Test function for storePastPlays() //TODO
Player initialToId(char initial) {
	switch(initial) {
		case 'G':
			return PLAYER_LORD_GODALMING;
		case 'S':
			return PLAYER_DR_SEWARD;
		case 'H':
			return PLAYER_VAN_HELSING;
		case 'M':
			return PLAYER_MINA_HARKER;
		case 'D':
			return PLAYER_DRACULA;
		default:
			fprintf(stderr, "Initial is not a valid player!\n"); //TODO check msg
			exit(EXIT_FAILURE);
	}
}