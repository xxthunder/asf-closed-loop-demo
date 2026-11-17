#ifndef AUTO_OFF_H
#define AUTO_OFF_H

#include "rte.h"

/** @brief Resets the Auto Off timer. */
void autoOffInit(void);

/** @brief Runnable: switches the light off after the configured time without operation. */
void autoOff(void);

#endif /* AUTO_OFF_H */
