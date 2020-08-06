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

# define xPastPlays "GSR.... SST.... HMN.... MSZ.... DAS.... GMA.... SMU.... HLO.... MBE.... DBIT... GBO.... SZA.... HEC.... MBE.... DNPT... GMR.... SSJ.... HAO.... MBE.... DROT... GCF.... SBE.... HMS.... MBE.... DD3T... GPA.... SBE.... HMR.... MBE.... DHIT... GPA.... SBE.... HMR.... MBC.... DAS.... GST.... SKL.... HGO.... MBD.... DIO..M. GMU.... SBE.... HTS.... MBE.... DTS..M. GZA.... SBE.... HIO.... MBE.... DMS..M. GSJ.... SBE.... HVA.... MBE.... DAO..M. GSO.... SBE.... HSJ.... MBE.... DLST.M. GSO.... SBE.... HSJ.... MSA.... DSNT... GSA.... SBD.... HVA.... MIO.... DSR.V.. GBE.... SBE.... HIO.... MTS.... DBOT... GBE.... SBE.... HTS.... MMS.... DNAT... GBE.... SBE.... HMS.... MAO.... DPAT... GBE.... SBE.... HAO.... MEC.... DCFT.M. GBE.... SBE.... HAO.... MEC.... DTOT.M. GVI.... SSJ.... HBB.... MAO.... DSRT.V. GMU.... SSO.... HBOT... MBB.... DALT... GMI.... SSZ.... HCFT... MBO.... DGRT.M. GGE.... SVE.... HMR.... MCF.... DCAT.M. GCF.... SGO.... HMS.... MMR.... DAO.... GMR.... STS.... HAO.... MCF.... DNS..M. GMS.... SIO.... HEC.... MPA.... DHAT.M. GAO.... SVA.... HNS.... MST.... DCO.VM. GEC.... SSO.... HHAT... MCOVD.. DSTT.M. GLE.... SBE.... HCO.... MBU.... DMUT.M. GBU.... SBD.... HSTT... MLE.... DMIT... GST.... SPR.... HMUT... MBU.... DVET... GMU.... SNU.... HZA.... MFR.... DGOT... GZA.... SMU.... HSJ.... MST.... DMRT... GSJ.... SST.... HZA.... MMU.... DTOT... GBE.... SMU.... HSZ.... MZA.... DBAT... GVI.... SZA.... HBE.... MMU.... DSRT.M. GVI.... SZA.... HBE.... MMU.... DSNT.M. GVE.... SBD.... HVI.... MST.... DLST.M. GMU.... SBE.... HZA.... MCO.... DCAT.M. GST.... SBE.... HMU.... MPA.... DGR.VM. GCO.... SBE.... HST.... MMR.... DMAT.M. GBU.... SBE.... HCO.... MCF.... DALT.M. GBU.... SBE.... HCO.... MCF.... DMS..M. GSR.... SSJ.... HBU.... MBO.... DAO..M. GMAT... SVA.... HBO.... MSR.... DNS..M. GALT... SIO.... HCF.... MMA.... DHAT.V. GMS.... STS.... HBO.... MAL.... DLIT... GAL.... SMS.... HSR.... MMS.... DNUT... GSR.... SAL.... HMA.... MAO.... DMUT... GMA.... SSR.... HPA.... MEC.... DZAT... GBO.... SMR.... HST.... MNS.... DSZT... GCF.... SGE.... HMUT... MHAT... DBET... GMR.... SMI.... HZAT... MVI.... DBC.VM. GMS.... SMU.... HSJ.... MZA.... DKLT.M. GMR.... SZA.... HBET... MBD...."

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
