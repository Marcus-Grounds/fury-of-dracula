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

#include <stdio.h>
#include "dracula.h"
#include "DraculaView.h"
#include "GameView.h"
#include "Game.h"
#include "queue.h"
#include <stdio.h>
#include <string.h>

#define NUM_HUNTERS 4
#define MIN_STARTING_DISTANCE 4
#define WORTH_OF_NON_SEA 3
#define HALF_START_HEALTH GAME_START_BLOOD_POINTS/2

#define guaranteeHunterKillIfSameLoc(round, healthHunter, locFallingOffTrail, desiredLoc, numTrapsDesiredLoc)\
									((round % 13 == 0 && locFallingOffTrail != desiredLoc && healthHunter <= numTrapsDesiredLoc * LIFE_LOSS_TRAP_ENCOUNTER)\
									|| ((round % 13 == 0 && locFallingOffTrail == desiredLoc && healthHunter <=  (numTrapsDesiredLoc - 1) * LIFE_LOSS_TRAP_ENCOUNTER))\
									|| ((round % 13 != 0 && (round - TRAIL_SIZE) % 13 != 0) && locFallingOffTrail == desiredLoc && healthHunter <= numTrapsDesiredLoc * LIFE_LOSS_TRAP_ENCOUNTER)\
									|| ((round % 13 != 0 && (round - TRAIL_SIZE) % 13 != 0) && locFallingOffTrail != desiredLoc && healthHunter <= (numTrapsDesiredLoc + 1) * LIFE_LOSS_TRAP_ENCOUNTER)\
									|| ((round % 13 != 0 && (round - TRAIL_SIZE) % 13 == 0) && healthHunter <= (numTrapsDesiredLoc + 1) * LIFE_LOSS_TRAP_ENCOUNTER))

struct closestHunter {
	int hunterDist;
	Player hunter;
	PlaceId hunterLoc;
};
typedef struct closestHunter *ClosestHunter;

// Helper function declarations:
bool locationInArray(PlaceId location, PlaceId *array, int numArray);
char *dracLocToMoveAbbrev(DraculaView dv, PlaceId loc);
void lowHealthMove(DraculaView dv, PlaceId furthestLoc);
void handleRoundZero(DraculaView dv);
int distFromHunters(DraculaView dv, PlaceId loc);
int shortestPathFrom(DraculaView dv, Player hunter, PlaceId dest);
ClosestHunter initClosestHunter(void);
void storeClosestHunter(DraculaView dv, ClosestHunter closestHunterInfo, 
						PlaceId dracLoc);
PlaceId maxLocFromHunter(DraculaView dv, PlaceId *locArray, 
							int locArrayLen, Player hunter);
Player hunterAtSameSea(DraculaView dv, PlaceId currSeaLoc, int *numHunter);

