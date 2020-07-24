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
#define HUNTER 0
#define DRACULA 1
#define PLAY_SIZE 7
// add your own #includes here

// Helper Function Declarations: //TODO: make static or move this to GameView.h later?
void initialisePlayers(GameView gv);
PlaceId *newMoveHistory(void);
void storePastPlays(GameView gv, char *pastPlays);
void storeMoveHistory(GameView gv, char *play, Player player);
Player initialToPlayer(char initial);
PlaceId locationOfHide(PlaceId *moveHistory, int index, PlaceId currMove);
PlaceId locationOfDoubleBack(PlaceId *moveHistory, int index, PlaceId currMove);
void InitScoreRound(GameView gv);
void PlayerInitLocation(GameView gv, char *pastPlays);
PlaceId GetCurrentLocation(GameView gv, Player player);
bool IsDracrRevealed(GameView gv, Player player);
PlaceId vampirePlaced(GameView gv);
bool vampireMatures(GameView gv);
bool vampireVanquished(GameView gv, Player player);
void initVampires(GameView gv);
void initTraps(GameView gv) ;
PlaceId *setTraps(GameView gv);
void updateGameScore(GameView gv, char *play, Player player);
void vampireInGeneral(GameView gv, Player player);
void encounterDrac(GameView gv, Player player);
void updatePlayerHealth(GameView gv, char *pastPlays, char *play, Player player);
// TODO: ADD YOUR OWN STRUCTS HERE

typedef struct travel {
	int rail;
	bool road;
	bool sea;
} Travel;

typedef struct vampire {

	bool is_revealed;
	bool is_alive;
	bool has_matured;
	PlaceId location;


}Vampire;

typedef struct playerData {
	int historyCount;
	PlaceId *history; // Move history of player
	int health;
	PlaceId location;
	int role;
} PlayerData;

typedef struct traps {

	int num;
	int *locations;

} Traps;

struct gameView {
	// TODO: ADD FIELDS HERE
	PlayerData players[NUM_PLAYERS];
	Traps traps;
	Vampire vampires;
	Map places;
	char *plays;
	int turn;
	int score;
	int round;
	bool made_turn;
	bool if_drac_isrev;
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
	
	InitScoreRound(new);
	initialisePlayers(new);
	initTraps(new);
	storePastPlays(new, pastPlays);
	int last =  new->players[PLAYER_LORD_GODALMING].historyCount;

	for (int curr = 0; curr < last; curr++) {
		printf("LOCATION_STR: %d\n",new->players[PLAYER_LORD_GODALMING].history[curr]);
	}
	initVampires(new);
	return new;
}

void GvFree(GameView gv)
{
	//TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION

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
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	//int round = GvGetRound(gv); 
	return gv->score;
}

int GvGetHealth(GameView gv, Player player)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	return gv->players[player].health;
}

PlaceId GvGetPlayerLocation(GameView gv, Player player)
{
	
	if (gv->players[player].historyCount == 0) return NOWHERE;

	//PlaceId location;
	PlaceId last_move = gv->players[player].historyCount;

	if (player % 4 == 0 && player != 0) {
		gv->players[player].role = DRACULA;
	} else {
		gv->players[player].role = HUNTER;
	}

	PlaceId location = GetCurrentLocation(gv, player);
	//If a turn has not been made, return NOWHERE
	//If the player is a hunter
	if (gv->players[player].role == HUNTER) {
		if (gv->players[player].health <= 0) {
			gv->players[player].location = ST_JOSEPH_AND_ST_MARY;
			gv->players[player].history[last_move - 1] = ST_JOSEPH_AND_ST_MARY;
			return gv->players[player].location;

		}

		return location;
	
		//If the player is the Dracula
	} else if (gv->players[player].role == DRACULA) {
		
		//If the dracula has been revealed
		if (IsDracrRevealed(gv, player) == true) {
			return location;
		}
		//If Dracula's last move was a TELEPORT
		if ( gv->players[player].history[last_move - 1] == TELEPORT) {
			 gv->players[player].history[last_move - 1] = CASTLE_DRACULA;
			return CASTLE_DRACULA;
		} 

		PlaceType location_type = placeIdToType(location);
		if (location_type == SEA) {
			return SEA_UNKNOWN;
		} else if (location_type == LAND) {
			return CITY_UNKNOWN;
		}

		if (location == 103) return locationOfDoubleBack(gv->players[player].history, 1, gv->players[player].location);
		if (location == 104) return locationOfDoubleBack(gv->players[player].history, 2, gv->players[player].location);
		if (location == 105) return locationOfDoubleBack(gv->players[player].history, 3, gv->players[player].location);
		if (location == 106) return locationOfDoubleBack(gv->players[player].history, 4, gv->players[player].location);
		if (location == 107) return locationOfDoubleBack(gv->players[player].history, 5, gv->players[player].location);
				
		
		

	}	
	return NOWHERE;
}

