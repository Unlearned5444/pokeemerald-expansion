// Pachisi Minigame

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
#include "pachisi_data/pachisi_levels.h"



//=============================================================================
// Global Variables
//=============================================================================
EWRAM_DATA u16 pos;
EWRAM_DATA u8 lastRoll;
static EWRAM_DATA const u8* gPachisiCurrentText = NULL;

//=============================================================================
// Text Constants for Pachisi Messages
//=============================================================================
const u8 Text_roll1[] = _("You rolled a 1.");
const u8 Text_roll2[] = _("You rolled a 2.");
const u8 Text_roll3[] = _("You rolled a 3.");
const u8 Text_roll4[] = _("You rolled a 4.");
const u8 Text_roll5[] = _("You rolled a 5.");
const u8 Text_roll6[] = _("You rolled a 6.");
const u8 text_current_pos[] = _("You are now on tile {STR_VAR_1}.");
const u8 text_end[] = _("You have reached the end of the board.");


//=============================================================================
// Forward Declarations
//=============================================================================

// Task to handle the Pachisi message box state machine.
void task_PachisiMessageBox(u8 taskId);

// Forward declaration for square-specific functions.
static void launchBattle(void);
static void giveMoney(void);
static void triggerDeath(void);

//=============================================================================
// Message Box Functions
//=============================================================================

// Creates and initializes a task to display a Pachisi message.
void pachisi_MessageBox(const u8* str)
{
    u8 taskId = CreateTask(task_PachisiMessageBox, 8);  // Priority set to 8
    gTasks[taskId].data[0] = 0;
    gPachisiCurrentText = str;
}

// Task function to process the Pachisi message box.
void task_PachisiMessageBox(u8 taskId)
{
    s16* state = &gTasks[taskId].data[0];

    if (*state == 0)
    {
        // Initialize message box and start auto-scroll.
        // Uncomment InitFieldMessageBox() if initialization is needed.
        // InitFieldMessageBox();
        // ShowFieldMessage(gPachisiCurrentText);
        gTextFlags.autoScroll = TRUE;
        ShowFieldAutoScrollMessage(gPachisiCurrentText);
        (*state)++;
    }
    else if (*state == 1)
    {
        // Proceed when the message box has finished.
        if (GetFieldMessageBoxMode() == FIELD_MESSAGE_BOX_HIDDEN)
        {
            gPachisiCurrentText = NULL; // Mark as inactive.
            DestroyTask(taskId);
        }
    }
}

// Returns true if a Pachisi message box is currently active.
u8 pachisiMessageBoxActive()
{
    return (gPachisiCurrentText != 0);
}

//=============================================================================
// Minigame Callbacks and Utility Functions
//=============================================================================

// Forward declarations for static functions.
static void CB2_TransitionToPachisi();   // Sets up the minigame transition.
static void initPachisiVars();           // Initializes minigame variables.
static void CB2_PollPachisi();            // Main loop for processing game tasks.
static u8 rollDice();                    // Simulates a dice roll (1-6).
static void printOutcome(u8 rollResult); // Displays message based on dice roll.

// Entry point for starting the Pachisi minigame.
void playPachisiMinigame(void)
{
    SetMainCallback2(CB2_TransitionToPachisi);
}

// Transition callback: prepares the minigame.
static void CB2_TransitionToPachisi()
{
    // Initialize variables and transition to the main polling loop.
    initPachisiVars();
    SetMainCallback2(CB2_PollPachisi);
}

// Initializes Pachisi game variables and starts its background music.
static void initPachisiVars()
{
    pos = 0;
    lastRoll = 0;
    PlayBGM(MUS_PACHISI);
/* Examples of how to access pachisi information. This is showing you how it is done, this is not exactly how you should do it. I'm only trying to show you the interface. */

/* Get level: */
const PachisiSquare* level=pachisi_levels[PACHISI_LEVEL_THE_FIRST_LOSER].level;
/* And size. */
size_t levelSize=pachisi_levels[PACHISI_LEVEL_THE_FIRST_LOSER].size;
/* Get the type of the last square. */
int square=level[levelSize-1].square;
/* Get the first data field of the third. */
int levelData0=level[2].data[0];

/* Let's confirm that the levelSize is 50. It should be as of this writing, if the generator is doing its job correctly. */
u8 valid=1;
if(levelSize!=50)
valid=0;
/* Let's confirm that the last square is a death square. It should be as of this writing, if the generator is doing its job correctly and our code is correctly accessing the data. */
if(square!=SQUARE_DEATH)
valid=0;
if(valid)
PlaySE(SE_EGG_HATCH);
else
PlaySE(SE_FAILURE);

/*
Protip / coding exercise:
Write getters around the pachisi level data so you're never touching the structures directly.
E.G. the above code could look like:
int square=getPachisiSquare(levelId, levelSize-1);
s16 square_data_0=getPachisiSquareData(levelId, 2, 0);
where levelId = PACHISI_LEVEL_THE_FIRST_LOSER


*/


}

// Simulates a dice roll by returning a random number between 1 and 6.
static u8 rollDice()
{
    u8 randRoll = Random() % 6;
    return randRoll + 1;
}

// Displays the outcome message for the given dice roll.
static void printOutcome(u8 rollResult)
{
    if (rollResult == 1)  pachisi_MessageBox(Text_roll1);
    else if (rollResult == 2)  pachisi_MessageBox(Text_roll2);
    else if (rollResult == 3)  pachisi_MessageBox(Text_roll3);
    else if (rollResult == 4)  pachisi_MessageBox(Text_roll4);
    else if (rollResult == 5)  pachisi_MessageBox(Text_roll5);
    else if (rollResult == 6)  pachisi_MessageBox(Text_roll6);
}


// Stub functions for square-specific tasks.
static void launchBattle(void)
{
    // Placeholder for battle.
}

static void giveMoney(void)
{
    // Placeholder for giving money.
}

static void triggerDeath(void)
{
    PlaySE(SE_THUNDER);
    PlaySE(SE_THUNDER2);
    PlayBGM(MUS_LITTLEROOT);
    SetMainCallback2(CB2_ReturnToFieldContinueScript);

}

// Main loop callback for processing Pachisi minigame tasks and inputs.
static void CB2_PollPachisi()
{
    RunTasks();

    // If a message is still showing, halt input processing.
    if (pachisiMessageBoxActive())
        return;

    // Display the current position if a roll was made.
    if (lastRoll > 0)
    {
/* Todo: update this with the modern code. */

//        if (pos >= SQUARE_COUNT)
//        {
//            pachisi_MessageBox(text_end);
//            SetMainCallback2(CB2_ReturnToFieldContinueScript);
//        }
//        else
        {
            ConvertIntToDecimalStringN(gStringVar1, pos, STR_CONV_MODE_LEFT_ALIGN, 5);
            StringExpandPlaceholders(gStringVar4, text_current_pos);
            pachisi_MessageBox(gStringVar4);
            lastRoll = 0;
        }
    }

    // Process input: Roll dice on A_BUTTON.
    if (JOY_NEW(A_BUTTON))
    {
        PlaySE(SE_SELECT);
        lastRoll = rollDice();
        printOutcome(lastRoll);
        pos += lastRoll;
    }
    // Process input: Exit minigame on START_BUTTON.
    else if (JOY_NEW(START_BUTTON))
    {
        PlaySE(SE_M_SELF_DESTRUCT);
        PlayBGM(MUS_LITTLEROOT);
        SetMainCallback2(CB2_ReturnToFieldContinueScript);
    }
}