void decideDraculaMove(DraculaView dv)
{
	// TODO: Register a random move to ensure that a move is made.
	////////////////////////////////////////////////////////////////////////
	Round round = DvGetRound(dv);
	// Decide on best location to spawn.
	if (round == 0) {
		handleRoundZero(dv);
		return;
	}

	int numLocs = 0;
	PlaceId currLoc = DvGetPlayerLocation(dv, PLAYER_DRACULA);
	PlaceId *reachableLocations = DvWhereCanIGo(dv, &numLocs);
	
	// Handle cases when no choice.
	if (numLocs == 0 && round != 0) registerBestPlay("TP", "Mwahahahaha"); 
	if (numLocs == 1) registerBestPlay(dracLocToMoveAbbrev(dv, reachableLocations[0]), "...");
	// TODO: Calculate shortest means to castle dracula, if health is low
	//if (health <= 10) {
		// Get safest path to castle dracula
		// a. Direct path with less than or equal to (health - 2) / 2 seas
		// b. Self-enforced teleport.
	//}

	// Squeeze maximum information out of current situation.
	// TODO: Calculate furthest distance, if at a distance of 2 or less, from closest hunter.
	// int distClosestHunterNonSea = 0;
	// int distClosestHunter = 0;
	int furthestTotalDist = 0;
	PlaceId furthestLoc = reachableLocations[0];
	int furthestNonSeaDist = 0;
	PlaceId furthestNonSeaLoc = CITY_UNKNOWN;
	int furthestSeaDist = 0;
	// PlaceId furthestSeaLoc = SEA_UNKNOWN; //TODO: error as variable wasn't used
	for (int i = 0; i < numLocs; i++) {
		int totalDist = distFromHunters(dv, reachableLocations[i]);
		if (totalDist > furthestTotalDist) {
			furthestTotalDist = totalDist;
			furthestLoc = reachableLocations[i];
		}

		if (placeIsLand(reachableLocations[i]) && totalDist > furthestNonSeaDist) {
			furthestNonSeaDist = totalDist;
			furthestNonSeaLoc = reachableLocations[i];
		}

		if (placeIsSea(reachableLocations[i] && totalDist > furthestSeaDist)) {
			furthestSeaDist = totalDist;
			// furthestSeaLoc = reachableLocations[i];
		}
	}

	int numHunterSameSea = 0;
	int numTrapsAtNonSeaLoc = 0;
	int healthHunterSameSea = 0;
	PlaceId locFallingOffTrail = DvGetLocationFallingOffTrail(dv);
	Player hunterSameSea = PLAYER_DRACULA;
	if (placeIsSea(currLoc))  {
		hunterSameSea = hunterAtSameSea(dv, currLoc, &numHunterSameSea);
		healthHunterSameSea = DvGetHealth(dv, hunterSameSea);
		numTrapsAtNonSeaLoc = numTrapsAtLoc(dv, furthestNonSeaLoc);
	}

	// Register best play, in order of precedence.
	// Handle case when there is nso choice but to travel by sea.
	if (furthestNonSeaLoc == CITY_UNKNOWN) registerBestPlay(dracLocToMoveAbbrev(dv, furthestLoc), "...");
	// Give precedece to avoiding suiciding at sea when he has 2 blood points (or less) left...
	else if (DvGetHealth(dv, PLAYER_DRACULA) <= LIFE_LOSS_SEA) registerBestPlay(dracLocToMoveAbbrev(dv, furthestNonSeaLoc), "...");
	// ... then to avoiding the closest hunter, if dangerously close, health is low, and can't guarantee a hunter kill...
	// a. This could mean heading to sea to avoid an encounter.
	// b. This could mean maximising your distance from the hunter
	else if (DvGetHealth(dv, PLAYER_DRACULA) <= HALF_START_HEALTH) lowHealthMove(dv, furthestLoc);
	// else if (health <= 12 && distClosestHunter <= 1 && DvGetHealth(dv, closestHunter) > highestDamageIfEncounteredAtCity) registerBestPlay(dracLocToMoveAbbrev(dv, furthestLoc), "...");
	// ... then to avoiding land if hunter is at the same sea location of Dracula, and a hunter kill is not guaranteed...
	else if (placeIsSea(currLoc) && 
			 (numHunterSameSea > 1 
			 || (numHunterSameSea == 1 
			 	&& guaranteeHunterKillIfSameLoc(round, healthHunterSameSea, locFallingOffTrail, furthestNonSeaLoc, numTrapsAtNonSeaLoc))))
		registerBestPlay(dracLocToMoveAbbrev(dv, furthestLoc), "...");
	// ... then to placing the immature vampire...
	else if (round % 13 == 0) registerBestPlay(dracLocToMoveAbbrev(dv, furthestNonSeaLoc), "...");
	// ... then to making his way to Castle Dracula, if health is low...
	// else if (health < LOW_HEALTH) find shortest way to castle dracula - could be either by self-restriction or travelling
	// ... then to separating himself from the hunters, avoiding the sea...
		// If heading to sea means that one less encounter can be made with a hunter before death, then
		// head to the best city location instead.	
	else if ((DvGetHealth(dv, PLAYER_DRACULA) - LIFE_LOSS_SEA) % LIFE_LOSS_HUNTER_ENCOUNTER == 0)
		registerBestPlay(dracLocToMoveAbbrev(dv, furthestNonSeaLoc), "...");
	else 
		registerBestPlay(dracLocToMoveAbbrev(dv, furthestLoc), "...");

}