PlaceId GvGetVampireLocation(GameView gv)
{
	
	PlaceId location = gv->vampires.location;

	printf("VAMP HERE::%d\n\n", gv->vampires.is_alive);
	if (vampireVanquished(gv, GvGetPlayer(gv)) == true) return NOWHERE;
	Vampire vamp = gv->vampires;


	if (location == NOWHERE) {
		return NOWHERE;
	}

	if (vamp.has_matured == true) {
		return NOWHERE;
	}

	if (gv->if_drac_isrev == true){
		return gv->vampires.location;
	} else {
		return CITY_UNKNOWN;
	}

	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	return location;
}

PlaceId vampirePlaced(GameView gv) {

	int round = GvGetRound(gv);
	PlaceId location = GvGetPlayerLocation(gv, PLAYER_DRACULA);
	if (round == 1 || round % 13 == 0) {
		if (placeIdToType(GetCurrentLocation(gv, PLAYER_DRACULA) != SEA)) {
			gv->vampires.location = location;
			gv->vampires.is_alive = true;
			gv->vampires.has_matured = false;
			//printf("VAMP placed HERE::%d\n\n", location);
			return gv->vampires.location;
		}

	} 
	
	if (gv->vampires.is_alive == true) return gv->vampires.location;

	return NOWHERE;
}

PlaceId *GvGetTrapLocations(GameView gv, int *numTraps)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	*numTraps = gv->traps.num;

	PlaceId *locations = setTraps(gv);
	
	return locations;
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

