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
		//printf("%d\n", traps[1]);
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
		for (int i = 0; i < numTraps; i++) {
			printf("TRAP NUMBER: %d,TRAP LOC: %d\n", i, traps[i]);
		}
		
		/*assert(numTraps == 3);
		sortPlaces(traps, numTraps);
		assert(traps[0] == LONDON);
		assert(traps[1] == LONDON);
		assert(traps[2] == MANCHESTER);*/
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
	
		printf("Test for DvWhereCanIGo & DvWhereCanIGoByType\n");
		printf("\tDouble back and hide is invalid.\n");
		char *trail =
			"GGE.... SGE.... HGE.... MGE.... DKL.V.. "
			"GGE.... SGE.... HGE.... MGE.... DD1T... "
			"GGE.... SGE.... HGE.... MGE.... DBCT... "
			"GGE.... SGE.... HGE.... MGE.... DHIT... "
			"GGE.... SGE.... HGE.... MGE....";
		
		Message messages[24] = {};
		DraculaView dv = DvNew(trail, messages);

		int numLocs = -1;
		PlaceId *locs = DvWhereCanIGo(dv, &numLocs);
		printf("NUMLOCS: %d\n\n\n", numLocs);
		assert(numLocs == 4);
		sortPlaces(locs, numLocs);
		assert(locs[0] == BELGRADE);
		assert(locs[1] == CONSTANTA);
		assert(locs[2] == GALATZ);
		assert(locs[3] == SOFIA);							
		free(locs);

		numLocs = -1;
		locs = DvWhereCanIGoByType(dv, false, true, &numLocs);
		assert(numLocs == 0);
		assert(locs == NULL);											
		free(locs);

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
		printf("\tCase: Double back is valid.\n");
		char *trail =
			"GGE.... SGE.... HGE.... MGE.... DZU.V.. "
			"GGE.... SGE.... HGE.... MGE.... DSTT... "
			"GGE.... SGE.... HGE.... MGE.... DPAT... "
			"GGE.... SGE.... HGE.... MGE.... DCFT... "
			"GGE.... SGE.... HGE.... MGE....";
		
		Message messages[24] = {};
		DraculaView dv = DvNew(trail, messages);

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
		DvFree(dv);
	}
	
	{///////////////////////////////////////////////////////////////////
		printf("\tCase: Doubleback is invalid, hide is valid, Dracula is at sea\n.");
		char *trail =
			"GGE.... SGE.... HGE.... MGE.... DAL.V.. "
			"GGE.... SGE.... HGE.... MGE.... DGRT... "
			"GGE.... SGE.... HGE.... MGE.... DD2T... "
			"GGE.... SGE.... HGE.... MGE.... DMST... "
			"GGE.... SGE.... HGE.... MGE....";
		
		Message messages[24] = {};
		DraculaView dv = DvNew(trail, messages);

		int numLocs = -1;
		PlaceId *locs = DvWhereCanIGo(dv, &numLocs);
		assert(numLocs == 5);
		sortPlaces(locs, numLocs);
		assert(locs[0] == ATLANTIC_OCEAN);
		assert(locs[1] == BARCELONA);
		assert(locs[2] == CAGLIARI);
		assert(locs[3] == MARSEILLES);
		assert(locs[4] == TYRRHENIAN_SEA);
		DvFree(dv);
	}

	{///////////////////////////////////////////////////////////////////
		printf("\tCase: Doubleback is invalid, hide is valid, Dracula is on land\n.");
		char *trail =
			"GGE.... SGE.... HGE.... MGE.... DPA.V.. "
			"GGE.... SGE.... HGE.... MGE.... DLET... "
			"GGE.... SGE.... HGE.... MGE.... DNAT... "
			"GGE.... SGE.... HGE.... MGE.... DD3T... "
			"GGE.... SGE.... HGE.... MGE....";
		
		Message messages[24] = {};
		DraculaView dv = DvNew(trail, messages);

		int numLocs = -1;
		PlaceId *locs = DvWhereCanIGo(dv, &numLocs);
		assert(numLocs == 5);
		sortPlaces(locs, numLocs);
		assert(locs[0] == BRUSSELS);
		assert(locs[1] == CLERMONT_FERRAND);
		assert(locs[2] == GENEVA);
		assert(locs[3] == PARIS);
		assert(locs[4] == STRASBOURG);

		printf("Test passed!\n");
		DvFree(dv);
	}
	
	{///////////////////////////////////////////////////////////////////
	
		printf("Test for DvGetValidMoves 1\n"); 
		printf("Case: Dracula has not made any moves\n");
		
		char *trail =
			"GGE.... SGE.... HGE.... MGE....";
		
		Message messages[9] = {};
		DraculaView dv = DvNew(trail, messages);
		
		int numMoves = -1;
		PlaceId *moves = DvGetValidMoves(dv, &numMoves);
		assert(numMoves == 0);
		assert (moves == NULL);
		printf("Test passed!\n");
		DvFree(dv);
	} 

	{///////////////////////////////////////////////////////////////////
	
		printf("Test for DvGetValidMoves 2\n"); 
		printf("Case: Dracula has made 1 location move at city\n");
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
		
		printf("Test passed!\n");
		DvFree(dv);
		
	} 
	{///////////////////////////////////////////////////////////////////
	
		printf("Test for DvGetValidMoves 2\n"); 
		printf("Case: Dracula has made 1 location move at sea\n");
		
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
		
		printf("Test passed!\n");
		DvFree(dv);
	} 

	{///////////////////////////////////////////////////////////////////
	
		printf("Test for DvGetValidMoves when Dracula has made 2 location moves at city\n");
		
	} 

	{///////////////////////////////////////////////////////////////////
	
		printf("Test for DvGetValidMoves when Dracula has made 2 moves including HIDE\n");
		
	} 
	{///////////////////////////////////////////////////////////////////
	
		printf("Test for DvGetValidMoves when Dracula has made 2 moves including DOUBLE_BACK_1\n");
		
	} 
	{///////////////////////////////////////////////////////////////////
	
		printf("Test for DvGetValidMoves when Dracula has no adjacent locations to DOUBLE_BACK to\n");
		
	} 
	{///////////////////////////////////////////////////////////////////
	
		printf("Test for DvGetValidMoves when Dracula has a short trail\n");
		
	} 
	

	return EXIT_SUCCESS;
}
