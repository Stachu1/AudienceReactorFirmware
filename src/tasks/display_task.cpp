#include "display_task.h"

TimerDisplayTask::TimerDisplayTask()
    : display(nullptr), start_Time(0), duration(0), lastUpdate(0) {}

void TimerDisplayTask::begin(TM1637Display* display) {
    this->display = display;
    display->setBrightness(7);

}
void TimerDisplayTask::show(unsigned long durationMs){
        unsigned long int seconds = durationMs /1000;
        unsigned long int minutes = seconds/60;
        TimeDisp[2] = seconds / 10; //counting seconds
        TimeDisp[3] = seconds % 10;
        TimeDisp[0] = minutes / 10; //counting minutes
        TimeDisp[1] = minutes % 10;
        display->setSegments((uint8_t*)TimeDisp, 4);
    }

void TimerDisplayTask::start(unsigned long durationMs) {
    duration = durationMs;
    start_Time = millis();
    lastUpdate = 0;
}
void TimerDisplayTask::stop(){
    Timer=0;
    TimeDisp[2] = 0; //counting seconds 
    TimeDisp[3] = 0;
    TimeDisp[0] = 0; //counting minutes
    TimeDisp[1] = 0;
    display->setSegments((uint8_t*)TimeDisp, 4);

}

void TimerDisplayTask::update() {
    if (!display || duration == 0) return;
    
        unsigned long int elapsed =millis()-start_Time;
        unsigned long int current_time = Timer - elapsed;
        if(current_time < 0) current_time = 0;

        if(current_time - last_current_time >= 1000){
            unsigned long int seconds = current_time /1000;
            unsigned long int minutes = seconds/60;
            //seconds = seconds %60;
            TimeDisp[2] = seconds / 10; //counting seconds
            TimeDisp[3] = seconds % 10;
            TimeDisp[0] = minutes / 10; //counting minutes
            TimeDisp[1] = minutes % 10;
            display->setSegments((uint8_t*)TimeDisp, 4);
        }
        if(elapsed>=Timer){ //when timer is up reset timer display to 0
            Timer=0;
            TimeDisp[2] = 0; //counting seconds
            TimeDisp[3] = 0;
            TimeDisp[0] = 0; //counting minutes
            TimeDisp[1] = 0;
            display->setSegments((uint8_t*)TimeDisp, 4);

        }
}
