#include "global.h"
#include "main.h"
#include "overworld.h"
#include "constants/songs.h"
#include "random.h"
#include "sound.h"
#include "window.h"
#include "field_message_box.h"
#include "task.h"
#include "text.h"
#include "string_util.h"
#include "pachisi_data/pachisi_defines.h"
#include "pachisi_data/PachisiSquare.h"
 
EWRAM_DATA u16 pos;
EWRAM_DATA u8 lastRoll;
const EWRAM_DATA struct PachisiSquare* currentBoard=NULL;
static EWRAM_DATA const u8* gPachisiCurrentText=NULL;
void task_PachisiMessageBox(u8 taskId);// Forward declare.


const u8 Text_roll1[] = _("You rolled a 1.");
const u8 Text_roll2[] = _("You rolled a 2.");
const u8 Text_roll3[] = _("You rolled a 3.");
const u8 Text_roll4[] = _("You rolled a 4.");
const u8 Text_roll5[] = _("You rolled a 5.");
const u8 Text_roll6[] = _("You rolled a 6.");
const u8 text_current_pos[] = _("Pos is {STR_VAR_1}");


void pachici_MessageBox(const u8* str)
{
	u8 taskId=CreateTask(task_PachisiMessageBox, 8); /* Second param is priority. Idk. I just set it to a number I've seen used. */
	gTasks[taskId].data[0]=0;
	gPachisiCurrentText=str;


}
void task_PachisiMessageBox(u8 taskId)
{
	// Get the state:
	s16* state=&gTasks[taskId].data[0];
	if(*state==0)
	{
		//InitFieldMessageBox();// Try uncommenting this line as your first troubleshooting step.


		//ShowFieldMessage(gPachisiCurrentText);
		gTextFlags.autoScroll = TRUE;
		ShowFieldAutoScrollMessage(gPachisiCurrentText);


		(*state)++;


	}
	if(*state==1)
	{
		// We're done only when the printer finishes.
		if(GetFieldMessageBoxMode()==FIELD_MESSAGE_BOX_HIDDEN)
		{
			// Set the global string to null, that'll be how we can tell that we can continue pachisi for now.
			gPachisiCurrentText=NULL;
			// Destroy our task. We can eventually switch the task's existance as being how we can tell if we're done, this way's just easier.
			DestroyTask(taskId);
		}
	}
}


u8 pachisiMessageBoxActive()
{
	return (gPachisiCurrentText!=0);


}


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
pos = 0;
lastRoll = 0;
PlayBGM(MUS_PACHISI);

}


static u8 rollDice()
{
u8 randRoll = Random() % 6;
randRoll += 1;
return randRoll;
}


static void printOutcome(u8 rollResult)
{
	
	// void InitFieldMessageBox(void);


	if (rollResult == 1) {
		pachici_MessageBox(Text_roll1);
	}


	if (rollResult == 2) {
		pachici_MessageBox(Text_roll2);
	}


	if (rollResult == 3) {
		pachici_MessageBox(Text_roll3);
	}


	if (rollResult == 4) {
		pachici_MessageBox(Text_roll4);
	}


	if (rollResult == 5) {
		pachici_MessageBox(Text_roll5);
	}


	if (rollResult == 6) {
		pachici_MessageBox(Text_roll6);
	}
}




static void CB2_PollPachisi()
{
	RunTasks();
	if(pachisiMessageBoxActive())
	return;

	if (lastRoll > 0) {
		ConvertIntToDecimalStringN(gStringVar1, pos, STR_CONV_MODE_LEFT_ALIGN, 5);
		StringExpandPlaceholders(gStringVar4, text_current_pos);
		pachici_MessageBox(gStringVar4);	
		lastRoll = 0;
	}
	

	if (JOY_NEW(A_BUTTON)) {
		PlaySE(SE_SELECT);
		lastRoll = rollDice();
		printOutcome(lastRoll);
		pos += lastRoll;
				}
	if (JOY_NEW(START_BUTTON))
	{
		PlaySE(SE_M_SELF_DESTRUCT);
		PlayBGM(MUS_LITTLEROOT);
		SetMainCallback2(CB2_ReturnToFieldContinueScript);
	}
}
