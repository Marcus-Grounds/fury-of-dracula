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
#define PLAY_SIZE 7
// add your own #includes here

// Helper Function Declarations: //TODO: make static or move this to GameView.h later?
void initPlayers(GameView gv);
void initScoreTurn(GameView gv);
void initTraps(GameView gv);

void storePastPlays(GameView gv, char *pastPlays);
void storeTraps(GameView gv, char *pastPlays);
void storeMoveHistory(GameView gv, char *play, Player player);
void updatePlayerHealth(GameView gv, char *pastPlays, char *play, Player player);
void updateGameScore(GameView gv, char *play, Player player);
void vampireActivity(GameView gv, char *play, Player player);

Player initialToPlayer(char initial);
PlaceId locationOfHide(PlaceId *moveHistory, int index, PlaceId currMove);
PlaceId locationOfDoubleBack(PlaceId *moveHistory, int index, PlaceId currMove);
static int placeIdCmp(PlaceId x, PlaceId y);

// TODO: ADD YOUR OWN STRUCTS HERE
typedef struct playerData {
	PlaceId *history; // Move history of player
	int historyCount;	
	int health;
} PlayerData;

typedef struct traps {	
	PlaceId *locations;
	int trapCount;
} Traps;

struct gameView {
	// TODO: ADD FIELDS HERE
	PlayerData players[NUM_PLAYERS];
	PlaceId immatureVampLocation;
	Map places;
	Traps traps;

	int turn;
	int score;
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
	initScoreTurn(new);
	initPlayers(new);
	initTraps(new);
	new->immatureVampLocation = NOWHERE;

	storePastPlays(new, pastPlays);
	storeTraps (new, pastPlays);

	return new;
}

void GvFree(GameView gv)
{
	MapFree(gv->places);
	/*
	for (int i = 0; i < NUM_PLAYERS; i++) {
		PlaceId *hist = (gv->players[i]).history;
		free(hist);
	}
	free(gv->players);
	*/
	free(gv);
}

////////////////////////////////////////////////////////////////////////
// Game State Information

Round GvGetRound(GameView gv)
{
	return ((gv->turn) / TOTAL_PLAYERS);

}

Player GvGetPlayer(GameView gv)
{
	return ((gv->turn) % TOTAL_PLAYERS);
}

int GvGetScore(GameView gv)
{
	return gv->score;
}

int GvGetHealth(GameView gv, Player player)
{
	return gv->players[player].health;
}

PlaceId GvGetPlayerLocation(GameView gv, Player player)
{
	//If a turn has not been made, return NOWHERE
	if (gv->players[player].historyCount == 0) return NOWHERE;

	int moveCount = gv->players[player].historyCount;
	PlaceId *moveHistory = (gv->players[player]).history;
	PlaceId currMove = moveHistory[moveCount - 1];


	// If the player is a hunter
	if (player != PLAYER_DRACULA) {
		if (gv->players[player].health <= 0) {
			return ST_JOSEPH_AND_ST_MARY;

		}
		return currMove;

		//If the player is the Dracula
	} else {
		PlaceId location;
		if (currMove == TELEPORT) {
			return CASTLE_DRACULA;
		} else if (currMove == HIDE) {
			location = locationOfHide(moveHistory, moveCount - 1, currMove);
		} else if ((currMove) >= DOUBLE_BACK_1 && (currMove <= DOUBLE_BACK_5)) {
			location = locationOfDoubleBack(moveHistory, moveCount - 1, currMove);
		} else {
			location = currMove;
		}
		
		return location;
	}
}

PlaceId GvGetVampireLocation(GameView gv)
{
	return gv->immatureVampLocation;
}

PlaceId *GvGetTrapLocations(GameView gv, int *numTraps)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	*numTraps = gv->traps.trapCount;
	return gv->traps.locations;
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
			gv->players[PLAYER_DRACULA].historyCount = 1;
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

	*numReturnedLocs = numLocs;
	*canFree = true;
	return lastLocs;
}

////////////////////////////////////////////////////////////////////////
// Making a Move

