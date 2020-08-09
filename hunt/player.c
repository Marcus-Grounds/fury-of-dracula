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

# define xPastPlays "GBU.... SMA.... HVE.... MGA.... DC?.V.. GPA.... SBA.... HVE.... MCD.... DC?T... GSR.... SMS.... HMU.... MGA.... DHIT... GLE.... SMS.... HNU.... MKL.... DD1T... GEC.... SMS.... HPR.... MKL.... DS?.... GNS.... SMS.... HNU.... MKL.... DC?T... GHA.... SMS.... HPR.... MKL.... DC?T.V. GCO.... SMS.... HPR.... MKL.... DC?T.M. GLI.... SMS.... HHA.... MKL.... DHIT.M. GLI.... SMS.... HCO.... MKL.... DD1T.M. GCO.... SMS.... HLI.... MKL.... DLOT... GBO.... SMS.... HLI.... MKL.... DC?T.M. GSR.... SMS.... HCO.... MKL.... DLVT.M. GSR.... SMS.... HBO.... MKL.... DMN.VM. GBA.... SMS.... HSR.... MKL.... DHIT.M. GMS.... SMS.... HSR.... MKL.... DD1T.M. GMS.... SMS.... HBA.... MKL.... DLOT.M. GMS.... SMS.... HMS.... MKL.... DSWT.M. GMS.... SMS.... HMS.... MKL.... DLVT.M. GMS.... SMS.... HMS.... MKL.... DMNT.V. GMS.... SMS.... HMS.... MKL.... DHIT.M. GMS.... SMS.... HMS.... MKL.... DD1T.M. GMS.... SMS.... HMS.... MKL.... DLOT.M. GMS.... SMS.... HMS.... MKL.... DSWT.M. GMS.... SMS.... HMS.... MKL.... DLVT.M. GMS.... SMS.... HMS.... MKL.... DMNT.M. GMS.... SMS.... HMS.... MKL.... DHI.VM. GMS.... SMS.... HMS.... MKL.... DD1T.M. GMS.... SMS.... HMS.... MKL.... DLOT.M. GMS.... SMS.... HMS.... MKL.... DSWT.M. GMS.... SMS.... HMS.... MKL.... DC?T.M. GMS.... SMS.... HMS.... MKL.... DC?T.M. GMS.... SMS.... HMS.... MKL.... DHIT.V. GMS.... SMS.... HMS.... MKL.... DD1T.M. GMS.... SMS.... HMS.... MKL.... DC?T.M. GMS.... SMS.... HMS.... MBD.... DC?T.M. GBA.... SBA.... HBA.... MSO.... DC?T.M. GAL.... SBO.... HBO.... MSA.... DC?T.M. GMA.... SBB.... HCF.... MSO.... DHIT.M. GBO.... SAO.... HGE.... MSO.... DD1.VM. GCF.... SAO.... HCF.... MBE.... DC?T.M. GGE.... SAO.... HGE.... MKL.... DS?..M. GCF.... SAO.... HMR.... MBD.... DC?T.M. GGE.... SAO.... HCF.... MBE.... DC?T.M. GMR.... SAO.... HGE.... MSO.... DC?T.M. GCF.... SAO.... HGE.... MSA.... DHIT.V. GGE.... SAO.... HMR.... MSO.... DD1T.M. GGE.... SAO.... HCF.... MSO.... DC?T... GMR.... SAO.... HGE.... MBE.... DC?T.M. GCF.... SAO.... HGE.... MKL.... DC?T.M. GGE.... SAO.... HMR.... MBD.... DC?T.M. GGE.... SAO.... HCF.... MBE.... DC?T.M. GMR.... SAO.... HGE.... MSO.... DKL.VM. GCF.... SAO.... HGE.... MSA.... DCDT.M. GGE.... SAO.... HMR.... MSO.... DHIT.M. GGE.... SAO.... HCF.... MSO.... DD1T.M. GMR.... SAO.... HGE.... MBE.... DC?T.M. GCF.... SAO.... HGE.... MKLV... DC?T.M. GGE.... SAO.... HMR.... MBD.... DC?T... GGE.... SAO.... HCF.... MBE.... DSOT.M. GMR.... SAO.... HGE.... MSOTD.. DC?T.M. GCF.... SAO.... HGE.... MSA.... DKLT.M. GGE.... SAO.... HMR.... MSO.... DCDT.M. GGE.... SAO.... HCF.... MSO.... DHIT.M. GMR.... SAO.... HGE.... MBE.... DD1T.M. GCF.... SAO.... HGE.... MKLT... DC?.V.. GGE.... SAO.... HMR.... MBD.... DC?T.M. GGE.... SAO.... HCF.... MBE.... DC?T... GMR.... SAO.... HGE.... MSO.... DSOT.M. GCF.... SAO.... HGE.... MSA.... DC?T.M. GGE.... SAO.... HMR.... MSOT... DD5T.M. GGE.... SAO.... HCF.... MSO.... DCDT.V. GMR.... SAO.... HGE.... MBE.... DHIT.M. GCF.... SAO.... HGE.... MKL.... DC?T.M. GGE.... SAO.... HMR.... MBD.... DC?T... GGE.... SAO.... HCF.... MBE.... DC?T.M. GMR.... SAO.... HGE.... MSO.... DD2T.M. GCF.... SAO.... HGE.... MSA.... DC?T.M. GGE.... SAO.... HMR.... MSO.... DHI.VM. GGE.... SAO.... HCF.... MSO.... DGAT.M. GMR.... SAO.... HGE.... MBE.... DCDT.M. GCF.... SAO.... HGE.... MKL.... DKLT.M. GGE.... SAO.... HMR.... MBD.... DD2T.M. GGE.... SAO.... HCF.... MBE.... DTPT.M. GCF.... SGW.... HNA.... MGAT... DHI..V. GNA.... SAO.... HBB...."
# define xMsgs { "", "", "", "" }

#else

typedef HunterView View;

# define ViewNew HvNew
# define decideMove decideHunterMove
# define ViewFree HvFree

# define xPastPlays "GKL.... SMR.... HLS.... MAM.... DCD.V.. GCDVD.. SMI.... HAO.... MCO.... DD1T... GCDTD.. SMU.... HNS.... MFR.... DHIT."
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
