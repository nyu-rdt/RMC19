#include "ros/ros.h"
#include "scheduler/GetPhase.h"
#include <stdint.h>

class Scheduler {
public:
    enum PHASE {MANUAL=0,ESTOP=1,WAIT=2,APPROACH=3,DECOUPLE=4,DIG=5,ALIGN=6,TRANSFER=7,RETREAT=8,DEP_ALIGN=9,DEPOSIT=10}
    Scheduler() :cur_phase(PHASE::MANUAL) {}
    void set_phase_timings(Scheduler::PHASE phase, int millis);
    int get_match_time();
    void start_match();
    void pause();
    void end_match();
    void next_phase();
    void set_phase(Scheduler::PHASE); 
    uint8_t get_phase();
private:
    Scheduler::PHASE cur_phase;

}
