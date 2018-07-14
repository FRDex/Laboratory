/*
 * BUTTONS.h
 *
 *  Created on: 26/9/2016
 *      Author: tomh6
 */

#ifndef SOURCES_BUTTONS_H_
#define SOURCES_BUTTONS_H_

#include "Template/LDM.h"
#define BUTTON_2 2
#define BUTTON_3 3
#define STATE 0
#define RISING_EDGE 1
#define FALLING_EDGE 2

void BUTTONS_Init();
void BUTTONS_PISR();
bool BUTTONS_Service(unsigned int service, unsigned int button_id);

static void get_button (unsigned int button_id);


#endif /* SOURCES_BUTTONS_H_ */
