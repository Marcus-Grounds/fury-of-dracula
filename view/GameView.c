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

#define TOTAL_PLAYERS 5
#define TOTAL_PLACES 71
#define MIN_BRANCHING_DISTANCE 2
#define INITIAL_GAME_SCORE 366
// add your own #includes here

// Helper Function Declarations: //TODO: make static or move this to GameView.h later?
void initialisePlayers(GameView gv);
PlaceId *newMoveHistory(void);
void storePastPlays(GameView gv, char *pastPlays);
void storeMoveHistory(GameView gv, char *play, Player player);
void updateGameScore(GameView gv, char *play, Player player);
Player initialToPlayer(char initial);
PlaceId locationOfHide(PlaceId *moveHistory, int index, PlaceId currMove);
PlaceId locationOfDoubleBack(PlaceId *moveHistory, int index, PlaceId currMove);

// TODO: ADD YOUR OWN STRUCTS HERE
typedef struct playerData {
	int historyCount;
	PlaceId *history; // Move history of player
	int health;
} PlayerData;

struct gameView {
	// TODO: ADD FIELDS HERE
	PlayerData *players;
	Map places;
	int score;
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
	new->places = MapNew();
	new->players = malloc(NUM_PLAYERS * sizeof (PlayerData));
	if (new->players == NULL) {
		fprintf(stderr, "Couldn't allocate GameView!\n");
		exit(EXIT_FAILURE);
	}
	new->score = INITIAL_GAME_SCORE;
	new->turn = 0;
	initialisePlayers(new);
	storePastPlays(new, pastPlays);

	return new;
}

void GvFree(GameView gv)
{
	MapFree(gv->places);
	for (int i = 0; i < NUM_PLAYERS; i++) {
		PlaceId *hist = (gv->players[i]).history;
		free(hist);
	}
	free(gv->players);
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
	return gv->score;
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
	*numReturnedMoves = (gv->players[player]).historyCount;
	*canFree = false;
	return (gv->players[player]).history;
}

PlaceId *GvGetLastMoves(GameView gv, Player player, int numMoves,
                        int *numReturnedMoves, bool *canFree)
{
	if (numMoves >= (gv->players[player]).historyCount) {
		return GvGetMoveHistory(gv, player, numReturnedMoves, canFree);
	}

	PlaceId *moveHistory = (gv->players[player]).history;
	PlaceId *lastMoves = malloc(sizeof(PlaceId) * numMoves);
	assert (lastMoves != NULL);

	// Copy the last 'numMoves' moves of moveHistory into 'lastMoves'
	int startIndex = (gv->players[player]).historyCount - numMoves;
	for (int i = 0; i < numMoves; i++) {
		lastMoves[i] = moveHistory[startIndex + i];
	}

	*numReturnedMoves = numMoves;
	*canFree = true;
	return lastMoves;
}

PlaceId *GvGetLocationHistory(GameView gv, Player player,
                              int *numReturnedLocs, bool *canFree)
{
	if (player != PLAYER_DRACULA) {
		// TODO: dont have to care abt teleporting to hospital?
		return GvGetMoveHistory(gv, player, numReturnedLocs, canFree);
	}

	PlaceId *moveHistory = (gv->players[player]).history;
	int moveCount = (gv->players[player]).historyCount;

	PlaceId *locHistory = malloc(sizeof(PlaceId) * moveCount);
	assert (locHistory != NULL);

	for (int i = 0; i < moveCount; i++) {
		PlaceId currMove = moveHistory[i];

		if (currMove == TELEPORT) {
			locHistory[i] = CASTLE_DRACULA;
		} else if (currMove == HIDE) {
			locHistory[i] = locationOfHide(moveHistory, i, currMove);
		} else if ((currMove >= DOUBLE_BACK_1) && (currMove <= DOUBLE_BACK_5)) {
			locHistory[i] = locationOfDoubleBack(moveHistory, i, currMove);
		} else {
			locHistory[i] = currMove;
		}
	}
	*numReturnedLocs = moveCount;
	*canFree = true;
	return locHistory;
}

