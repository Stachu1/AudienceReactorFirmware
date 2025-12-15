#include "display_task.h"

TimerDisplayTask::TimerDisplayTask()
    : display(nullptr), start_Time(0), duration(0), lastUpdate(0) {}

void TimerDisplayTask::begin(TM1637Display* display) {
    this->display = display;
    display->setBrightness(7);

}
void TimerDisplayTask::show(unsigned long duration){
        Timer=duration*1000;
        //unsigned long int seconds = durationMs /1000;
        unsigned long int totalSeconds = duration;
        unsigned long int seconds = totalSeconds %60;
        unsigned long int minutes = totalSeconds/60;
        uint16_t value = minutes * 100 + seconds;
        display->showNumberDecEx(value, 0b01000000, true);
    }

void TimerDisplayTask::start() {
    start_Time = millis();
    Serial.println(start_Time);
    last_current_time=0;

}
void TimerDisplayTask::stop(){
    Timer=0;
    start_Time=0;
    uint16_t value = 0;
    display->showNumberDecEx(value, 0b01000000, true);
}

void TimerDisplayTask::update() {
    if(start_Time==0) return;
    unsigned long int elapsed =millis()-start_Time;
    unsigned long int current_time = (elapsed>=Timer) ? 0: (Timer - elapsed);
    Serial.println(elapsed);
        if(elapsed - last_current_time >= 1000){
            Serial.println("update timer");
            Serial.println(elapsed);
            unsigned long int totalSeconds = current_time/1000; 
            unsigned long int seconds = totalSeconds %60;
            unsigned long int minutes = totalSeconds/60;
            last_current_time=elapsed;
            uint16_t value = minutes * 100 + seconds;
            display->showNumberDecEx(value, 0b01000000, true);
        }
    if(elapsed==Timer){ //when timer is up reset timer display to 0
        stop();
    }
}
