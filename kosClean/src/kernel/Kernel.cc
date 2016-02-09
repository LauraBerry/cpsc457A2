/******************************************************************************
DONE            import atoi library
convert to cycles
store in scheduler

machine
    DONE        initbsp2()
    DONE        near end but before kosmain() thread
    DONE        get a cycle CPU::readTSC() and CPU::wait(1000) in miliseconds
        DONE    how many cycles in one second
        DONE    stored in scheduler
    backup hard values also stored in scheduler

rtc.cc
    DONE        clear last four bits 0xF0
    DONE        or it with 0x03(?) : read wiki
    DONE        don't clear entire thing
    DONE        top will tell how to do it
    
    Copyright © 2012-2015 Martin Karsten

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
******************************************************************************/
#include "runtime/Thread.h"
#include "kernel/AddressSpace.h"
#include "kernel/Clock.h"
#include "kernel/Output.h"
#include "world/Access.h"
#include "machine/Machine.h"
#include "devices/Keyboard.h"
#include "main/UserMain.h"
#include "devices/RTC.h"
#include <stdio.h>
#include <stdlib.h>
#include <string>

AddressSpace kernelSpace(true); // AddressSpace.h
volatile mword Clock::tick;     // Clock.h

extern Keyboard keyboard;

string epochLength;
string minGranularity;

const char* epoch;
const char* gran;

#if TESTING_KEYCODE_LOOP
static void keybLoop() 
{
  for (;;) 
  {
    Keyboard::KeyCode c = keyboard.read();
    StdErr.print(' ', FmtHex(c));
  }
}
#endif

void kosMain() {
  KOUT::outl("Welcome to KOS!", kendl);
  auto iter = kernelFS.find("motb");
  if (iter == kernelFS.end()) {
    KOUT::outl("motb information not found");
  } else {
    FileAccess f(iter->second);
    for (;;) {
      char c;
      if (f.read(&c, 1) == 0) break;
      KOUT::out1(c);
    }
    KOUT::outl();
  }
  
  /* A2 */
  
  bool flag = false;
  auto iter2 = kernelFS.find("schedparam");
  if (iter2 == kernelFS.end()) 
  {
    KOUT::outl("schedparam information not found");
  } 
  else 
  {
    FileAccess f(iter2->second);
    for (;;) 
	{
      char c;
      if (f.read(&c, 1) == 0) break;
      
      if(!isspace(c))
      {
          if(flag == false)
          {
                minGranularity = c;
                flag = true;
          }
          else
          {
                epochLength += c;
          }
      }
    }
    
    KOUT::outl();
  } 
  
    epoch = epochLength.c_str();
    gran = minGranularity.c_str();
    
    Machine::epochLength = atoi(epoch);
    Machine::minGranularity = atoi(gran);
  
    KOUT::out1("epochLength: " + epochLength);
    KOUT::outl();
    KOUT::out1("minGranularity: " + minGranularity);
    KOUT::outl();
    KOUT::out1("cycles: ");
    KOUT::out1(Machine::cycles);
    KOUT::outl();

	Scheduler::epochLength=Machine::epochLength;
	Scheduler::minGranularity=Machine::minGranularity;
    
    /* A2 */

#if TESTING_TIMER_TEST
  StdErr.print(" timer test, 3 secs...");
  for (int i = 0; i < 3; i++) 
  {
    Timeout::sleep(Clock::now() + 1000);
    StdErr.print(' ', i+1);
  }
  StdErr.print(" done.", kendl);
#endif
#if TESTING_KEYCODE_LOOP
  Thread* t = Thread::create()->setPriority(topPriority);
  Machine::setAffinity(*t, 0);
  t->start((ptr_t)keybLoop);
#endif
  Thread::create()->start((ptr_t)UserMain);
#if TESTING_PING_LOOP
  for (;;) 
  {
    Timeout::sleep(Clock::now() + 1000);
    // KOUT::outl("...ping...");
  }
#endif
}

extern "C" void kmain(mword magic, mword addr, mword idx)         __section(".boot.text");
extern "C" void kmain(mword magic, mword addr, mword idx) 
{
    
  if (magic == 0 && addr == 0xE85250D6)
 {
    // low-level machine-dependent initialization on AP
    Machine::initAP(idx);
  } 
  else 
  {
    // low-level machine-dependent initialization on BSP -> starts kosMain
    Machine::initBSP(magic, addr, idx);
  }
}