//todo: comment description for function
bool isRepeat(PlaceId *reachableLocations, PlaceId newLocation, int *numReturnedLocs) {
	for (int i = 0; i < (*numReturnedLocs); i++) {
		if (reachableLocations[i] == newLocation) return true;
	}
	return false;
}

// todo: comment description for function
PlaceId *addPlaceId(PlaceId new, PlaceId *reachableLocations,
					int *numReturnedLocs) {
	(*numReturnedLocs)++;
	reachableLocations = realloc(reachableLocations, 
								 (*numReturnedLocs) * sizeof(PlaceId));
	assert(reachableLocations != NULL);
	reachableLocations[(*numReturnedLocs) - 1] = new;
	return reachableLocations;
}

// todo: comment description for function
PlaceId *getConnectionsByRail(GameView gv, PlaceId from, PlaceId intermediate, 
							  PlaceId *reachableLocations, int maxRailDistance, 
							  int distance, int *numReturnedLocs) {

	if (maxRailDistance < distance) return reachableLocations;

	ConnList intermediateConns = MapGetConnections(gv->places, intermediate);
	for (ConnList curr = intermediateConns; curr != NULL; curr = curr->next) {
		if (curr->type == RAIL && !isRepeat(reachableLocations, curr->p, numReturnedLocs)) {
			reachableLocations = addPlaceId(curr->p, reachableLocations, numReturnedLocs);
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
	
	reachableLocations = addPlaceId(from, reachableLocations, numReturnedLocs);
	ConnList allConnections = MapGetConnections(gv->places, from);
	int maxRailDistance = (player + round) % 4;
	for (ConnList curr = allConnections; curr != NULL; curr = curr->next){
		if (curr->p == HOSPITAL_PLACE && player == PLAYER_DRACULA) continue;
		if (isRepeat(reachableLocations, curr->p, numReturnedLocs)) continue;
		if (player != PLAYER_DRACULA && curr->type == RAIL && maxRailDistance > 0) {
			reachableLocations = addPlaceId(curr->p, reachableLocations, numReturnedLocs);
			reachableLocations = getConnectionsByRail(gv, from, curr->p, 
												      reachableLocations, 
													  maxRailDistance, 
													  MIN_BRANCHING_DISTANCE, 
													  numReturnedLocs);
			continue;
		}
		if (curr->type == ROAD || curr->type == BOAT)
			reachableLocations = addPlaceId(curr->p, reachableLocations, numReturnedLocs);
	}
	return reachableLocations;
}

PlaceId *GvGetReachableByType(GameView gv, Player player, Round round,
                              PlaceId from, bool road, bool rail,
                              bool boat, int *numReturnedLocs)
{
	*numReturnedLocs = 0;
	PlaceId *reachableLocations = NULL;

	reachableLocations = addPlaceId(from, reachableLocations, numReturnedLocs);
	ConnList allConnections = MapGetConnections(gv->places, from);
	int maxRailDistance = (player + round) % 4;
	for (ConnList curr = allConnections; curr != NULL; curr = curr->next) {
		if (curr->p == HOSPITAL_PLACE && player == PLAYER_DRACULA) continue;
		if (isRepeat(reachableLocations, curr->p, numReturnedLocs)) continue;
		if (player != PLAYER_DRACULA && rail && curr->type == RAIL && maxRailDistance > 0) {
			reachableLocations = addPlaceId(curr->p, reachableLocations, numReturnedLocs);
			reachableLocations = getConnectionsByRail(gv, from, curr->p, 
												      reachableLocations, 
													  maxRailDistance, 
													  MIN_BRANCHING_DISTANCE, 
													  numReturnedLocs);
			continue;
		}
		if ((road && curr->type == ROAD) || (boat && curr->type == BOAT)) {
			reachableLocations = addPlaceId(curr->p, reachableLocations, numReturnedLocs);
		}
	}
	return reachableLocations;
}

////////////////////////////////////////////////////////////////////////
// Your own interface functions

// Initialises the players[] field of the GameView data structure
void initPlayers(GameView gv) {
	for (Player curr = PLAYER_LORD_GODALMING; curr <= PLAYER_DRACULA; curr++) {
		(gv->players[curr]).historyCount = 0;
		(gv->players[curr]).history = NULL;
		(gv->players[curr]).health = GAME_START_HUNTER_LIFE_POINTS; 
	}
	(gv->players[PLAYER_DRACULA]).health = GAME_START_BLOOD_POINTS;
	return;
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
		gv->turn++;

		// Extracting information from current play & storing into GameView data structure:
		storeMoveHistory(gv, play, currPlayer);
		vampireActivity(gv, play, currPlayer);
		updatePlayerHealth (gv, pastPlays ,play, currPlayer);
		updateGameScore (gv, play, currPlayer);
	}
	free(freeTmp);
}

// Stores a player move into the move history of player (in GameView data structure)
void storeMoveHistory(GameView gv, char *play, Player player) {
	
	PlaceId *moveHistory = (gv->players[player]).history;
	//assert (moveHistory != NULL);

	char placeAbbrev[3] = {play[1], play[2], '\0'};
	PlaceId place = placeAbbrevToId(placeAbbrev);

	// Add new place into the player's move history
	int count = (gv->players[player]).historyCount;
	count++;
	moveHistory = realloc(moveHistory, (sizeof(PlaceId) * count));
	assert (moveHistory != NULL);

	moveHistory[count - 1] = place;

	(gv->players[player]).history = moveHistory;
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

//Initializing score and round
void initScoreTurn(GameView gv) {
	gv->score = GAME_START_SCORE;
	gv->turn = 0;
}	

// todo: comment description for this function
void initTraps(GameView gv) {
	//Can scan through drac history to determine size of trap store / look for 'M'
	//in actual trap store we check count value along side size of m
	gv->traps.locations = malloc(sizeof(PlaceId) * gv->players[PLAYER_DRACULA].historyCount);
	assert(gv->traps.locations != NULL);
	gv->traps.trapCount = 0;
}

// todo: comment description for this function
void storeTraps(GameView gv, char *pastPlays) {
	
	if (strcmp(pastPlays, "") == 0) return;

	PlaceId * trapLoc = gv->traps.locations;
	PlaceId * DracHist = gv->players[PLAYER_DRACULA].history;
	PlaceId * trapEnc = malloc(sizeof(PlaceId) * gv->turn);
	assert (trapLoc != NULL);
	assert (trapEnc != NULL);

	int DracHistCount = gv->players[PLAYER_DRACULA].historyCount;
	int trapCnt = 0;
	char *tmp = strdup(pastPlays);
	char *play;
	
	//CREATE A PLACEID ARRAY OF WHERE TRAPS WERE ENCOUTED
	//ALSO FINDS TRAP COUNT
	
	int x = 0;
	while ((play = strsep(&tmp, " ")) != NULL) { 
		
		if (play[0] != 'D') {
			if (play[3] == 'T') {
				char placeAbbrev[3] = {play[1], play[2], '\0'};
				PlaceId place = placeAbbrevToId(placeAbbrev);
				trapEnc[x] = place;
				x++;	
			}
			if (play[4] == 'T') {
				char placeAbbrev[3] = {play[1], play[2], '\0'};
				PlaceId place = placeAbbrevToId(placeAbbrev);
				trapEnc[x] = place;
				x++;	
			}
		} else {
			if (play[3] == 'T') trapCnt ++;
			if (play[5] == 'M') trapCnt --;	
		}
	}

	free(tmp);

	
	tmp = strdup(pastPlays);
	trapLoc = realloc(trapLoc, (sizeof(PlaceId) * trapCnt));

	int i = 0; int j = 0;
	int trapSkipCnt = DracHistCount - trapCnt;
	
	while ((play = strsep(&tmp, " ")) != NULL) {
			
		if (play[0] != 'D') continue;

		int check = 0;
		if (i >= trapSkipCnt && i < DracHistCount) { //SKIPS OVER TRAPS THAT HAVE VANISHED
			
			char placeAbbrev[3] = {play[1], play[2], '\0'};
			PlaceId place = placeAbbrevToId(placeAbbrev);

			if (place == HIDE) {
				place = locationOfHide(DracHist, i, place);
			
			} else if (place >= DOUBLE_BACK_1 && place <= DOUBLE_BACK_5) {
				place = locationOfDoubleBack(DracHist, i, place);
			}
			
			for (int i = 0; i < x; i ++) {
				
				if (placeIdCmp(place, trapEnc[i]) != 0) continue;
				
				trapEnc[i] = NOWHERE;
				trapCnt --;
				check = 1;	
				
				break;
			}		
			
			if (check != 1) {
				trapLoc[j] = place; 
				j ++;
			} 
		}
		i ++;
	}

	gv->traps.trapCount = trapCnt;	
	free(trapEnc);
	free(tmp);
}

static int placeIdCmp(PlaceId x, PlaceId y) {
	PlaceId p1 = x;
	PlaceId p2 = y;
	return p1 - p2;
}

// todo: comment description for this function
void updateGameScore(GameView gv, char *play, Player player) {
	if (player == PLAYER_DRACULA) {
		if (play[5] == 'V') {
			gv->immatureVampLocation = NOWHERE;
			gv->score = gv->score - SCORE_LOSS_VAMPIRE_MATURES;
		}
		gv->score = gv->score - SCORE_LOSS_DRACULA_TURN;
	} else {
		if ((gv->players[player]).health <= 0) {
			gv->score = gv->score - SCORE_LOSS_HUNTER_HOSPITAL;
		}
	}
}

// todo: comment description for this function
void updatePlayerHealth(GameView gv, char *pastPlays, char *play, Player player) {
	
	int health = (gv->players[player]).health;
	int histCount = (gv->players[player]).historyCount;
	PlaceId *playerHistory = (gv->players[player]).history;
	
	if (player != PLAYER_DRACULA) {

		if (health <= 0) {
			health = GAME_START_HUNTER_LIFE_POINTS; 
		}
	
		for (int i = 3; i < PLAY_SIZE; i++) {
			if (play[i] == 'T') {
				health = health - LIFE_LOSS_TRAP_ENCOUNTER;
			}
			if (play[i] == 'D') {
				health = health - LIFE_LOSS_DRACULA_ENCOUNTER;	
				gv->players[PLAYER_DRACULA].health = gv->players[PLAYER_DRACULA].health - LIFE_LOSS_HUNTER_ENCOUNTER;
			}
		}

		if (playerHistory[histCount - 1] == playerHistory[histCount - 2]) {
			health = health + LIFE_GAIN_REST; 
				
			if (health > GAME_START_HUNTER_LIFE_POINTS) {
				health = GAME_START_HUNTER_LIFE_POINTS;
			}
		}

	} else {
		//COUNTER FOR DRACULA ENCOUNTER AND DRACULA AT CASTLE
		
		PlaceId currLoc = GvGetPlayerLocation(gv, PLAYER_DRACULA);
		for (int i = 0; i < histCount; i++) {
		}

		//HEALTH LOSS AT SEA/ HEALTH GAIN AT CASTLE
		if (placeIdToType(currLoc) == SEA) {
			health = health - LIFE_LOSS_SEA;
		} else if (currLoc == CASTLE_DRACULA) {
			health = health + LIFE_GAIN_CASTLE_DRACULA;
		}
	}	
	if (health < 0) health = 0;
	(gv->players[player]).health = health;
}

// Checks if an immature vampire was placed or vanquished and updates field 
// in gameView struct
void vampireActivity(GameView gv, char *play, Player player) {
	if (player == PLAYER_DRACULA) {
		if (play[4] == 'V') {
			// Immature vampire is placed
			
			char placeAbbrev[3] = {play[1], play[2], '\0'};
			PlaceId place = placeAbbrevToId(placeAbbrev);
			gv->immatureVampLocation = place;
		}
	} else {
		for (int i = 3; i < PLAY_SIZE; i++) {
			if (play[i] == 'V') {
				// Immature vampire is vanquished
				gv->immatureVampLocation = NOWHERE;
			}
		}
	}
	return;
}