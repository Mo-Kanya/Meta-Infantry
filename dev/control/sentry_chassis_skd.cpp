//
// Created by zhukerui on 2019/4/29.
// Modified by jintengjun on 2019/6/11
// Modified by Laixinyi on 2019/7/1
//

#include "sentry_chassis_skd.h"


/* Parameters */

SentryChassisSKD::SentryChassisThread SentryChassisSKD::sentryChassisThread;

bool SentryChassisSKD::printPosition;
bool SentryChassisSKD::printCurrent;
bool SentryChassisSKD::printVelocity;

bool SentryChassisSKD::enable;
bool SentryChassisSKD::POM;
PIDController SentryChassisSKD::sentry_a2v_pid;
PIDController SentryChassisSKD::sentry_POM_pid;
PIDController SentryChassisSKD::right_v2i_pid;
PIDController SentryChassisSKD::left_v2i_pid;
SentryChassisSKD::sentry_mode_t SentryChassisSKD::running_mode;
float SentryChassisSKD::radius;
float SentryChassisSKD::left_terminal;
float SentryChassisSKD::right_terminal;


/* Functions */

void SentryChassisSKD::init() {
    enable = false;
    POM = false;
    set_pid(2, POM_PID_P2V_PARAMS);
    set_pid(1, CRUISING_PID_A2V_PARAMS);
    set_pid(0, SENTRY_CHASSIS_PID_V2I_PARAMS);
    printPosition = printCurrent = printVelocity = false;
    running_mode = STOP_MODE;
    radius = 50.0f;
}

void SentryChassisSKD::turn_on(){
    enable = true;
}

void SentryChassisSKD::turn_off(){
    enable = false;
}

void SentryChassisSKD::set_pid(int pid_id, PIDControllerBase::pid_params_t new_params){
    if (pid_id == 0){
        left_v2i_pid.change_parameters(new_params);
        right_v2i_pid.change_parameters(new_params);
        left_v2i_pid.clear_i_out();
        right_v2i_pid.clear_i_out();
    }else if (pid_id == 1) {
        sentry_a2v_pid.change_parameters(new_params);
        sentry_a2v_pid.clear_i_out();
    } else if (pid_id == 2){
        sentry_POM_pid.change_parameters(new_params);
        sentry_POM_pid.clear_i_out();
    }
}

void SentryChassisSKD::print_pid(bool print_a2v){
    if (print_a2v){
        PIDControllerBase::pid_params_t to_print = sentry_a2v_pid.get_parameters();
        LOG("%f %f %f %f %f", to_print.kp, to_print.ki, to_print.kd, to_print.i_limit, to_print.out_limit);
    } else{
        PIDControllerBase::pid_params_t to_print = right_v2i_pid.get_parameters();
        LOG("%f %f %f %f %f", to_print.kp, to_print.ki, to_print.kd, to_print.i_limit, to_print.out_limit);
    }
}

/** set all to zero: position and velocity for chassis and motors */
void SentryChassisSKD::set_origin() {
    SentryChassisIF::motor[0].clear_position();
    SentryChassisIF::motor[1].clear_position();
    SentryChassisIF::target_position = 0;
    SentryChassisIF::target_velocity = 0;
    SentryChassisIF::present_position = 0;
    SentryChassisIF::present_velocity = 0;
}

void SentryChassisSKD::set_mode(sentry_mode_t target_mode) {
    running_mode = target_mode;
    set_origin();
    if(running_mode == SHUTTLED_MODE){
        set_destination(radius);
    } else if (running_mode == FINAL_AUTO_MODE){
        SentryChassisIF::present_region = STRAIGHTWAY;
        POM = false; // If the parameters of POM is figured out, set this to be TRUE
        sentry_POM_pid.clear_i_out();
        set_terminals(STRAIGHTWAY_LEFT, STRAIGHTWAY_RIGHT);
    }
}

void SentryChassisSKD::set_destination(float dist) {
    SentryChassisIF::target_position = dist;
}

void SentryChassisSKD::set_maximum_velocity(float new_velocity){
    SentryChassisIF::target_velocity = new_velocity;
    set_pid(1, {SENTRY_CHASSIS_PID_A2V_KP, SENTRY_CHASSIS_PID_A2V_KI, SENTRY_CHASSIS_PID_A2V_KD, SENTRY_CHASSIS_PID_A2V_I_LIMIT, new_velocity});
}

/** set target terminals and set "present region" to the target region */
void SentryChassisSKD::set_terminals(float leftTerminal, float rightTerminal){
    left_terminal = leftTerminal;
    right_terminal = rightTerminal;
    if (left_terminal == CURVE_1_LEFT) SentryChassisIF::present_region = CURVE_1;
    else if (left_terminal == CURVE_2_LEFT) SentryChassisIF::present_region = CURVE_2;
    else SentryChassisIF::present_region = STRAIGHTWAY;
}

