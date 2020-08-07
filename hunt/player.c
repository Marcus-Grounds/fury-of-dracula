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

# define xPastPlays "GCD.... SGA.... HFR.... MSR.... DLV.V.. GGA.... SCD.... HBR.... MBA.... DSWT... GCD.... SGA.... HPR.... MSR.... DHIT... GGA.... SGA.... HVI.... MBA.... DD1T... GGA.... SGA.... HZA.... MLS.... DLOT... GGA.... SCD.... HMU.... MMA.... DMNT... GGA.... SCD.... HMU.... MMA.... DEDT.V. GBD.... SKL.... HMI.... MAL.... DNS..M. GVI.... SBC.... HRO.... MAL.... DAMT.M. GMU.... SBD.... HRO.... MMS.... DHIT.M. GMU.... SBD.... HRO.... MMS.... DD1T.M. GZA.... SZA.... HFL.... MMR.... DBUT.M. GZA.... SZA.... HMI.... MMI.... DLET.M. GZA.... SZA.... HMU.... MMU.... DNA.V.. GZA.... SZA.... HMU.... MMU.... DBB..M. GMU.... SSJ.... HVE.... MLI.... DSNT.M. GST.... SZA.... HFL.... MBUT... DHIT.M. GBU.... SVI.... HGE.... MAM.... DD1T... GBO.... SHA.... HCF.... MBU.... DSRT.M. GLS.... SNS.... HBO.... MBO.... DTOT.V. GLS.... SAO.... HBA.... MLS.... DCFT... GLS.... SAO.... HBA.... MLS.... DGET.M. GSRT... SBB.... HMS.... MMA.... DZUT.M. GAL.... SAO.... HTS.... MAL.... DMUT.M. GMA.... SLS.... HMS.... MLS.... DZAT... GLS.... SLS.... HAO.... MLS.... DSJT.M. GLS.... SLS.... HLS.... MLS.... DBE.VM. GLS.... SLS.... HLS.... MAO.... DKLT.M. GAO.... SAO.... HAO.... MIR.... DCDT.M. GMS.... SMS.... HMS.... MAO.... DHIT.M. GTS.... SMR.... HTS.... MMS.... DD1T.M. GIO.... SGO.... HIO.... MTS.... DGAT.M. GBS.... SVE.... HSA.... MGO.... DBCT.V. GCN.... SBD.... HBE.... MVE.... DSOT.M. GBCT... SBE.... HBE.... MVI.... DVAT.M. GBE.... SBE.... HBE.... MBD.... DATT.M. GSJ.... SSJ.... HSA.... MSOT... DHIT.M. GZA.... SZA.... HIO.... MSJ.... DD1T.M. GMU.... SMU.... HAS.... MZA.... DIO.... GFR.... SVE.... HBI.... MVI.... DSA.V.. GNU.... SMU.... HAS.... MLI.... DSOT.M. GNU.... SNU.... HVE.... MNU.... DVRT.M. GNU.... SNU.... HMU.... MNU.... DBS..M. GCO.... SLI.... HVE.... MNU.... DCNT.M. GCO.... SLI.... HVE.... MHA.... DD3T... GAM.... SHA.... HBR.... MNS.... DHIT.V. GNS.... SNS.... HHA.... MAO...."
# define xMsgs { "", "", "", "" }

#else

typedef HunterView View;

# define ViewNew HvNew
# define decideMove decideHunterMove
# define ViewFree HvFree

# define xPastPlays "GZA.... SED.... HZU...."
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