void handleRoundZero(DraculaView dv) {
	PlaceId bestLoc = MIN_REAL_PLACE;
	int currBestTotal = 0;

	int distSetFromHunter[NUM_HUNTERS];
	int totalScatter = 0;
	int totalDist = 0;
	for (PlaceId loc = MIN_REAL_PLACE; loc < MAX_REAL_PLACE; loc++) {
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
	
	if (bestLoc != MIN_REAL_PLACE) registerBestPlay(placeIdToAbbrev(bestLoc), "Mwahahahaha");
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
			free(intermediateConns);
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

// Returns true if the location exists in the array, and false if
// it doesn't
bool locationInArray(PlaceId location, PlaceId *array, int numArray) {
	for (int i = 0; i < numArray; i++) {
		if (array[i] == location) return true;
	}
	return false;
}

// Avoiding the closest hunter if health is low
// This func assumes that Dracula has valid moves other than TP
void lowHealthMove(DraculaView dv, PlaceId furthestLoc) {
	// Finding moves which avoid potential encounters 
	// with hunters in the next round

	PlaceId *locAvoidsHunt = NULL;
	int numAvoidsHunt = 0;

	int numDracReachable = 0;
	PlaceId *dracReachable = DvWhereCanIGo(dv, &numDracReachable);

	for (Player p = PLAYER_LORD_GODALMING; 
		 p <= PLAYER_MINA_HARKER; p++) {
		int numHuntReachable = 0;
		PlaceId *huntReachable = DvWhereCanTheyGo(dv, p, &numHuntReachable);
		
		for (int i = 0; i < numDracReachable; i++) {
			if (!locationInArray(dracReachable[i], huntReachable, 
								numHuntReachable)) {
				locAvoidsHunt[numAvoidsHunt] = dracReachable[i];
				numAvoidsHunt++;
			}
		}
		free(huntReachable);
	}

	// Choosing a location for Dracula's move
	PlaceId locChosen = NOWHERE;

	ClosestHunter nearestHunter = initClosestHunter();
	PlaceId dracLoc = DvGetPlayerLocation(dv, PLAYER_DRACULA);
	storeClosestHunter(dv, nearestHunter, dracLoc);

	int numDracSea = 0;
	PlaceId *dracSea = DvWhereCanIGoByType(dv, false, true, &numDracSea);

	if ((nearestHunter->hunterDist <= 2) && (numDracSea != 0)) {
		// Heading to sea if possible to avoid an encounter if 
		// the closest hunter is <= 2 moves away
		// TODO: currently chooses the first sea location in array
		locChosen = dracSea[0];
	} else {
		// Aim to have move not reachable by any hunters
		// Also aim to stay on land and maximise distance from nearest hunter
		if (numAvoidsHunt == 0) {
			// Next move cannot avoid hunters' reachable locations
			// Choose the move with furthest total distance from hunters
			locChosen = furthestLoc;
		} else {
			// Play move which maximises distance from nearest hunter
			// Try to stay on land but if not possible, go to sea
			locChosen = maxLocFromHunter(dv, locAvoidsHunt, numAvoidsHunt, 
										nearestHunter->hunter);
		}
	}

	free(dracReachable);
	free(nearestHunter);
	free(dracSea);
	registerBestPlay(dracLocToMoveAbbrev(dv, locChosen), "...");
	return;
}

// Finds a location in locArray that has the maximum distance from the hunter's
// location.
// Aim for a land location but if no land moves exist in locArray, return
// a sea location.
PlaceId maxLocFromHunter(DraculaView dv, PlaceId *locArray, 
							int locArrayLen, Player hunter) {

	bool landMoveExists = false;
	for (int i = 0; i < locArrayLen; i++) {
		if (placeIsLand(locArray[i])) landMoveExists = true;
	}

	int maxDist = 0;
	PlaceId maxLoc = locArray[0];
	for (int i = 0; i < locArrayLen; i++) {
		if (landMoveExists && (placeIsSea(locArray[i]))) continue;
		int dist = shortestPathFrom(dv, hunter, locArray[i]);
		if (dist > maxDist) {
			maxLoc = locArray[i];
			maxDist = dist;
		} else if (dist == maxDist) {
			// if 2 moves are both the max distance, 
			// choose move w highest distFromHunters
			if (distFromHunters(dv, locArray[i]) > distFromHunters(dv, maxLoc)) {
				maxLoc = locArray[i];
				maxDist = dist;
			}
		}
	}
	return maxLoc;
}

char *dracLocToMoveAbbrev(DraculaView dv, PlaceId loc) {
	// Assumes that the location given is reachable by a valid Dracula move

	char *abbrev = NULL;
	int numValid = 0;
	PlaceId *validMoves = DvGetValidMoves(dv, &numValid);

	int numDracHist = 0; 
	bool histCanFree = false;
	PlaceId *dracLocHist = DvGetLocationHistory(dv, &numDracHist, &histCanFree);

	for (int i = 0; i < numValid; i++) {
		if (validMoves[i] == loc) {
			abbrev = placeIdToAbbrev(validMoves[i]);
			break;
		}
		if ((validMoves[i] == HIDE) && 
			(DvGetPlayerLocation(dv, PLAYER_DRACULA) == loc)) {
			abbrev = placeIdToAbbrev(validMoves[i]);
			break;
		}

		if ((validMoves[i] >= DOUBLE_BACK_1) && 
			(validMoves[i] <= DOUBLE_BACK_5)) {
			PlaceId doubleBackLoc = dracLocHist[numDracHist - 1 - 
									(validMoves[i] - DOUBLE_BACK_1)];
			if (doubleBackLoc == loc) {
				abbrev = placeIdToAbbrev(validMoves[i]);
				break;
			}
		}
	}
	free(validMoves);
	if (histCanFree) free(dracLocHist);
	return abbrev;
}

ClosestHunter initClosestHunter(void) {
	ClosestHunter new = malloc(sizeof(*new));
	if (new == NULL) {
		printf("Could not allocate memory\n");
		exit(EXIT_FAILURE);
	}
	new->hunterDist = -1;
	new->hunter = PLAYER_LORD_GODALMING;
	new->hunterLoc = NOWHERE;
	return new;
}

void storeClosestHunter(DraculaView dv, ClosestHunter closestHunterInfo, 
						PlaceId dracLoc) {
	int minDist = shortestPathFrom(dv, PLAYER_LORD_GODALMING, dracLoc);
	int closestHunter = PLAYER_LORD_GODALMING;
	int closestLoc = DvGetPlayerLocation(dv, PLAYER_LORD_GODALMING);
	for (int hunter = PLAYER_DR_SEWARD; hunter <= PLAYER_MINA_HARKER; hunter++) {
		int hunterDist = shortestPathFrom(dv, hunter, dracLoc);
		if (hunterDist < minDist) {
			minDist = hunterDist;
			closestHunter = hunter;
			closestLoc = DvGetPlayerLocation(dv, hunter);
		}
	}
	closestHunterInfo->hunter = closestHunter;
	closestHunterInfo->hunterDist = minDist;
	closestHunterInfo->hunterLoc = closestLoc;
	return;
}

Player hunterAtSameSea(DraculaView dv, PlaceId currSeaLoc, int *numHunter) {
	*numHunter = 0;
	Player hunterReturned = PLAYER_DRACULA;
	for (Player i = PLAYER_LORD_GODALMING; i <= PLAYER_MINA_HARKER; i++) {
		if (DvGetPlayerLocation(dv, i) == currSeaLoc) hunterReturned = i;
		(*numHunter)++;
	}
	return hunterReturned;
}

