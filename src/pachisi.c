#include "global.h"// Pretty much should include in all C files.
#include "main.h"// We need this to be able to change the main callback from overworld navigation etc. to Pachisi.
#include "overworld.h"// We need this to run the callback that will restore overworld navigation when we're done.
// The following includes are only used for testing because we'll be making sure things are working by playing sounds.
#include "constants/songs.h"
#include "sound.h"
#include "random.h"


// Forward declarations:

static void CB2_TransitionToPachisi();
static void initPachisiVars();
static void CB2_PollPachisi();
static u8 rollDice();

// Entry point from script:
void playPachisiMinigame(void)
{
// We're going to take over the main game's logic since we don't want you to be able to move around, open start menu, etc.
	SetMainCallback2(CB2_TransitionToPachisi);

}


static void CB2_TransitionToPachisi()
{
	// You could go straight to Pachisi. But a lot of Pokemon routines have some preliminary step,
	// usually for graphics updates I think. So I'm just including this here, and we can at least do our var initialization here if we want, like dice rolls / pos etc.
initPachisiVars();
SetMainCallback2(CB2_PollPachisi);

}

static void initPachisiVars()

{
u8 pos = 0;
}

static u8 rollDice()
{
u8 randRoll = Random() % 6;
randRoll += 1;
return randRoll;
}

static void printOutcome(u8 rollResult)
{
	if (rollResult == 1) {
		//Print 1
		
	}

	if (rollResult == 2) {
		
	}

	if (rollResult == 3) {
		
	}

	if (rollResult == 4) {
		
	}

	if (rollResult == 5) {
		
	}

	if (rollResult == 6) {
		
	}
}


static void CB2_PollPachisi()
{
// This function will be called every frame.
// For testing, we'll just play a noise when the A button or B button is pressed.
//We'll show you how to stop by closing out Pachisi when start is pressed.

	if (JOY_NEW(A_BUTTON)) {


	}
	if (JOY_NEW(START_BUTTON))
	{
		PlaySE(SE_SHINY);
	SetMainCallback2(CB2_ReturnToFieldContinueScript);
	}
}