// Initialises the players[] field of the GameView data structure
void initialisePlayers(GameView gv) {
	for (Player curr = PLAYER_LORD_GODALMING; curr <= PLAYER_DRACULA; curr++) {
		(gv->players[curr]).historyCount = 0;
		(gv->players[curr]).history = newMoveHistory(); 
		(gv->players[curr]).health = GAME_START_HUNTER_LIFE_POINTS; 
	}
	(gv->players[PLAYER_DRACULA]).health = GAME_START_BLOOD_POINTS;
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

		gv->turn++;
		gv->made_turn = true;
		// Extracting information from current play & storing into GameView data structure:
		gv->round = GvGetRound(gv);
		IsDracrRevealed(gv, currPlayer);
		vampireInGeneral(gv, currPlayer);
		//printf("\n \nVAMP/IS ALIVE%d\n\n", gv->vampires.is_alive);
		storeMoveHistory(gv, play, currPlayer);
		//printf("GV::: %d\n\n", gv->players[PLAYER_LORD_GODALMING].location);
		//encounterDrac(gv, currPlayer);
		updatePlayerHealth (gv, pastPlays ,play, currPlayer);
		updateGameScore (gv, play, currPlayer);
		//printf("ROUND: %d PLAYER: %d -- LOCATION: %d  -- DRAC:%d\n  ", GvGetRound(gv), currPlayer, GvGetPlayerLocation(gv, currPlayer), GvGetPlayerLocation(gv, PLAYER_DRACULA));
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

//Initializing score and round
void InitScoreRound(GameView gv) {
	gv->score = GAME_START_SCORE;
	gv->round = 1;
	gv->turn = 0;
	gv->made_turn = true;
}	

PlaceId GetCurrentLocation(GameView gv, Player player) {

	int last = gv->players[player].historyCount;


	if (gv->players[player].history[last-1] == 103 || gv->players[player].history[last-1] == 102 ) {

		gv->players[player].history[last - 1] = gv->players[player].history[last - 2];		 
	} else if (gv->players[player].history[last-1] == 104) {

		gv->players[player].history[last - 1] = gv->players[player].history[last - 3];		 
	} else if (gv->players[player].history[last-1] == 105) {

		gv->players[player].history[last - 1] = gv->players[player].history[last - 4];		 
	} else if (gv->players[player].history[last-1] == 106) {

		gv->players[player].history[last - 1] = gv->players[player].history[last - 5];

	} else if (gv->players[player].history[last-1] == 107) {
		gv->players[player].history[last - 1] = gv->players[player].history[last - 6];
	}
	
	gv->players[player].location = gv->players[player].history[last-1];
	return gv->players[player].location;
}

bool IsDracrRevealed(GameView gv, Player player) {

	PlaceId Drac_location = gv->players[PLAYER_DRACULA].location; 
	if (Drac_location == CASTLE_DRACULA) {
		gv->if_drac_isrev = true;
	}
	
	for (int curr = PLAYER_LORD_GODALMING; curr < PLAYER_DRACULA; curr++) {
		if (GetCurrentLocation(gv, curr) == Drac_location) {
			gv->if_drac_isrev = true;
		}
	}

	return gv->if_drac_isrev;
}

bool vampireMatures(GameView gv) {

	int round = GvGetRound(gv);
	
	if (gv->vampires.is_alive) {
		if (round % 6 == 0 && round != 0 ) {
			gv->score = gv->score - 13;
			gv->vampires.has_matured = true;
			gv->vampires.is_alive = false;
			gv->vampires.location = NOWHERE;
			return false;
		}
	}

	return true;
}

bool vampireVanquished(GameView gv, Player player) {

	PlaceId location = gv->vampires.location;
	if (location != NOWHERE && gv->vampires.is_alive == true && gv->vampires.has_matured == false) {
		for (Player curr = PLAYER_LORD_GODALMING; curr < PLAYER_DRACULA; curr++) {
			if (gv->players[curr].location == location) {
				gv->vampires.location = NOWHERE;
				gv->vampires.has_matured = false;
				gv->vampires.is_alive = false;
				gv->vampires.is_revealed = false;
				return true;
			}
		}
	}

	return false;
}

void initVampires(GameView gv) {
	gv->vampires.location = NOWHERE;
	gv->vampires.is_alive = false;
	gv->vampires.has_matured = false;
	gv->vampires.is_revealed = false;

}

void initTraps(GameView gv) {

	gv->traps.locations = malloc(sizeof(int *));
	gv->traps.num = 0;
}

PlaceId *setTraps(GameView gv) {

	PlaceId location = GvGetPlayerLocation(gv, PLAYER_DRACULA);
	int count = gv->traps.num;

	if (location == NOWHERE) return gv->traps.locations;

	if (count == 0) {
		gv->traps.locations[0] = location;
		gv->traps.num++;
	} else {
		gv->traps.locations[count + 1] = location;
		gv->traps.num++;
	}
		
	
	return gv->traps.locations;
}

void trap_encountered(GameView gv) {
}

void updateGameScore(GameView gv, char *play, Player player) {
	if (player == PLAYER_DRACULA) {
		if (play[5] == 'V') {
			gv->score = gv->score - 13;
		}
		gv->score --;
	} else {
		if ((gv->players[player]).health <= 0) {
			gv->score = gv->score - SCORE_LOSS_HUNTER_HOSPITAL;
		}
	}
}
	
void vampireInGeneral(GameView gv, Player player) {

	vampirePlaced(gv);
	vampireMatures(gv);
	vampireVanquished( gv, player) ;
}

void encounterDrac(GameView gv, Player player) {

	Player curr = player;
	PlaceId drac_loc = GetCurrentLocation(gv, PLAYER_DRACULA);
		if (GetCurrentLocation(gv, curr) == drac_loc && curr != PLAYER_DRACULA && GvGetRound(gv) != 0) {
			gv->if_drac_isrev = true;
			gv->vampires.is_revealed = true;
			gv->players[curr].health = gv->players[curr].health - 4;
			gv->players[PLAYER_DRACULA].health = gv->players[PLAYER_DRACULA].health - 10;
			printf("PLAYER::: %d   HELTHHH: %d\n \n",curr ,gv->players[curr].health);		
		}

			
}

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
			}
		}
		for (int i = 0; i < histCount - 1; i ++) {
			if (playerHistory[i] == playerHistory[i + 1]) {
				
				health = health + LIFE_GAIN_REST;
				
				if (health > GAME_START_HUNTER_LIFE_POINTS) {
					health = GAME_START_HUNTER_LIFE_POINTS;
				}
			}
		}

	} else {
		
		//COUNTER FOR DRACULA ENCOUNTER AND DRACULA AT CASTLE
		char *freeTmp = strdup(pastPlays);
		char *tmp = freeTmp;

		int encounter = 0; 
		char *play;
		
		while ((play = strsep(&tmp, " ")) != NULL) {
			if (play[0] != 'D') {
				for (int i = 3; i < PLAY_SIZE; i++) {
					if (play[i] == 'D') {
						encounter ++;
					}
				}		
			}
		}
		free (freeTmp);

		health = health - (LIFE_LOSS_HUNTER_ENCOUNTER * encounter);

		//HEALTH LOSS AT SEA/ HEALTH GAIN AT CASTLE
		for (int i = 0; i < histCount; i ++) {
			if (placeIdToType(playerHistory[i]) == SEA) {
				health = health - LIFE_LOSS_SEA;
			}
			if (playerHistory[i] == CASTLE_DRACULA) {
				health = health + LIFE_GAIN_CASTLE_DRACULA;
			}
		}
	}	
	
	(gv->players[player]).health = health;
}