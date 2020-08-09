////////////////////////////////////////////////////////////////////////
// Runs a player's game turn ...
//
// Can  produce  either a hunter player or a Dracula player depending on
// the setting of the I_AM_DRACULA #define
//
// This  is  a  dummy  version of the real player.c used when you submit
// your AIs. It is provided so that you can test whether  your  code  is
// likely to compile ...
//
// Note that this is used to drive both hunter and Dracula AIs. It first
// creates an appropriate view, and then invokes the relevant decideMove
// function,  which  should  use the registerBestPlay() function to send
// the move back.
//
// The real player.c applies a timeout, and will halt your  AI  after  a
// fixed  amount of time if it doesn 't finish first. The last move that
// your AI registers (using the registerBestPlay() function) will be the
// one used by the game engine. This version of player.c won't stop your
// decideMove function if it goes into an infinite loop. Sort  that  out
// before you submit.
//
// Based on the program by David Collien, written in 2012
//
// 2017-12-04	v1.1	Team Dracula <cs2521@cse.unsw.edu.au>
// 2018-12-31	v1.2	Team Dracula <cs2521@cse.unsw.edu.au>
// 2020-07-10	v1.3	Team Dracula <cs2521@cse.unsw.edu.au>
//
////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Game.h"
#ifdef I_AM_DRACULA
# include "dracula.h"
# include "DraculaView.h"
#else
# include "hunter.h"
# include "HunterView.h"
#endif

// Moves given by registerBestPlay are this long (including terminator)
#define MOVE_SIZE 3

// The minimum static globals I can get away with
static char latestPlay[MOVE_SIZE] = "";
static char latestMessage[MESSAGE_SIZE] = "";

// A pseudo-generic interface, which defines
// - a type `View',
// - functions `ViewNew', `decideMove', `ViewFree',
// - a trail `xtrail', and a message buffer `xmsgs'.
#ifdef I_AM_DRACULA

typedef DraculaView View;

# define ViewNew DvNew
# define decideMove decideDraculaMove
# define ViewFree DvFree