PlaceId *GvGetLastLocations(GameView gv, Player player, int numLocs,
                            int *numReturnedLocs, bool *canFree)
{
	PlaceId *locHistory = GvGetLocationHistory(gv, player, numReturnedLocs, canFree);
	if (numLocs >= (*numReturnedLocs)) {
		return locHistory;
	}

	PlaceId *lastLocs = malloc(sizeof(PlaceId) * numLocs);
	assert (lastLocs != NULL);

	// Copy the last 'numLocs' locations of locHistory into 'lastLocs'
	int startIndex = (*numReturnedLocs) - numLocs;
	for (int i = 0; i < numLocs; i++) {
		lastLocs[i] = locHistory[startIndex + i];
	}
	free(locHistory);
	
	*numReturnedLocs = numLocs;
	*canFree = true;
	return lastLocs;
}

////////////////////////////////////////////////////////////////////////
// Making a Move

PlaceId *addPlaceId(PlaceId new, PlaceId *reachableLocations,
					int *numReturnedLocs) {
	(*numReturnedLocs)++;
	reachableLocations = realloc(reachableLocations, 
								 (*numReturnedLocs) * sizeof(PlaceId));
	reachableLocations[(*numReturnedLocs) - 1] = new;
	return reachableLocations;
}

PlaceId *getConnectionsByRail(GameView gv, PlaceId from, PlaceId intermediate, 
							  PlaceId *reachableLocations, int maxRailDistance, 
							  int distance, int *numReturnedLocs) {

	if (maxRailDistance < distance) return reachableLocations;
	ConnList intermediateConns = MapGetConnections(gv->places, intermediate);
	for (ConnList curr = intermediateConns; curr != NULL; curr = curr->next) {
		if (curr->p == from) continue;
		if (curr->type == RAIL) {
			addPlaceId(curr->p, reachableLocations, numReturnedLocs);
			reachableLocations = getConnectionsByRail(gv, from, curr->p, 
								 		        	  reachableLocations,
													  maxRailDistance,
													  distance + 1, 
													  numReturnedLocs);
		}
	}
	return reachableLocations;
}

PlaceId *GvGetReachable(GameView gv, Player player, Round round,
                        PlaceId from, int *numReturnedLocs)
{	
	*numReturnedLocs = 0;
	PlaceId *reachableLocations = NULL;
	ConnList allConnections = MapGetConnections(gv->places, from);
	int maxRailDistance = (player + round) % 4;
	for (ConnList curr = allConnections; curr != NULL; curr = curr->next){
		if (curr->p == HOSPITAL_PLACE && player == PLAYER_DRACULA) continue;
		if (curr->type == ROAD || curr->type == BOAT) {
			addPlaceId(curr->p, reachableLocations, numReturnedLocs);
		}
		if (player == PLAYER_DRACULA || maxRailDistance == 0) continue;
		if (curr->type == RAIL) {
			addPlaceId(curr->p, reachableLocations, numReturnedLocs);
			reachableLocations = getConnectionsByRail(gv, from, curr->p, 
												      reachableLocations, 
													  maxRailDistance, 
													  MIN_BRANCHING_DISTANCE, 
													  numReturnedLocs);
		}
	}
	return reachableLocations;
}

PlaceId *GvGetReachableByType(GameView gv, Player player, Round round,
                              PlaceId from, bool road, bool rail,
                              bool boat, int *numReturnedLocs)
{
	*numReturnedLocs = 0;
	PlaceId *reachableLocations = NULL;
	ConnList allConnections = MapGetConnections(gv->places, from);
	int maxRailDistance = (player + round) % 4;
	for (ConnList curr = allConnections; curr != NULL; curr = curr->next){
		if (curr->p == HOSPITAL_PLACE && player == PLAYER_DRACULA) continue;
		if ((road && curr->type == ROAD) || (boat && curr->type == BOAT)) {
			addPlaceId(curr->p, reachableLocations, numReturnedLocs);
		}
		if (player == PLAYER_DRACULA || maxRailDistance == 0) continue;
		if (rail && curr->type == RAIL) {
			addPlaceId(curr->p, reachableLocations, numReturnedLocs);
			reachableLocations = getConnectionsByRail(gv, from, curr->p, 
												      reachableLocations, 
													  maxRailDistance, 
													  MIN_BRANCHING_DISTANCE, 
													  numReturnedLocs);
		}
	}
	return reachableLocations;
}

