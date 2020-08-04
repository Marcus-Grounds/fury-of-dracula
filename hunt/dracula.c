////////////////////////////////////////////////////////////////////////
// COMP2521 20T2 ... the Fury of Dracula
// dracula.c: your "Fury of Dracula" Dracula AI
//
// 2014-07-01	v1.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2017-12-01	v1.1	Team Dracula <cs2521@cse.unsw.edu.au>
// 2018-12-31	v2.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2020-07-10	v3.0	Team Dracula <cs2521@cse.unsw.edu.au>
//
////////////////////////////////////////////////////////////////////////

#include "dracula.h"
#include "DraculaView.h"
#include "GameView.h"
#include "Game.h"
#include "queue.h"
#include <string.h>
#define NUM_HUNTERS 4
#define MIN_STARTING_DISTANCE 4

void handleRoundZero(DraculaView dv);
void avoidSuicide(DraculaView dv, PlaceId loc);
int distFromHunters(DraculaView dv, PlaceId loc);
int shortestPathFrom(DraculaView dv, Player hunter, PlaceId dest);
PlaceId *DvWhereCanTheyGoByRound(DraculaView dv, Player player, 
								 Round round, PlaceId from, 
								 int *numReturnedLocs);
char *placeIdToAbbrevNonConst(PlaceId pid);

void decideDraculaMove(DraculaView dv)
{
	// TODO: Register a random move to ensure that a move is made.
	////////////////////////////////////////////////////////////////////////
	
	Round round = DvGetRound(dv);
	int numLocs = 0;
	
	// Decide on best location to spawn.
	if (round == 0) {
		handleRoundZero(dv);
		return;
	}
	PlaceId *reachableLocations = DvWhereCanIGo(dv, &numLocs);
	
	// Handle cases when no choice.
	if (numLocs == 0 && round != 0) registerBestPlay("CD", "Mwahahahaha");
	if (numLocs == 1) registerBestPlay(placeIdToAbbrevNonConst(reachableLocations[0]), "...");

	// Give precedence to avoiding hunters, then to avoiding the sea.
	/*int distClosestHunterNonSea = 0;
	int distClosestHunter = 0;
	int bestTotalDist = 0;
	PlaceId furthestLoc = reachableLocations[0];
	int furthestNonSeaDist = 0;
	PlaceId bestNonSeaLoc = CITY_UNKNOWN;
	for (int i = 0; i < numLocs; i++) {
		int totalDist = distFromHunters(dv, reachableLocations[i]);
		if (totalDist > bestDist) {
			bestTotalDist = totalDist;
			bestLoc = reachableLocations[i];
			if (!placeIsSea(bestLoc)) {
				bestNonSeaDist = totalDist;
				bestNonSeaLoc = bestLoc;
				
			}
		}
	}

	if (bestNonSeaLoc == CITY_UNKNOWN || distClosestHunterNonSea == 1) registerBestPlay(placeIdToAbbrevNonConst(bestLoc), "...")
	else if (round % 13 == 0) registerBestPlay(placeIdToAbbrevNonConst(bestNonSeaLoc), "...")
	else if ()
	totalDist = 0;*/
}

void handleRoundZero(DraculaView dv) {
	PlaceId bestLoc = MIN_REAL_PLACE;
	int currBestTotal = 0;

	PlaceId loc;
	int distSetFromHunter[NUM_HUNTERS];
	int totalScatter = 0;
	int totalDist = 0;

	for (loc = MIN_REAL_PLACE; loc < MAX_REAL_PLACE; loc++) {
		if (loc == HOSPITAL_PLACE || loc == CASTLE_DRACULA || placeIsSea(loc)) continue;
		for (int hunter = PLAYER_LORD_GODALMING; hunter <= PLAYER_MINA_HARKER; hunter++) {
			distSetFromHunter[hunter] = shortestPathFrom(dv, hunter, loc);
			if (distSetFromHunter[hunter] < MIN_STARTING_DISTANCE) break;
			totalDist += distSetFromHunter[hunter];
			for (int i = 0; i < hunter; i++) {
				totalScatter += (distSetFromHunter[hunter] > distSetFromHunter[i]) * distSetFromHunter[hunter] + 
							    (distSetFromHunter[hunter] < distSetFromHunter[i]) * distSetFromHunter[i];
			}
		}
		if (totalDist + totalScatter > currBestTotal) {
			currBestTotal = totalDist + totalScatter;
			bestLoc = loc;
		}
		totalDist = 0;
		totalScatter = 0;
	}
	
	if (bestLoc != MIN_REAL_PLACE) registerBestPlay(placeIdToAbbrevNonConst(bestLoc), "Mwahahahaha");
}

/*
void avoidSuicide(DraculaView dv, PlaceId *reachableLocations) {
	if (health <= 2) {
		
	}
}

int distFromHunters(DraculaView dv, PlaceId loc) {
	int distSetFromHunter[NUM_HUNTERS];
	int totalDist = 0;
	for (int hunter = PLAYER_LORD_GODALMING; hunter <= PLAYER_MINA_HARKER; hunter++) {
		distSetFromHunter[hunter] = shortestPathFrom(dv, hunter, loc);
		totalDist += distSetFromHunter[hunter];
	}
	return totalDist;
}
*/

int shortestPathFrom(DraculaView dv, Player hunter, PlaceId dest)
{

	PlaceId src = DvGetPlayerLocation(dv, hunter);
	Round round = DvGetRound(dv) + 1;

	// Initialise visited array to NOWHERE;
	PlaceId visited[NUM_REAL_PLACES];
	for (int i = 0; i < NUM_REAL_PLACES; i++) visited[i] = NOWHERE;

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
			PlaceId *intermediateConns = DvWhereCanTheyGoByRound(dv, hunter, 
														  		 round + roundsFromSrc, 
														 		 intermediate, 
														  		 &numLocs);
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
	int pathLength = 0;
	for (PlaceId intermediate = dest; 
		 intermediate != src; 
		 intermediate = visited[intermediate]) {
		pathLength++;
	}

	return pathLength;
}

char *placeIdToAbbrevNonConst(PlaceId pid) {
	return strdup(placeIdToAbbrev(pid));
}