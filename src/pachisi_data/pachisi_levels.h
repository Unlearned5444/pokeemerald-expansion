#ifndef PACHISI_LEVELS_H
#define PACHISI_LEVELS_H

#include <stddef.h>          // for size_t
#include "pachisi_defines.h"
#include "the_first_loser.h"

struct PachisiLevelInfo {
    const PachisiSquare *level;
    size_t size;
};

static const struct PachisiLevelInfo pachisi_levels[] = {
    [PACHISI_LEVEL_THE_FIRST_LOSER] = { .level = PachisiLevel_THE_FIRST_LOSER, .size = sizeof(PachisiLevel_THE_FIRST_LOSER)/sizeof(*PachisiLevel_THE_FIRST_LOSER) },
};

#endif // PACHISI_LEVELS_H
