#ifndef TIMER_H
#define TIMER_H

#include <chrono>
#include "definitions.h"
#include <iostream>
#include <string>


#ifdef _WIN32
#include "windows.h"
#else
#include "sys/time.h"
#include "sys/select.h"
#include "unistd.h"
#include "string.h"
#endif


UINT_64 millis();

int InputWaiting();
void ReadInput(SEARCH_INFORMATION*) ;
#endif // TIMER_H
