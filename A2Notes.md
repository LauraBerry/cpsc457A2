#To-Do List

##Part 1: Create system file to define schedular parameters
- [x] create file "schedparam" in src/user/exec
- [x] "schedparam" should be read and parsed by KOS BEFORE scheduler objects created
- [x] print scheduler parameter names and values after parsed
- Example of reading system file: src/kernel/Kernel.cc
- KOUT:out1 for printing
- scheduler objects initialization in src/machine.cc (BSP initilizaton)
- default AFS parameters
  - epoch length: 20ms
  - minGranularity: 4ms

##Part 2: Recalibrate scheduler parameters
- [x] set RTC to the high possible frequency
- [ ] print frequency on screen while KOS is booting
- [ ] recalibrate AFS scheduler parmeters in terms of TSC time ticks
- [ ] find average # of tics between two consecutive timer interrupts
- [ ] based on frequency of timer interrupt and average, calculate ticks of the interval
- [ ] store the ticks as static variables in scheduler class
- [ ] print recalibrated values during bot up
- RTC time drives periodic interrupts
  - Rate of 1024 Hz
- Check files
  - devices/RTC.h
  - devices/RTC.cc
  - machine/Machine.cc
- timer interrupts in 1 second
  - depends on frequency of RTC timerr
  - timer interrupts received by core 0
    - core 0 passes interrupt to one of the other cores
      - void Machine:rrPreemptIPI (mword ticks) in src/machine/Machine.cc
    - exception handler whenever PreemptIPI is received
      - extern "C" void irq_handler_0xed(mword* isrFrame)
      
##Part 3: Integrate balanced tree implementation
- [ ] add kernel code
- [ ] test it during boot time (place test cases anywhere)

##Part 4: Implement scheduler
- [ ] implement using the tree

#Other Information

##AFS Specification
- Epochs of length T
- Beginning of epoch: N processes waiting
- End of epoch: each process run around T/N

##AFS Design

###Time measurement and task running time
- vRuntime: amount of virtual time for which task has run since creation
- for every timer interrupt: scheduler updates vRuntime
  - vRuntime = vRuntime + (t/p)
  
###Scheduler parameters
- schedMinGranularity: min time a task must run before it can be preempted
- defaultEpochLength: default length of an epoch

###Schedular Data Structure
- readyTree: binary search tree
  - tasks ordered based on vRuntime
  - least vRuntime: leftmost node of the tree
- minvRuntime: vRuntime of task that has run for the least amount of virutal time

###Scheduler Behaviour
- default scheduler parameter values: converted from milliseconds to TSC ticks
  - schedMinGranularityTicks
  - defaultEpochLengthTicks
  - TSC: increments by 1 for every CPU cycle
  - detect frequency of CPU from RTC time
  - http://wiki.osdev.org/Detecting_CPU_Speed
- task to be dispatched
  - taskTimeSlice = (EpochLengthTicks * priority of taks) / (total priority values of all tasks in readyTree)
- task added to readyTree
  - see Assignment2 pdf
- task created
  - vRuntime = minvRuntime
- task suspended
  - vRuntime = vRuntime - minvRuntime
- task unblocked before inserting into readyTree
  - vRuntime = vRuntime + minvRuntime
- AFS_timer_inteerrupt algorith in Assignment2 pdf
