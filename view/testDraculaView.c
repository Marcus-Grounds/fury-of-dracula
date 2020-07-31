////////////////////////////////////////////////////////////////////////
// COMP2521 20T2 ... the Fury of Dracula
// testDraculaView.c: test the DraculaView ADT
//
// As supplied, these are very simple tests.  You should write more!
// Don't forget to be rigorous and thorough while writing tests.
//
// 2014-07-01	v1.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2017-12-02	v1.1	Team Dracula <cs2521@cse.unsw.edu.au>
// 2018-12-31	v2.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2020-07-10	v3.0	Team Dracula <cs2521@cse.unsw.edu.au>
//
////////////////////////////////////////////////////////////////////////

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "DraculaView.h"
#include "Game.h"
#include "Places.h"
#include "testUtils.h"

int main(void)
{
	{///////////////////////////////////////////////////////////////////
	
		printf("Test for basic functions, "
			   "just before Dracula's first move\n");

		char *trail =
			"GST.... SAO.... HZU.... MBB....";
		
		Message messages[] = {
			"Hello", "Goodbye", "Stuff", "..."
		};
		
		DraculaView dv = DvNew(trail, messages);

		assert(DvGetRound(dv) == 0);
		assert(DvGetScore(dv) == GAME_START_SCORE);
		assert(DvGetHealth(dv, PLAYER_DRACULA) == GAME_START_BLOOD_POINTS);
		assert(DvGetPlayerLocation(dv, PLAYER_LORD_GODALMING) == STRASBOURG);
		assert(DvGetPlayerLocation(dv, PLAYER_DR_SEWARD) == ATLANTIC_OCEAN);
		assert(DvGetPlayerLocation(dv, PLAYER_VAN_HELSING) == ZURICH);
		assert(DvGetPlayerLocation(dv, PLAYER_MINA_HARKER) == BAY_OF_BISCAY);
		assert(DvGetPlayerLocation(dv, PLAYER_DRACULA) == NOWHERE);
		assert(DvGetVampireLocation(dv) == NOWHERE);
		int numTraps = -1;
		PlaceId *traps = DvGetTrapLocations(dv, &numTraps);
		assert(numTraps == 0);
		free(traps);

		printf("Test passed!\n");
		DvFree(dv);
	}

	{///////////////////////////////////////////////////////////////////
	
		printf("Test for encountering Dracula\n");

		char *trail =
			"GST.... SAO.... HCD.... MAO.... DGE.V.. "
			"GGEVD.. SAO.... HCD.... MAO....";
		
		Message messages[] = {
			"Hello", "Goodbye", "Stuff", "...", "Mwahahah",
			"Aha!", "", "", ""
		};
		
		DraculaView dv = DvNew(trail, messages);

		assert(DvGetRound(dv) == 1);
		assert(DvGetScore(dv) == GAME_START_SCORE - SCORE_LOSS_DRACULA_TURN);
		assert(DvGetHealth(dv, PLAYER_LORD_GODALMING) == 5);
		assert(DvGetHealth(dv, PLAYER_DRACULA) == 30);
		assert(DvGetPlayerLocation(dv, PLAYER_LORD_GODALMING) == GENEVA);
		assert(DvGetPlayerLocation(dv, PLAYER_DRACULA) == GENEVA);
		assert(DvGetVampireLocation(dv) == NOWHERE);

		printf("Test passed!\n");
		DvFree(dv);
	}

	{///////////////////////////////////////////////////////////////////
	
		printf("Test for Dracula leaving minions 1\n");

		char *trail =
			"GGE.... SGE.... HGE.... MGE.... DED.V.. "
			"GST.... SST.... HST.... MST.... DMNT... "
			"GST.... SST.... HST.... MST.... DLOT... "
			"GST.... SST.... HST.... MST.... DHIT... "
			"GST.... SST.... HST.... MST....";
		
		Message messages[24] = {};
		DraculaView dv = DvNew(trail, messages);

		assert(DvGetRound(dv) == 4);
		assert(DvGetVampireLocation(dv) == EDINBURGH);
		int numTraps = -1;
		PlaceId *traps = DvGetTrapLocations(dv, &numTraps);
		
		assert(numTraps == 3);
		sortPlaces(traps, numTraps);
		assert(traps[0] == LONDON);
		assert(traps[1] == LONDON);
		assert(traps[2] == MANCHESTER);
		free(traps);
		
		printf("Test passed!\n");
		DvFree(dv);
	}

	{///////////////////////////////////////////////////////////////////
		printf("Test for Dracula's valid moves 1\n");
		
		char *trail =
			"GGE.... SGE.... HGE.... MGE.... DCD.V.. "
			"GGE.... SGE.... HGE.... MGE....";
		
		Message messages[9] = {};
		DraculaView dv = DvNew(trail, messages);
		
		int numMoves = -1;
		PlaceId *moves = DvGetValidMoves(dv, &numMoves);
		assert(numMoves == 4);
		sortPlaces(moves, numMoves);
		assert(moves[0] == GALATZ);
		assert(moves[1] == KLAUSENBURG);
		assert(moves[2] == HIDE);
		assert(moves[3] == DOUBLE_BACK_1);
		free(moves);
		
		printf("Test passed!\n");
		DvFree(dv);

	} 
	
	{///////////////////////////////////////////////////////////////////
	
		printf("Testing DvGetValidMoves.\n"); 
		printf("\tCase: Dracula has not made any moves.\n");
	
		char *trail =
			"GGE.... SGE.... HGE.... MGE....";
		
		Message messages[9] = {};
		DraculaView dv = DvNew(trail, messages);
		
		int numMoves = -1;
		PlaceId *moves = DvGetValidMoves(dv, &numMoves);
		assert(numMoves == 0);
		assert (moves == NULL);
		DvFree(dv);
	} 

	{///////////////////////////////////////////////////////////////////
		
		printf("\tCase: Made 1 LOCATION move at city.\n");
	
		char *trail =
			"GGE.... SGE.... HGE.... MGE.... DLS.V.. "
			"GGE.... SGE.... HGE.... MGE....";
		
		Message messages[9] = {};
		DraculaView dv = DvNew(trail, messages);
		
		int numMoves = -1;
		PlaceId *moves = DvGetValidMoves(dv, &numMoves);
		
		assert(numMoves == 6);
		sortPlaces(moves, numMoves);
		assert(moves[0] == ATLANTIC_OCEAN);
		assert(moves[1] == CADIZ);
		assert(moves[2] == MADRID);
		assert(moves[3] == SANTANDER);
		assert(moves[4] == HIDE);
		assert(moves[5] == DOUBLE_BACK_1);
		free(moves);
		
		DvFree(dv);
		
	} 
	
	{///////////////////////////////////////////////////////////////////
	
		printf("\tCase: Made 1 LOCATION move at sea.\n");
	
		char *trail =
			"GGE.... SGE.... HGE.... MGE.... DBS.V.. "
			"GGE.... SGE.... HGE.... MGE....";
		
		Message messages[9] = {};
		DraculaView dv = DvNew(trail, messages);
		
		int numMoves = -1;
		PlaceId *moves = DvGetValidMoves(dv, &numMoves);
	
		assert(numMoves == 4);
		sortPlaces(moves, numMoves);
		assert(moves[0] == CONSTANTA);
		assert(moves[1] == IONIAN_SEA);
		assert(moves[2] == VARNA);
		assert(moves[3] == DOUBLE_BACK_1);
		free(moves);
		
		DvFree(dv);
	} 

	{///////////////////////////////////////////////////////////////////
	
		printf("\tCase: Made 2 LOCATION moves at city.\n");

		char *trail =
			"GGE.... SGE.... HGE.... MGE.... DST.V.. "
			"GGE.... SGE.... HGE.... MGE.... DGET... "
			"GGE.... SGE.... HGE.... MGE....";
		
		Message messages[9] = {};
		DraculaView dv = DvNew(trail, messages);
		
		int numMoves = -1;
		PlaceId *moves = DvGetValidMoves(dv, &numMoves);
	
		assert(numMoves == 7);
		sortPlaces(moves, numMoves);
		assert(moves[0] == CLERMONT_FERRAND);
		assert(moves[1] == MARSEILLES);
		assert(moves[2] == PARIS);
		assert(moves[3] == ZURICH);
		assert(moves[4] == HIDE);
		assert(moves[5] == DOUBLE_BACK_1);
		assert(moves[6] == DOUBLE_BACK_2);
		free(moves);
		
		DvFree(dv);
	} 

	{///////////////////////////////////////////////////////////////////
	
		printf("\tCase: Made 1 LOCATION move at city and HIDE.\n");

		char *trail =
			"GGE.... SGE.... HGE.... MGE.... DST.V.. "
			"GGE.... SGE.... HGE.... MGE.... DHIT... "
			"GGE.... SGE.... HGE.... MGE....";
		
		Message messages[9] = {};
		DraculaView dv = DvNew(trail, messages);
		
		int numMoves = -1;
		PlaceId *moves = DvGetValidMoves(dv, &numMoves);
	
		assert(numMoves == 10);
		sortPlaces(moves, numMoves);
		assert(moves[0] == BRUSSELS);
		assert(moves[1] == COLOGNE);
		assert(moves[2] == FRANKFURT);
		assert(moves[3] == GENEVA);
		assert(moves[4] == MUNICH);
		assert(moves[5] == NUREMBURG);
		assert(moves[6] == PARIS);
		assert(moves[7] == ZURICH);
		assert(moves[8] == DOUBLE_BACK_1);
		assert(moves[9] == DOUBLE_BACK_2);
		free(moves);
		
		DvFree(dv);
	} 
	{///////////////////////////////////////////////////////////////////
	
		printf("\tCase: Made 1 LOCATION move at city and DOUBLE_BACK_1.\n");

		char *trail =
			"GGE.... SGE.... HGE.... MGE.... DST.V.. "
			"GGE.... SGE.... HGE.... MGE.... DD1T... "
			"GGE.... SGE.... HGE.... MGE....";
		
		Message messages[9] = {};
		DraculaView dv = DvNew(trail, messages);
		
		int numMoves = -1;
		PlaceId *moves = DvGetValidMoves(dv, &numMoves);
	
		assert(numMoves == 9);
		sortPlaces(moves, numMoves);
		assert(moves[0] == BRUSSELS);
		assert(moves[1] == COLOGNE);
		assert(moves[2] == FRANKFURT);
		assert(moves[3] == GENEVA);
		assert(moves[4] == MUNICH);
		assert(moves[5] == NUREMBURG);
		assert(moves[6] == PARIS);
		assert(moves[7] == ZURICH);
		assert(moves[8] == HIDE);
		free(moves);
		
		DvFree(dv);

	} 
	{///////////////////////////////////////////////////////////////////
	
		printf("\tCase: All currently reachable locations are in trail. HIDE and DOUBLE_BACK are valid.\n");

		char *trail =
			"GGE.... SGE.... HGE.... MGE.... DBE.V.. "
			"GGE.... SGE.... HGE.... MGE.... DKLT... "
			"GGE.... SGE.... HGE.... MGE.... DBDT... "
			"GGE.... SGE.... HGE.... MGE.... DZAT... "
			"GGE.... SGE.... HGE.... MGE.... DSZT... "
			"GGE.... SGE.... HGE.... MGE....";
		
		Message messages[9] = {};
		DraculaView dv = DvNew(trail, messages);
		
		int numMoves = -1;
		PlaceId *moves = DvGetValidMoves(dv, &numMoves);
	
		assert(numMoves == 6);
		sortPlaces(moves, numMoves);
		assert(moves[0] == HIDE);
		assert(moves[1] == DOUBLE_BACK_1);
		assert(moves[2] == DOUBLE_BACK_2);
		assert(moves[3] == DOUBLE_BACK_3);
		assert(moves[4] == DOUBLE_BACK_4);
		assert(moves[5] == DOUBLE_BACK_5);
		free(moves);
		
		DvFree(dv);
	} 
	{///////////////////////////////////////////////////////////////////
	
		printf("\tCase: Not all DOUBLE_BACK locations are adjacent.\n");
		
		char *trail =
			"GGE.... SGE.... HGE.... MGE.... DSN.V.. "
			"GGE.... SGE.... HGE.... MGE.... DLST... "
			"GGE.... SGE.... HGE.... MGE.... DCAT... "
			"GGE.... SGE.... HGE.... MGE....";
		
		Message messages[9] = {};
		DraculaView dv = DvNew(trail, messages);
		
		int numMoves = -1;
		PlaceId *moves = DvGetValidMoves(dv, &numMoves);
	
		assert(numMoves == 6);
		sortPlaces(moves, numMoves);
		assert(moves[0] == ATLANTIC_OCEAN);
		assert(moves[1] == GRANADA);
		assert(moves[2] == MADRID);
		assert(moves[3] == HIDE);
		assert(moves[4] == DOUBLE_BACK_1);
		assert(moves[5] == DOUBLE_BACK_2);
		free(moves);
		
		DvFree(dv);
	} 

	{///////////////////////////////////////////////////////////////////
	
		printf("\tCase: No valid moves other than TELEPORT.\n");

		char *trail =
			"GGE.... SGE.... HGE.... MGE.... DIO.V.. "
			"GGE.... SGE.... HGE.... MGE.... DBST... "
			"GGE.... SGE.... HGE.... MGE.... DCNT... "
			"GGE.... SGE.... HGE.... MGE.... DVRT... "
			"GGE.... SGE.... HGE.... MGE.... DD3T... "
			"GGE.... SGE.... HGE.... MGE....";
		
		Message messages[9] = {};
		DraculaView dv = DvNew(trail, messages);
		
		int numMoves = -1;
		PlaceId *moves = DvGetValidMoves(dv, &numMoves);
	
		assert(numMoves == 0);
		sortPlaces(moves, numMoves);
		assert(moves == NULL);
		free(moves);
		
		DvFree(dv);

		printf("Tests passed!\n");
		
	} 
	
	{///////////////////////////////////////////////////////////////////

		printf("Testing DvWhereCanIGo & DvWhereCanIGoByType.\n");
		printf("\tCase: From Bucharest. Double back and hide is invalid.\n");
		char *trail =
			"GGE.... SGE.... HGE.... MGE.... DKL.V.. "
			"GGE.... SGE.... HGE.... MGE.... DD1T... "
			"GGE.... SGE.... HGE.... MGE.... DBCT... "
			"GGE.... SGE.... HGE.... MGE.... DHIT... "
			"GGE.... SGE.... HGE.... MGE....";
		
		Message messages[24] = {};
		DraculaView dv = DvNew(trail, messages);

		printf("\t\tAll connections...\n");
		int numLocs = -1;
		PlaceId *locs = DvWhereCanIGo(dv, &numLocs);
		assert(numLocs == 4);
		sortPlaces(locs, numLocs);
		assert(locs[0] == BELGRADE);
		assert(locs[1] == CONSTANTA);
		assert(locs[2] == GALATZ);
		assert(locs[3] == SOFIA);							
		free(locs);

		printf("\t\tBoat connections...\n");
		numLocs = -1;
		locs = DvWhereCanIGoByType(dv, false, true, &numLocs);
		assert(numLocs == 0);
		assert(locs == NULL);											
		free(locs);

		printf("\t\tRoad connections...\n");
		numLocs = -1;
		locs = DvWhereCanIGoByType(dv, true, false, &numLocs);
		assert(numLocs == 4);
		sortPlaces(locs, numLocs);
		assert(locs[0] == BELGRADE);
		assert(locs[1] == CONSTANTA);
		assert(locs[2] == GALATZ);
		assert(locs[3] == SOFIA);												
		free(locs);
		DvFree(dv);

	}

	{///////////////////////////////////////////////////////////////////

		printf("\tCase: From Clermont Ferrand. Double back is valid.\n");
		char *trail =
			"GGE.... SGE.... HGE.... MGE.... DZU.V.. "
			"GGE.... SGE.... HGE.... MGE.... DSTT... "
			"GGE.... SGE.... HGE.... MGE.... DPAT... "
			"GGE.... SGE.... HGE.... MGE.... DCFT... "
			"GGE.... SGE.... HGE.... MGE....";
		
		Message messages[24] = {};
		DraculaView dv = DvNew(trail, messages);

		printf("\t\tAll connections...\n");
		int numLocs = -1;
		PlaceId *locs = DvWhereCanIGo(dv, &numLocs);
		assert(numLocs == 7);
		sortPlaces(locs, numLocs);
		assert(locs[0] == BORDEAUX);
		assert(locs[1] == CLERMONT_FERRAND);
		assert(locs[2] == GENEVA);
		assert(locs[3] == MARSEILLES);
		assert(locs[4] == NANTES);
		assert(locs[5] == PARIS);
		assert(locs[6] == TOULOUSE);

		printf("\t\tBoat connections...\n");
		numLocs = -1;
		locs = DvWhereCanIGoByType(dv, false, true, &numLocs);
		assert(numLocs == 1);
		assert(locs[0] == CLERMONT_FERRAND);
		free(locs);

		printf("\t\tRoad connections...\n");		
		numLocs = -1;
		locs = DvWhereCanIGoByType(dv, true, false, &numLocs);
		assert(numLocs == 7);
		sortPlaces(locs, numLocs);
		assert(locs[0] == BORDEAUX);
		assert(locs[1] == CLERMONT_FERRAND);
		assert(locs[2] == GENEVA);
		assert(locs[3] == MARSEILLES);
		assert(locs[4] == NANTES);
		assert(locs[5] == PARIS);
		assert(locs[6] == TOULOUSE);
		free(locs);
		DvFree(dv);
	}
	
	{///////////////////////////////////////////////////////////////////

		printf("\tCase: From Mediterranean Sea. Doubleback is invalid, hide is valid, Dracula is at sea.\n");
		char *trail =
			"GGE.... SGE.... HGE.... MGE.... DAL.V.. "
			"GGE.... SGE.... HGE.... MGE.... DGRT... "
			"GGE.... SGE.... HGE.... MGE.... DD2T... "
			"GGE.... SGE.... HGE.... MGE.... DMST... "
			"GGE.... SGE.... HGE.... MGE....";
		
		Message messages[24] = {};
		DraculaView dv = DvNew(trail, messages);
		
		printf("\t\tAll connections...\n");
		int numLocs = -1;
		PlaceId *locs = DvWhereCanIGo(dv, &numLocs);
		assert(numLocs == 5);
		sortPlaces(locs, numLocs);
		assert(locs[0] == ATLANTIC_OCEAN);
		assert(locs[1] == BARCELONA);
		assert(locs[2] == CAGLIARI);
		assert(locs[3] == MARSEILLES);
		assert(locs[4] == TYRRHENIAN_SEA);

		printf("\t\tBoat connections...\n");
		numLocs = -1;
		locs = DvWhereCanIGoByType(dv, false, true, &numLocs);
		assert(numLocs == 5);
		sortPlaces(locs, numLocs);
		assert(locs[0] == ATLANTIC_OCEAN);
		assert(locs[1] == BARCELONA);
		assert(locs[2] == CAGLIARI);
		assert(locs[3] == MARSEILLES);
		assert(locs[4] == TYRRHENIAN_SEA);
		free(locs);

		printf("\t\tRoad connections...\n");		
		numLocs = -1;
		locs = DvWhereCanIGoByType(dv, true, false, &numLocs);
		assert(numLocs == 0);
		assert(locs == NULL);
		free(locs);
		DvFree(dv);
	}

	{///////////////////////////////////////////////////////////////////

		printf("\tCase: From Paris. Doubleback is invalid, hide is valid, Dracula is on land.\n");
		char *trail =
			"GGE.... SGE.... HGE.... MGE.... DPA.V.. "
			"GGE.... SGE.... HGE.... MGE.... DLET... "
			"GGE.... SGE.... HGE.... MGE.... DNAT... "
			"GGE.... SGE.... HGE.... MGE.... DD3T... "
			"GGE.... SGE.... HGE.... MGE....";
		
		Message messages[24] = {};
		DraculaView dv = DvNew(trail, messages);

		printf("\t\tAll connections...\n");
		int numLocs = -1;
		PlaceId *locs = DvWhereCanIGo(dv, &numLocs);
		assert(numLocs == 5);
		sortPlaces(locs, numLocs);
		assert(locs[0] == BRUSSELS);
		assert(locs[1] == CLERMONT_FERRAND);
		assert(locs[2] == GENEVA);
		assert(locs[3] == PARIS);
		assert(locs[4] == STRASBOURG);
		free(locs);

		printf("\t\tBoat connections...\n");
		numLocs = -1;
		locs = DvWhereCanIGoByType(dv, false, true, &numLocs);
		assert(numLocs == 1);
		assert(locs[0] == PARIS);
		free(locs);

		printf("\t\tRoad connections...\n");		
		numLocs = -1;
		locs = DvWhereCanIGoByType(dv, true, false, &numLocs);
		assert(numLocs == 5);
		sortPlaces(locs, numLocs);
		assert(locs[0] == BRUSSELS);
		assert(locs[1] == CLERMONT_FERRAND);
		assert(locs[2] == GENEVA);
		assert(locs[3] == PARIS);
		assert(locs[4] == STRASBOURG);
		free(locs);

		DvFree(dv);
	}

	{///////////////////////////////////////////////////////////////////

		printf("\tCase: From Bordeaux. Some valid routes are by road, others are by sea.\n");
		char *trail =
			"GGE.... SGE.... HGE.... MGE.... DPA.V.. "
			"GGE.... SGE.... HGE.... MGE.... DLET... "
			"GGE.... SGE.... HGE.... MGE.... DNAT... "
			"GGE.... SGE.... HGE.... MGE.... DBOT... "
			"GGE.... SGE.... HGE.... MGE....";
		
		Message messages[24] = {};
		DraculaView dv = DvNew(trail, messages);

		printf("\t\tAll connections...\n");
		int numLocs = -1;
		PlaceId *locs = DvWhereCanIGo(dv, &numLocs);
		assert(numLocs == 6);
		sortPlaces(locs, numLocs);
		assert(locs[0] == BAY_OF_BISCAY);
		assert(locs[1] == BORDEAUX); 
		assert(locs[2] == CLERMONT_FERRAND);
		assert(locs[3] == NANTES);
		assert(locs[4] == SARAGOSSA);
		assert(locs[5] == TOULOUSE);

		printf("\t\tBoat connections...\n");
		numLocs = -1;
		locs = DvWhereCanIGoByType(dv, false, true, &numLocs);
		assert(numLocs == 2);
		sortPlaces(locs, numLocs);
		assert(locs[0] == BAY_OF_BISCAY);
		assert(locs[1] == BORDEAUX); 
		free(locs);

		printf("\t\tRoad connections...\n");		
		numLocs = -1;
		locs = DvWhereCanIGoByType(dv, true, false, &numLocs);
		assert(numLocs == 5);
		sortPlaces(locs, numLocs);
		assert(locs[0] == BORDEAUX); 
		assert(locs[1] == CLERMONT_FERRAND);
		assert(locs[2] == NANTES);
		assert(locs[3] == SARAGOSSA);
		assert(locs[4] == TOULOUSE);	
		free(locs);

		DvFree(dv);
	}
	{///////////////////////////////////////////////////////////////////
		printf("\tCase: From Nowhere. Dracula hasn't made a move yet.\n");
		char *trail =
			"GGE.... SGE.... HGE.... MGE....";
		Message messages[4] = {};
		DraculaView dv = DvNew(trail, messages);
		printf("\t\tAll connections...\n");
		int numLocs = -1;
		PlaceId *locs = DvWhereCanIGo(dv, &numLocs);
		assert(numLocs == 0);
		assert(locs == NULL);
		free(locs);

		printf("\t\tBoat connections...\n");
		numLocs = -1;
		locs = DvWhereCanIGoByType(dv, false, true, &numLocs);
		assert(numLocs == 0);
		assert(locs == NULL);
		free(locs);

		printf("\t\tRoad connections...\n");		
		numLocs = -1;
		locs = DvWhereCanIGoByType(dv, true, false, &numLocs);
		assert(numLocs == 0);
		assert(locs == NULL);
		free(locs);

		printf("Test passed!\n");

		DvFree(dv);
	}

	{///////////////////////////////////////////////////////////////////
		printf("Testing DvWhereCanTheyGo & DvWhereCanTheyGoByType.\n");
		char *trail =
			"GST.... SRO.... HRO.... MHA.... DAM.V.. "
			"GST.... SRO.... HRO.... MHA....";
		Message messages[9] = {};
		DraculaView dv = DvNew(trail, messages);

		printf("\tCase: Lord Godalming, from Strausbourg (Round 2).\n");
		printf("\t\tAll connections...\n");
		int numLocs = -1;
		PlaceId *locs = DvWhereCanTheyGo(dv, PLAYER_LORD_GODALMING, &numLocs);

		assert(numLocs == 11);
		sortPlaces(locs, numLocs);
		assert(locs[0] == BRUSSELS);
		assert(locs[1] == COLOGNE);
		assert(locs[2] == FRANKFURT);
		assert(locs[3] == GENEVA);
		assert(locs[4] == LEIPZIG);
		assert(locs[5] == MILAN);
		assert(locs[6] == MUNICH);
		assert(locs[7] == NUREMBURG);
		assert(locs[8] == PARIS);
		assert(locs[9] == STRASBOURG);
		assert(locs[10] == ZURICH);
		free(locs);

		printf("\t\tBoat connections...\n");
		locs = DvWhereCanTheyGoByType(dv, PLAYER_LORD_GODALMING, false, false, true, &numLocs);

		assert(numLocs == 1);
		sortPlaces(locs, numLocs);
		assert(locs[0] == STRASBOURG);

		free(locs);

		printf("\t\tRail connections...\n");
		numLocs = -1;
		locs = DvWhereCanTheyGoByType(dv, PLAYER_LORD_GODALMING, false, true, false, &numLocs);

		assert(numLocs == 6);
		sortPlaces(locs, numLocs);
		assert(locs[0] == COLOGNE);
		assert(locs[1] == FRANKFURT);
		assert(locs[2] == LEIPZIG);
		assert(locs[3] == MILAN);
		assert(locs[4] == STRASBOURG);
		assert(locs[5] == ZURICH);

		free(locs);

		printf("\t\tRoad connections...\n");
		numLocs = -1;
		locs = DvWhereCanTheyGoByType(dv, PLAYER_LORD_GODALMING, true, false, false, &numLocs);

		assert(numLocs == 9);
		sortPlaces(locs, numLocs);
		assert(locs[0] == BRUSSELS);
		assert(locs[1] == COLOGNE);
		assert(locs[2] == FRANKFURT);
		assert(locs[3] == GENEVA);
		assert(locs[4] == MUNICH);
		assert(locs[5] == NUREMBURG);
		assert(locs[6] == PARIS);
		assert(locs[7] == STRASBOURG);
		assert(locs[8] == ZURICH);
		free(locs);

		printf("\tCase: Dr Seward, from Rome (Round 2).\n");
		printf("\t\tAll connections...\n");
		numLocs = -1;
		locs = DvWhereCanTheyGo(dv, PLAYER_DR_SEWARD, &numLocs);

		assert(numLocs == 9);
		sortPlaces(locs, numLocs);
		assert(locs[0] == BARI);
		assert(locs[1] == FLORENCE);
		assert(locs[2] == GENEVA);
		assert(locs[3] == GENOA);
		assert(locs[4] == MILAN);
		assert(locs[5] == NAPLES);
		assert(locs[6] == ROME);
		assert(locs[7] == TYRRHENIAN_SEA);
		assert(locs[8] == ZURICH);
		free(locs);

		printf("\t\tBoat connections...\n");
		numLocs = -1;
		locs = DvWhereCanTheyGoByType(dv, PLAYER_DR_SEWARD, false, false, true, &numLocs);

		assert(numLocs == 2);
		sortPlaces(locs, numLocs);
		assert(locs[0] == ROME);
		assert(locs[1] == TYRRHENIAN_SEA);

		free(locs);

		printf("\t\tRail connections...\n");
		numLocs = -1;
		locs = DvWhereCanTheyGoByType(dv, PLAYER_DR_SEWARD, false, true, false, &numLocs);

		assert(numLocs == 8);
		sortPlaces(locs, numLocs);
		assert(locs[0] == BARI);
		assert(locs[1] == FLORENCE);
		assert(locs[2] == GENEVA);
		assert(locs[3] == GENOA);
		assert(locs[4] == MILAN);
		assert(locs[5] == NAPLES);
		assert(locs[6] == ROME);
		assert(locs[7] == ZURICH);

		free(locs);

		printf("\t\tRoad connections...\n");
		numLocs = -1;
		locs = DvWhereCanTheyGoByType(dv, PLAYER_DR_SEWARD, true, false, false, &numLocs);

		assert(numLocs == 4);
		sortPlaces(locs, numLocs);
		assert(locs[0] == BARI);
		assert(locs[1] == FLORENCE);
		assert(locs[2] == NAPLES);
		assert(locs[3] == ROME);

		free(locs);

		printf("\tCase: Van Helsing, from Rome (Round 2).\n");
		printf("\t\tAll connections...\n");
		numLocs = -1;
		locs = DvWhereCanTheyGo(dv, PLAYER_VAN_HELSING, &numLocs);

		assert(numLocs == 5);
		sortPlaces(locs, numLocs);
		assert(locs[0] == BARI);
		assert(locs[1] == FLORENCE);
		assert(locs[2] == NAPLES);
		assert(locs[3] == ROME);
		assert(locs[4] == TYRRHENIAN_SEA);

		free(locs);

		printf("\t\tBoat connections...\n");
		numLocs = -1;
		locs = DvWhereCanTheyGoByType(dv, PLAYER_VAN_HELSING, false, false, true, &numLocs);

		assert(numLocs == 2);
		sortPlaces(locs, numLocs);
		assert(locs[0] == ROME);
		assert(locs[1] == TYRRHENIAN_SEA);

		free(locs);

		printf("\t\tRail connections...\n");
		numLocs = -1;
		locs = DvWhereCanTheyGoByType(dv, PLAYER_VAN_HELSING, false, true, false, &numLocs);

		assert(numLocs == 1);
		sortPlaces(locs, numLocs);
		assert(locs[0] == ROME);

		free(locs);

		printf("\t\tRoad connections...\n");
		numLocs = -1;
		locs = DvWhereCanTheyGoByType(dv, PLAYER_VAN_HELSING, true, false, false, &numLocs);

		assert(numLocs == 4);
		sortPlaces(locs, numLocs);
		assert(locs[0] == BARI);
		assert(locs[1] == FLORENCE);
		assert(locs[2] == NAPLES);
		assert(locs[3] == ROME);

		free(locs);

		printf("Tests passed!\n");
	}


	return EXIT_SUCCESS;
}