////////////////////////////////////////////////////////////////////////
// Your own interface functions

// TODO

// Initialises the players field of the GameView data structure
void initialisePlayers(GameView gv) {
	for (Player curr = PLAYER_LORD_GODALMING; curr <= PLAYER_DRACULA; curr++) {
		(gv->players[curr]).historyCount = 0;
		(gv->players[curr]).history = newMoveHistory(); 
		(gv->players[curr]).health = 9; 
	}
	(gv->players[PLAYER_DRACULA]).health = 40;
	return;
}

// Creates new empty array to store player move history
PlaceId *newMoveHistory(void) {
	PlaceId *new = malloc(sizeof(*new)); 

	if (new == NULL) {
		fprintf(stderr, "Couldn't allocate move history array\n");
		exit(EXIT_FAILURE);
	}
	return new;
}

// TODO: need to free move history (function?)


// Stores data from pastPlays into the GameView data structure
void storePastPlays(GameView gv, char *pastPlays) {
	if (strcmp(pastPlays, "") == 0) return;

	// Extracting each play from pastPlays string with strsep
	char *freeTmp = strdup(pastPlays);
	char *tmp = freeTmp;

	char *play;
	Player currPlayer;
	while ((play = strsep(&tmp, " ")) != NULL) {
		currPlayer = initialToPlayer(play[0]);

		// Extracting information from current play & storing into GameView data structure:
		storeMoveHistory(gv, play, currPlayer);
		updateGameScore (gv, play, currPlayer);

		// TODO: add in health info, traps, encounters etc
		
	}
	free(freeTmp);
}

// Stores a player move into the move history of player (in GameView data structure)
void storeMoveHistory(GameView gv, char *play, Player player) {
	PlaceId *moveHistory = (gv->players[player]).history;
	assert (moveHistory != NULL);

	char placeAbbrev[3] = {play[1], play[2], '\0'};
	PlaceId place = placeAbbrevToId(placeAbbrev);

	// Add new place into the player's move history
	int count = (gv->players[player]).historyCount;
	count++;
	moveHistory = realloc(moveHistory, (sizeof(PlaceId) * count));
	assert (moveHistory != NULL);

	moveHistory[count - 1] = place;
	(gv->players[player]).historyCount = count;
}

// Returns a player given their initial
Player initialToPlayer(char initial) {
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
			fprintf(stderr, "Initial is not a valid player\n"); 
			exit(EXIT_FAILURE);
	}
}

// Resolves location of Dracula's HIDE move to a city or sea
PlaceId locationOfHide(PlaceId *moveHistory, int index, PlaceId currMove) {

	if (currMove != HIDE) return currMove;
	// Check if the resolved location of hide is a doubleback move
	return locationOfDoubleBack(moveHistory, index - 1, moveHistory[index - 1]);
}

// Resolves location of Dracula's DOUBLE_BACK_n move to a city or sea
PlaceId locationOfDoubleBack(PlaceId *moveHistory, int index, PlaceId currMove) {

	// Doubling back and checking if the resolved location is a hide move
	switch(currMove) {
		case DOUBLE_BACK_1:
			return locationOfHide(moveHistory, index - 1, moveHistory[index - 1]);
		case DOUBLE_BACK_2:
			return locationOfHide(moveHistory, index - 2, moveHistory[index - 2]);
		case DOUBLE_BACK_3:
			return locationOfHide(moveHistory, index - 3, moveHistory[index - 3]);
		case DOUBLE_BACK_4:
			return locationOfHide(moveHistory, index - 4, moveHistory[index - 4]);
		case DOUBLE_BACK_5:
			return locationOfHide(moveHistory, index - 5, moveHistory[index - 5]);
		default: 
			// Move given is not a double back move
			return currMove;
	}
}

void updateGameScore(GameView gv, char *play, Player player) {
	if (player == PLAYER_DRACULA) {
		if (play[5] == 'V') {
			gv->score = gv->score - 13;
		}
		gv->score --;
	} else {
		if ((gv->players[player]).health <= 0) {
			gv->score = gv->score - 6;
		}
	}
}
	