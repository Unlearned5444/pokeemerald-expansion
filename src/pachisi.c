#include "global.h"
#include "main.h"
#include "overworld.h"
#include "random.h"
#include "window.h"
#include "field_message_box.h"

// Forward declarations:

static void CB2_TransitionToPachisi();
static void initPachisiVars();
static void CB2_PollPachisi();
static u8 rollDice();
static void printOutcome(u8 rollResult);

// Entry point from script:
void playPachisiMinigame(void)
{
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
const u8 Text_roll1[] = _("You rolled a 1.{PAUSE_UNTIL_PRESS}");
const u8 Text_roll2[] = _("You rolled a 2.{PAUSE_UNTIL_PRESS}");
const u8 Text_roll3[] = _("You rolled a 3.{PAUSE_UNTIL_PRESS}");
const u8 Text_roll4[] = _("You rolled a 4.{PAUSE_UNTIL_PRESS}");
const u8 Text_roll5[] = _("You rolled a 5.{PAUSE_UNTIL_PRESS}");
const u8 Text_roll6[] = _("You rolled a 6.{PAUSE_UNTIL_PRESS}");
}

static u8 rollDice()
{
u8 randRoll = Random() % 6;
randRoll += 1;
return randRoll;
}

static void printOutcome(u8 rollResult)
{
	void InitFieldMessageBox(void);

	if (rollResult == 1) {
		bool8 ShowFieldAutoScrollMessage(const u8 *Text_roll1);	
	}

	if (rollResult == 2) {
		bool8 ShowFieldAutoScrollMessage(const u8 *Text_roll2);	

	}

	if (rollResult == 3) {
		bool8 ShowFieldAutoScrollMessage(const u8 *Text_roll3);	

	}

	if (rollResult == 4) {
		bool8 ShowFieldAutoScrollMessage(const u8 *Text_roll4);	
	}

	if (rollResult == 5) {
		bool8 ShowFieldAutoScrollMessage(const u8 *Text_roll5);	
	}

	if (rollResult == 6) {
		bool8 ShowFieldAutoScrollMessage(const u8 *Text_roll6);
	}
}


static void CB2_PollPachisi()
{


	if (JOY_NEW(A_BUTTON)) {
		printOutcome(u8 rollDice());
			}
	if (JOY_NEW(START_BUTTON))
	{
		PlaySE(SE_SHINY);
	SetMainCallback2(CB2_ReturnToFieldContinueScript);
	}
}