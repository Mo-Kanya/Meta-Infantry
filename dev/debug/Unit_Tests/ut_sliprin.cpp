//
// Created by 钱晨 on 2019/11/2.
//
#include "ch.hpp"
#include "hal.h"

#include "buzzer.h"
#include "led.h"
#include "shell.h"

#include "pid_controller.hpp"
#include "can_interface.h"
#include "chassis_interface.h"
#include "gimbal_interface.h"
#include "remote_interpreter.h"

using namespace chibios_rt;

#if defined(BOARD_RM_2018_A)

#include "vehicle/infantry/vehicle_infantry.h"
#else
#error "supports only RM Board 2018 A currently"
#endif

CANInterface can1(&CAND1);

#define CHASSIS_PID_V2I_KP 26.0f
#define CHASSIS_PID_V2I_KI 0.1f
#define CHASSIS_PID_V2I_KD 0.02f
#define CHASSIS_PID_V2I_I_LIMIT 2000.0f
#define CHASSIS_PID_V2I_OUT_LIMIT 6000.0f
#define CHASSIS_PID_V2I_PARAMS \
    {CHASSIS_PID_V2I_KP, CHASSIS_PID_V2I_KI, CHASSIS_PID_V2I_KD, \
    CHASSIS_PID_V2I_I_LIMIT, CHASSIS_PID_V2I_OUT_LIMIT}

float target_velocity = 0.0f;
float target_angle = 0.0f;
class CurrentSendThread : public chibios_rt::BaseStaticThread<512> {
public:
    bool remote_mode = true;
private:
    void main() final {
        int time = SYSTIME;
        bool status = false;

        PIDController lv2i;
        PIDController rv2i;
        PIDController loadera2v;
        PIDController loaderv2i;

        lv2i.change_parameters(CHASSIS_PID_V2I_PARAMS);
        rv2i.change_parameters(CHASSIS_PID_V2I_PARAMS);
        loadera2v.change_parameters(SHOOT_PID_BULLET_LOADER_A2V_PARAMS);
        loaderv2i.change_parameters(SHOOT_PID_BULLET_LOADER_V2I_PARAMS);

        float loader_target_velocity = 0.0f;
        while (!shouldTerminate()) {
            if (remote_mode){
                target_velocity = Remote::rc.ch0 * 3000;
            }
            ChassisIF::target_current[0] = (int) lv2i.calc(ChassisIF::feedback[ChassisIF::FR].actual_velocity,-target_velocity);
            ChassisIF::target_current[1] = (int) rv2i.calc(ChassisIF::feedback[ChassisIF::FL].actual_velocity,target_velocity);
            if (!remote_mode) {
                loader_target_velocity = loadera2v.calc(GimbalIF::feedback[2].accumulated_angle(),target_angle);
            }
            if(Remote::rc.s2 == Remote::S_UP) {
                loader_target_velocity = loadera2v.calc(GimbalIF::feedback[2].accumulated_angle(),target_angle);
            }
            loader_target_velocity = 10.0f;
            if(Remote::rc.s1 == Remote::S_UP) {
                GimbalIF::target_current[2] = (int) loaderv2i.calc(GimbalIF::feedback[2].actual_velocity,loader_target_velocity);
            }
            ChassisIF::send_chassis_currents();
            GimbalIF::send_gimbal_currents();
            if (SYSTIME-time>1000) {
                if(!status) {
                    LED::led_on(5);
                    time = SYSTIME;
                    status = !status;
                } else {
                    LED::led_off(5);
                    time = SYSTIME;
                    status = !status;
                }
            }
            sleep(TIME_MS2I(1));
        }
    }
}currentSendThread;

class FeedbackThread : public chibios_rt::BaseStaticThread<512> {
    int time = SYSTIME;
    bool status = false;
    void main() final {
        while (!shouldTerminate()){
            Shell::printf("!cv,%u,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f" SHELL_NEWLINE_STR,
                          TIME_I2MS(chibios_rt::System::getTime()),
                          ChassisIF::feedback[ChassisIF::FR].actual_velocity,
                          target_velocity,
                          ChassisIF::feedback[ChassisIF::FR].actual_velocity,
                          -target_velocity,
                          0.0f,
                          0.0f,
                          0.0f,
                          0.0f);
            if (SYSTIME-time>1000) {
                if(!status) {
                    LED::led_on(6);
                    time = SYSTIME;
                    status = !status;
                } else {
                    LED::led_off(6);
                    time = SYSTIME;
                    status = !status;
                }
            }
            sleep(TIME_MS2I(100));
        }
    }
}feedbackThread;

