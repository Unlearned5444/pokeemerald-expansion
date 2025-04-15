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

//=============================================================================
// Global Variables
//=============================================================================
EWRAM_DATA u16 pos;
EWRAM_DATA u8 lastRoll;
const EWRAM_DATA struct PachisiSquare* currentBoard = NULL;
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

// Function to check the square type and execute corresponding tasks.
static void checkSquare()
{
    if (currentBoard == NULL)
        return;

    const struct PachisiSquare* square = &currentBoard[pos];
    switch (square->square)
    {
        case SQUARE_WILD_LAND:
            launchBattle();
            break;
        case SQUARE_WILD_CAVE:
            launchBattle();
            break;
        case SQUARE_WILD_WATER:
            launchBattle();
            break;
        case SQUARE_MONEY:
            giveMoney();
            break;
        case SQUARE_DEATH:
            triggerDeath();
            break;
        default:
            break;
    }
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
        ConvertIntToDecimalStringN(gStringVar1, pos, STR_CONV_MODE_LEFT_ALIGN, 5);
        StringExpandPlaceholders(gStringVar4, text_current_pos);
        pachisi_MessageBox(gStringVar4);
        checkSquare();
        lastRoll = 0;
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