/** Power Optimized Mode. */
void SentryChassisSKD::startPOM() {
    POM = true;
    sentry_POM_pid.clear_i_out();
}

/** When enemies are spotted or the sentry is being attacked, start escaping using POM */
void SentryChassisSKD::start_escaping(){
    startPOM();
    if (SentryChassisIF::present_region == STRAIGHTWAY){
        // if we see an enemy on the left, go right
        // if (SuspensionGimbalIF::yaw.angular_position > 0) set_terminals(CURVE_2_LEFT, CURVE_2_RIGHT);
        if (Remote::rc.ch2 < 0) set_terminals(CURVE_2_LEFT, CURVE_2_RIGHT);
        // if we see an enemy on the right, go left
        else set_terminals(CURVE_1_LEFT, CURVE_1_RIGHT);
    }
    else if (SentryChassisIF::present_region == CURVE_1) set_terminals(STRAIGHTWAY_LEFT, STRAIGHTWAY_RIGHT);
    else if (SentryChassisIF::present_region == CURVE_2) set_terminals(STRAIGHTWAY_LEFT, STRAIGHTWAY_RIGHT);
}

/** When finishing the previous escaping process, exit POM, prepare for cruising */
void SentryChassisSKD::stop_escaping(){
    POM = false;
    sentry_a2v_pid.clear_i_out();
}

void SentryChassisSKD::update_target_current() {
    if (enable){

        float sentry_present_position = SentryChassisIF::present_position;

        switch (running_mode) {
            case (ONE_STEP_MODE):
                // If we are in the ONE_STEP_MODE
                if (sentry_present_position >= SentryChassisIF::target_position - 3 &&
                    sentry_present_position <= SentryChassisIF::target_position + 3) {
                    // If the sentry is in the "stop area", we stop the sentry by simply set the target velocity to 0
                    SentryChassisIF::target_velocity = 0;
                } else {
                    SentryChassisIF::target_velocity = sentry_a2v_pid.calc(sentry_present_position,
                                                                           SentryChassisIF::target_position);
                }
                break;
            case (SHUTTLED_MODE):
                // If we are in the SHUTTLED_MODE
                if (sentry_present_position > radius - 3) set_destination(-radius);
                else if (sentry_present_position < -radius + 3) set_destination(radius);
                SentryChassisIF::target_velocity = sentry_a2v_pid.calc(sentry_present_position,
                                                                       SentryChassisIF::target_position);
                break;
            case (V_MODE):
                // this mode is for adjusting velocity pid
                // The target velocity is given by user, here we do no calculation to target velocity
                break;
            case (FINAL_AUTO_MODE):
                // If we are in the FINAL_AUTO_MODE
                if (sentry_present_position > right_terminal - 3) set_destination(left_terminal);
                else if (sentry_present_position < left_terminal + 3) set_destination(right_terminal);
                POM = false; // Delete this line if referee works
                if (POM) {
                    float delta_velocity = sentry_a2v_pid.calc(Referee::power_heat_data.chassis_power, 20);
                    if (SentryChassisIF::target_position > sentry_present_position)
                        // If target position is greater than the present position, then we consider it as a negative direction accelerate
                        SentryChassisIF::target_velocity = SentryChassisIF::present_velocity - delta_velocity;
                    else if (SentryChassisIF::target_position < sentry_present_position)
                        // If target position is greater than the present position, then we consider it as a negative direction accelerate
                        SentryChassisIF::target_velocity = SentryChassisIF::present_velocity + delta_velocity;
                        // If target position is the same as present position, then we can not decide and do nothing
                } else {
                    SentryChassisIF::target_velocity = sentry_a2v_pid.calc(sentry_present_position, SentryChassisIF::target_position);
                }
                break;
            case (STOP_MODE):
            default:
                // If we are in the STOP_MODE, then the sentry now is not movable
                SentryChassisIF::target_velocity = 0;
        }
        // Set the target current
        SentryChassisIF::motor[0].target_current = (int16_t)(right_v2i_pid.calc(SentryChassisIF::motor[0].motor_present_velocity, SentryChassisIF::target_velocity));
        SentryChassisIF::motor[1].target_current = (int16_t)(left_v2i_pid.calc(SentryChassisIF::motor[1].motor_present_velocity, SentryChassisIF::target_velocity));

    }else {
        SentryChassisIF::motor[0].target_current = SentryChassisIF::motor[1].target_current = 0;
    }
}

void SentryChassisSKD::SentryChassisThread::main() {

    setName("SentryChassis");
    SentryChassisSKD::init();

    while (!shouldTerminate()){
        update_target_current();

        if(printPosition)
            print_position();

        if(printCurrent)
            print_current();

        if(printVelocity)
            print_velocity();

        SentryChassisIF::send_currents();
        sleep(TIME_MS2I(100));
    }
}