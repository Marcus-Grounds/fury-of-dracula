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
#define WORTH_OF_NON_SEA 3

void handleRoundZero(DraculaView dv);
int distFromHunters(DraculaView dv, PlaceId loc);
int shortestPathFrom(DraculaView dv, Player hunter, PlaceId dest);
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
	// TODO: Calculate shortest means to castle dracula, if health is low
	// Squeeze maximum information out of current situation.
	// TODO: Calculate furthest distance, if at a distance of 2 or less, from closest hunter.
	// int distClosestHunterNonSea = 0;
	// int distClosestHunter = 0;
	int furthestTotalDist = 0;
	PlaceId furthestLoc = reachableLocations[0];
	int furthestNonSeaDist = 0;
	PlaceId furthestNonSeaLoc = CITY_UNKNOWN;
	for (int i = 0; i < numLocs; i++) {
		int totalDist = distFromHunters(dv, reachableLocations[i]);
		if (totalDist > furthestTotalDist) {
			furthestTotalDist = totalDist;
			furthestLoc = reachableLocations[i];
		}
		if (!placeIsSea(reachableLocations[i]) && totalDist > furthestNonSeaDist) {
			furthestNonSeaDist = furthestTotalDist;
			furthestNonSeaLoc = reachableLocations[i];
		}
	}
	// Register best play, in order of precedence.
	// Handle case when there is no choice but to go to sea.
	if (furthestNonSeaLoc == CITY_UNKNOWN) registerBestPlay(placeIdToAbbrevNonConst(furthestLoc), "...");
	// Give precedence to avoiding suiciding at sea when he has 2 blood points (or less) left...
	else if (DvGetHealth(dv, PLAYER_DRACULA) <= LIFE_LOSS_SEA) registerBestPlay(placeIdToAbbrevNonConst(furthestNonSeaLoc), "...");
	// ... then to avoiding the closest hunter, if dangerously close, health is low, and can't guarantee a hunter kill...
	// a. This could mean heading to sea to avoid an encounter.
	// b. This could mean maximising your distance from the hunter
	// else if (health <= 12 && distClosestHunter <= 1 && DvGetHealth(dv, closestHunter) > highestDamageIfEncounteredAtCity) registerBestPlay(placeIdToAbbrevNonConst(furthestLoc), "...");
	// ... then to placing the immature vampire...
	else if (round % 13 == 0) registerBestPlay(placeIdToAbbrevNonConst(furthestNonSeaLoc), "...");
	// ... then to making his way to Castle Dracula, if health is low...
	// else if (health < LOW_HEALTH) find shortest way to castle dracula - could be either by self-restriction or travelling
	// ... then to separating himself from the hunters, avoiding the sea...
	else {
		// If heading to sea means that one less encounter can be made with a hunter before death, then
		// head to the best city location instead.
		if ((DvGetHealth(dv, PLAYER_DRACULA) - LIFE_LOSS_SEA) % LIFE_LOSS_HUNTER_ENCOUNTER == 0) {
			registerBestPlay(placeIdToAbbrevNonConst(furthestNonSeaLoc), "...");
		} else {
			registerBestPlay(placeIdToAbbrevNonConst(furthestLoc), "...");
		}
	}
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

int distFromHunters(DraculaView dv, PlaceId loc) {
	int distSetFromHunter[NUM_HUNTERS];
	int totalDist = 0;
	for (int hunter = PLAYER_LORD_GODALMING; hunter <= PLAYER_MINA_HARKER; hunter++) {
		distSetFromHunter[hunter] = shortestPathFrom(dv, hunter, loc);
		totalDist += distSetFromHunter[hunter];
	}
	return totalDist;
}

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