static void cmd_set_remote (BaseSequentialStream *chp, int argc, char *argv[]) {
    (void) argv;
    if (argc != 0) {
        shellUsage(chp, "enable_remote");
        return;
    }
    currentSendThread.remote_mode = true;
}
static void cmd_disable_remote (BaseSequentialStream *chp, int argc, char *argv[]) {
    (void) argv;
    if (argc != 0) {
        shellUsage(chp, "enable_remote");
        return;
    }
    currentSendThread.remote_mode = false;
}
static void cmd_set_velocity (BaseSequentialStream *chp, int argc, char *argv[]) {
    (void) argv;
    if (argc != 1) {
        shellUsage(chp, "set_velocity");
        return;
    }
        target_velocity = Shell::atof(argv[0]);
}
static void cmd_clear (BaseSequentialStream *chp, int argc, char *argv[]) {
    (void) argv;
    if (argc != 0) {
        shellUsage(chp, "clear");
        return;
    }
    currentSendThread.remote_mode = false;
    target_velocity = 0.0f;
}
static void cmd_shoot(BaseSequentialStream *chp, int argc, char *argv[]) {
    (void) argv;
    if (argc!= 0) {
        shellUsage(chp, "shoot");
        return;
    }
    target_angle += (360.0f/8.0f);
}
ShellCommand ControllerCommands[] = {
        {"enable_remote"  ,   cmd_set_remote},
        {"disable_remote" ,   cmd_disable_remote},
        {"set_velocity"   ,   cmd_set_velocity},
        {"clear"          ,   cmd_clear},
        {"shoot"          ,   cmd_shoot},
        {nullptr,         nullptr}
};

int main() {

    halInit();
    System::init();
    Shell::start(HIGHPRIO);
    Shell::addCommands(ControllerCommands);
    LED::all_off();
    can1.start(HIGHPRIO - 1);
    Remote::start();
    chThdSleepMilliseconds(5);
    chThdSleepMilliseconds(10);
    feedbackThread.start(NORMALPRIO+1);
    chThdSleepMilliseconds(10);
    ChassisIF::init(&can1);
    GimbalIF::init(&can1,233,233,GimbalIF::GM6020,GimbalIF::RM6623,GimbalIF::M2006);
    time_msecs_t t1 = SYSTIME;

    Buzzer::play_sound(Buzzer::sound_kong_fu_FC,THREAD_BUZZER_PRIO);

    /**-----check can error------*/
    while (WITHIN_RECENT_TIME(t1, 100)) {
        if (WITHIN_RECENT_TIME(can1.last_error_time, 5)) {  // can error occurs
            t1 = SYSTIME;  // reset the counter
        }
    }
    LED::led_on(1);

    /**---check feedback error---*/
    time_msecs_t t = SYSTIME;
    while (WITHIN_RECENT_TIME(t, 20)) {
        if (not WITHIN_RECENT_TIME(ChassisIF::feedback[ChassisIF::FR].last_update_time, 5)) {
            // No feedback in last 5 ms (normal 1 ms)
            LOG_ERR("Startup - Chassis FR offline.");
            t = SYSTIME;  // reset the counter
        }
    }
    LED::led_on(2);
    currentSendThread.start(NORMALPRIO + 2);
#if CH_CFG_NO_IDLE_THREAD  // See chconf.h for what this #define means.
    // ChibiOS idle thread has been disabled, main() should implement infinite loop
    while (true) {}
#else
    // When vehicle() quits, the vehicle thread will somehow enter an infinite loop, so we set the
    // priority to lowest before quitting, to let other threads run normally
    chibios_rt::BaseThread::setPriority(IDLEPRIO);
#endif
    return 0;
}