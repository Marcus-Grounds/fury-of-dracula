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

# define xPastPlays "GKL.... SMR.... HLS.... MAM.... DAT.V.. GBD.... SGO.... HCA.... MBU.... DHIT... GVI.... SVE.... HGR.... MPA.... DD1T... GZA.... SMU.... HAL.... MGE.... DVAT... GSJ.... SNU.... HSR.... MZU.... DSAT... GVAT... SMU.... HAL.... MMU.... DSOT... GSAT... SZA.... HMS.... MVI.... DBCT.V. GSOT... SSJ.... HTS.... MZA.... DGAT.M. GSO.... SZA.... HMS.... MSJ.... DCDT.M. GVR.... SMU.... HAO.... MZA.... DHIT... GCN.... SNU.... HLS.... MMU.... DD1T... GGAT... SFR.... HCA.... MVI.... DKLT... GGA.... SCO.... HGR.... MPR.... DBET.M. GBC.... SBU.... HAL.... MBR.... DSJ.V.. GKLT... SLE.... HSR.... MPR.... DVAT.M. GBD.... SNA.... HSN.... MBD.... DATT.M. GVI.... SBB.... HAL.... MVI.... DHIT.M. GVE.... SBO.... HMS.... MMU.... DD1T... GAS.... SPA.... HTS.... MMI.... DIO..M. GIO.... SCF.... HMS.... MVE.... DSAT.V. GAS.... SMR.... HAL.... MMU.... DSOT.M. GVE.... SGO.... HSR.... MVI.... DVRT.M. GBD.... SVE.... HSN.... MPR.... DHIT.M. GVI.... SMU.... HLS.... MBR.... DD1T.M. GZA.... SNU.... HCA.... MHA.... DBS.... GSJ.... SFR.... HGR.... MCO.... DCNT.M. GVA.... SCO.... HAL.... MAM.... DGA.VM. GSA.... SBU.... HSR.... MBU.... DCDT.M. GSO.... SLE.... HSN.... MPA.... DHIT.M. GVRT... SNA.... HLS.... MGE.... DD1T... GVR.... SBO.... HCA.... MZU.... DKLT... GCNT... STO.... HGR.... MMI.... DBET... GCN.... SMR.... HAL.... MVE.... DSOT.V. GGA.... SGO.... HSR.... MMU.... DBCT.M. GBCTD.. STS.... HAL.... MVI.... DGAT.M. GBC.... SIO.... HMS.... MZA.... DCDT.M. GBC.... STS.... HAL.... MMU.... DHIT.M. GKL.... SGO.... HSR.... MVI.... DD1T.M. GBD.... SVE.... HSN.... MPR.... DKLT.M. GVI.... SMU.... HLS.... MBR.... DBC.V.. GZA.... SNU.... HCA.... MHA.... DGAT.M. GSJ.... SFR.... HGR.... MCO.... DCDT.M. GVA.... SCO.... HAL.... MAM.... DHIT.M. GSA.... SBU.... HSR.... MBU.... DD1T.M. GSO.... SLE.... HSN.... MPA.... DKLT.M. GVR.... SNA.... HLS.... MGE.... DBCT.V. GCN.... SBO.... HCA.... MZU.... DGAT.M. GGATD.. SBB.... HAO.... MGO.... DCDT.M. GGA.... SBO.... HLS.... MST.... DHIT.M. GGA.... STO.... HCA.... MBU.... DD1T.M. GBCT... SMR.... HGR.... MPA.... DKLT.M. GKLTD.. SGO.... HAL.... MGE.... DBCT... GSJ.... STS.... HMS.... MFL.... DGA.V.. GVA.... SGO.... HAO.... MVE.... DCDT.M. GSA.... SVE.... HLS.... MMU.... DHIT.M. GSO.... SMU.... HCA.... MVI.... DD1T.M. GVR.... SNU.... HGR.... MPR.... DKLT... GCN.... SFR.... HAL.... MBR.... DBET.M. GGAV... SCO.... HSR.... MHA.... DSOT... GBC.... SBU.... HSN.... MCO.... DVRT.M. GKLT... SLE.... HLS.... MBR.... DBS..M. GBD.... SBU.... HAL.... MPR.... DD1..M. GVI.... SSR.... HMA.... MNU.... DCNT... GSZ.... STO.... HSR.... MMU.... DHIT.M. GKL.... SMR.... HSN.... MVI.... DGAT.M. GBD.... SGO.... HLS.... MPR.... DCD.VM. GVI.... SVE.... HCA.... MBR.... DKLT... GZA.... SMU.... HGR.... MHA.... DD2T... GSJ.... SNU.... HAL.... MCO.... DTPT.M. GVA.... SFR.... HSR.... MAM.... DHI..M. GSA.... SCO.... HSN.... MBU..."
# define xMsgs { "", "", "", "" }

#else

typedef HunterView View;

# define ViewNew HvNew
# define decideMove decideHunterMove
# define ViewFree HvFree

# define xPastPlays "GKL.... SMR.... HLS.... MAM.... DC?.V.. GBD.... SGO.... HCA.... MBU.... DHIT... GVI.... SVE.... HGR.... MPA.... DD1T... GZA.... SMU.... HAL.... MGE.... DVAT... GSJ.... SNU.... HSR.... MZU.... DSAT... GVAT... SMU.... HAL.... MMU.... DC?T... GSAT... SZA.... HMS.... MVI.... DC?T.V."
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