# define xPastPlays "GKL.... SMR.... HLS.... MAM.... DAT.V.. GBD.... SGO.... HCA.... MBU.... DC?T... GVI.... SVE.... HGR.... MPA.... DSJT... GZA.... SMU.... HAL.... MGE.... DC?T... GSJT... SMI.... HGR.... MST.... DC?T... GZA.... SGO.... HAL.... MZU.... DC?T... GZA.... SGO.... HAL.... MZU.... DBCT.V. GVI.... STS.... HMS.... MST.... DC?T.M. GMU.... SRO.... HTS.... MZU.... DC?T... GMI.... STS.... HRO.... MST.... DC?T.M. GGO.... SRO.... HFL.... MZU.... DC?T.M. GZU.... SFL.... HVE.... MST.... DC?T.M. GZU.... SFL.... HVE.... MST.... DBCT.M. GST.... SVE.... HSZ.... MZU.... DC?.VM. GPA.... SSZ.... HZA.... MST.... DC?T.M. GMR.... SZA.... HVI.... MPA.... DC?T.M. GCF.... SVI.... HMU.... MMR.... DC?T.M. GTO.... SMU.... HZA.... MCF.... DC?T.M. GTO.... SMU.... HZA.... MCF.... DBCT.M. GSR.... SMI.... HVI.... MTO.... DC?T.V. GTO.... SMR.... HMU.... MSR.... DC?T.M. GSR.... STO.... HZA.... MTO.... DD2T.M. GMA.... SSR.... HVI.... MSR.... DSZT.M. GLS.... STO.... HVE.... MMA.... DC?T.M. GLS.... STO.... HVE.... MMA.... DSOT.M. GSN.... SSR.... HSZT... MSR.... DC?T.M. GBB.... SPA.... HZA.... MMA.... DC?.VM. GBO.... SST.... HVI.... MSR.... DC?T.M. GBB.... SZU.... HMU.... MPA.... DS?.... GBO.... SST.... HZA.... MST.... DATT.M. GBO.... SST.... HZA.... MST.... DD1T.M. GPA.... SZU.... HVI.... MPA.... DHIT.M. GST.... SST.... HMU.... MMR.... DC?T.V. GZU.... SPA.... HZA.... MCF.... DC?T.M. GST.... SMR.... HVI.... MTO.... DC?T... GZU.... SCF.... HVE.... MSR.... DS?..M. GZU.... SCF.... HVE.... MSR.... DATT.M. GST.... STO.... HSZ.... MTO.... DD1T.M. GPA.... SSR.... HZA.... MSR.... DC?T.M. GMR.... STO.... HVI.... MMA.... DC?.VM. GCF.... SSR.... HMU.... MLS.... DC?T.M. GTO.... SMA.... HZA.... MMA.... DC?T... GTO.... SMA.... HZA.... MMA.... DVRT.M. GSR.... SSR.... HVI.... MSR.... DC?T.M. GTO.... SMA.... HMU.... MPA.... DC?T.M. GSR.... SSR.... HZA.... MST.... DC?T.V. GMA.... SPA.... HVI.... MZU.... DC?T.M. GLS.... SST.... HVE.... MST.... DC?T.M. GLS.... SST.... HVE.... MST.... DBCT.M. GSN.... SPA.... HSZ.... MZU.... DC?T.M. GBB.... SMR.... HZA.... MST.... DC?T.M. GBO.... SCF.... HVI.... MPA.... DS?..M. GBB.... STO.... HMU.... MMR.... DS?..M. GBO.... SSR.... HZA.... MCF.... DC?T.M. GBO.... SSR.... HZA.... MCF.... DSOT.M. GPA.... STO.... HVI.... MTO.... DC?T.M. GST.... SSR.... HMU.... MSR.... DC?T.M. GZU.... SMA.... HZA.... MTO.... DC?T... GST.... SLS.... HVI.... MSR.... DKLT... GZU.... SMA.... HVE.... MMA.... DC?T.M. GZU.... SMA.... HVE.... MMA.... DCDT.M. GMI.... SBO.... HBD.... MAL.... DD1T.M. GVE.... SMR.... HKLT... MMS.... DHIT.M. GBD.... SGO.... HCDTTTD MTS.... DTPT.M. GKL.... SVE.... HSJ.... MIO.... DKLT... GKLTD.. SAS.... HZA.... MVA.... DBD.VM. GKL.... SAS.... HZA.... MVA.... DSZT... GSZTD.. SBI.... HVI.... MIO.... DC?T... GSZ.... SRO.... HMU.... MVA.... DC?T... GBDV... STS.... HZA.... MIO.... DC?T.M. GVI.... SRO.... HVI.... MVA.... DC?T... GSZ.... SFL.... HVE.... MIO.... DC?T... GSZ.... SFL.... HVE.... MIO.... DGAT... GBD.... SVE.... HSZ.... MVA.... DCDT.M. GKL.... SBD.... HKL.... MSA.... DD1T.M. GCDTTD. SKL.... HCDD... MBE.... DHIT.M. GCDT... SCDD... HCDD... MGAT... DC?T.M."
# define xMsgs { "", "", "", "" }

#else

typedef HunterView View;

# define ViewNew HvNew
# define decideMove decideHunterMove
# define ViewFree HvFree

# define xPastPlays "GKL.... SMR.... HLS.... MAM.... DCD.V.. GCDVD.. SMI.... HAO.... MCO.... DD1T..."
# define xMsgs { "", "", "" }

#endif

int main(void)
{
	char *pastPlays = xPastPlays;
	Message msgs[] = xMsgs;

	View state = ViewNew(pastPlays, msgs);
	decideMove(state);
	ViewFree(state);
	
	printf("Move: %s, Message: %s\n", latestPlay, latestMessage);
	return EXIT_SUCCESS;
}

// Saves characters from play (and appends a terminator)
// and saves characters from message (and appends a terminator)
void registerBestPlay(char *play, Message message)
{
	strncpy(latestPlay, play, MOVE_SIZE - 1);
	latestPlay[MOVE_SIZE - 1] = '\0';

	strncpy(latestMessage, message, MESSAGE_SIZE - 1);
	latestMessage[MESSAGE_SIZE - 1] = '\0';
}
