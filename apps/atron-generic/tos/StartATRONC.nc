/*
 * Copyright (c) 2007-2008 University of Southern Denmark
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * - Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the
 *   distribution.
 * - Neither the name of the the copyright holders nor the names of
 *   their contributors may be used to endorse or promote products derived
 *   from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT
 * HOLDERS OR THEIR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * Start ATRON Controller using the ASE library
 *
 * @author Mirko Bordignon
 * @author David Johan Christensen
 */
#include "controller.h"
#include "printf.h"
#include <ase/infrastructure/Scheduler/Scheduler.h>
module StartATRONC
{
  uses {
    interface Boot;
    interface Leds;
    interface Timer<TMilli>;
    interface Read<module_state_t>;

  }
}
implementation
{
  task void ASEControl()  {
    ase_act();
    printfflush();
    //post ASEControl();
    call Timer.startOneShot(10);
  }
  bool init;
  event void Boot.booted() {
    init = TRUE;
    call Leds.set(255);
    //call Read.read();
    call Timer.startOneShot(100);
  }

  void startControl() {
    if(init) {
      call Leds.set(0);
      atronApi_setup();
      ase_init(controller_init);
      init = FALSE;
    }
    post ASEControl();
  }

  event void Read.readDone(error_t err, module_state_t state) {
    setStartState(state);
    startControl();
  }

  event void Timer.fired() {
    startControl();
  }
}


