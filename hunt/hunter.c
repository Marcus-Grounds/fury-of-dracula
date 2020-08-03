////////////////////////////////////////////////////////////////////////
// COMP2521 20T2 ... the Fury of Dracula
// hunter.c: your "Fury of Dracula" hunter AI.
//
// 2014-07-01	v1.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2017-12-01	v1.1	Team Dracula <cs2521@cse.unsw.edu.au>
// 2018-12-31	v2.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2020-07-10	v3.0	Team Dracula <cs2521@cse.unsw.edu.au>
//
////////////////////////////////////////////////////////////////////////

#include "Game.h"
#include "hunter.h"
#include "HunterView.h"

void decideHunterMove(HunterView hv)
{
	// TODO: Replace this with something better!
	Round round = HvGetRound(hv);
	Player player = HvGetPlayer(hv);
	PlaceId lastSeenDrac = HvGetLastKnownDraculaLocation(hv, &round);

	if (round == 0) {
		switch (player) {
            case PLAYER_LORD_GODALMING:
                registerBestPlay(placeIdToAbbrev(MUNICH), "Godalming to rescue");
                return;
            case PLAYER_DR_SEWARD:
               registerBestPlay(placeIdToAbbrev(MARSEILLES), "Time to go SewWards");
                return;
            case PLAYER_VAN_HELSING:
                registerBestPlay(placeIdToAbbrev(VIENNA), "See you in HEL dracula");
                return;
            case PLAYER_MINA_HARKER:
               registerBestPlay(placeIdToAbbrev(BERLIN), "Down to hunt!");
                return;
			case PLAYER_DRACULA:
				return;
        }


	}

	//If Dracula's last known position is unknown, 
	//The hunters would rest to find the location 
	if (lastSeenDrac == UNKNOWN && round % 6 == 0 ) {
		registerBestPlay(placeIdToAbbrev(HvGetPlayerLocation(hv, player)),
				"Rest y'all, we gotta find the blood sucking villain");
	}

	//Rest a round if health is too low 
	//TODO: Add conditions
	if (HvGetHealth(hv, player) < 2) {
		registerBestPlay(placeIdToAbbrev(HvGetPlayerLocation(hv, player)), "Health is too low, have to rest");
	}

}


//IF the hunter finds himself on the trail of dracula
//By falling prey to his traps
void Ontrail(HunterView hv) {


}