//
// Created by zhukerui on 2019/4/29.
//

#ifndef META_INFANTRY_SENTRY_CHASSIS_CALCULATOR_H
#define META_INFANTRY_SENTRY_CHASSIS_CALCULATOR_H
#define SYSTIME (TIME_I2MS(chVTGetSystemTime()))

#include "sentry_chassis_interface.h"
#include "pid_controller.h"
#include "can_interface.h"
#include "ch.hpp"
#include "hal.h"

class SentryChassisController: public SentryChassis{
public:

    enum sentry_mode_t{
        STOP_MODE,
        ONE_STEP_MODE,
        AUTO_MODE
    };

    static bool enable;

    static float constexpr radius = 30.0f; // the range that sentry can move around the origin in the AUTO MODE

    /**
     * @brief initialize the calculator class
     * @param can_interface
     */
    static void init_controller(CANInterface* can_interface);

    /**
     * @brief change the running mode if needed
     * @param target_mode
     */
    static void set_mode(sentry_mode_t target_mode){
        running_mode = target_mode;
        clear_position();
        if(running_mode == AUTO_MODE){
            set_destination(radius);
        }
    }

    /**
     * @brief set the present position and target position to be the 0 point
     */
    static void clear_position();

    static void set_target_current();

    /**
     * @brief set the target position and the target velocity according to the given position
     * @param dist the given position, positive for right, negative for left
     */
    static void set_destination(float dist);

    /**
 * @brief use the present data and PIDController to calculate and set the target current that will be sent
 */
    static void update_target_current();

    /**
     * @brief set the speed mode
     * @param mode true for varying_speed mode, false for const_speed mode
     */
    static void change_speed_mode(bool mode){
        change_speed = mode;
        if(change_speed) present_time = SYSTIME;
    }

    /**
     * @brief change the parameters for the v_to_i PIDController
     */
    static void change_v_to_i_pid(float _kp, float _ki, float _kd, float _i_limit, float _out_limit){
        motor_right_pid.change_parameters(_kp, _ki, _kd, _i_limit, _out_limit);
        motor_left_pid.change_parameters(_kp, _ki, _kd, _i_limit, _out_limit);
    }

    /**
     * @brief Debug helper function. Print the PIDController parameters
     * @param motor_id
     */
    static void print_pid_params(){
        LOG("motor_right:" SHELL_NEWLINE_STR);
        LOG("kp = %.2f ki = %.2f kd = %.2f i_limit = %.2f out_limit = %.2f" SHELL_NEWLINE_STR,
            motor_right_pid.kp, motor_right_pid.ki, motor_right_pid.kd, motor_right_pid.i_limit, motor_right_pid.out_limit);
        LOG("motor_left:" SHELL_NEWLINE_STR);
        LOG("kp = %.2f ki = %.2f kd = %.2f i_limit = %.2f out_limit = %.2f" SHELL_NEWLINE_STR,
            motor_left_pid.kp, motor_left_pid.ki, motor_left_pid.kd, motor_left_pid.i_limit, motor_left_pid.out_limit);
    }

    /**
     * @brief Debug helper function. Print the present position in cm
     */
    static void print_position(){
        LOG("motor %d position: %.2f", 0, motor[0].present_position);
        LOG("motor %d position: %.2f", 1, motor[1].present_position);
    }

    /**
     * @brief Debug helper function. Print the target current sent to the motors
     */
    static void print_current(){
        LOG("motor %d target_current: %d", 0, motor[0].target_current);
        LOG("motor %d target_current: %d", 1, motor[1].target_current);
    }

    /**
     * @brief Debug helper function. Print the present velocity in cm/s
     */
    static void print_velocity(){
        LOG("motor %d present_velocity: %.2f", 0, motor[0].present_velocity);
        LOG("motor %d present_velocity: %.2f", 1, motor[1].present_velocity);
    }

private:

    static sentry_mode_t running_mode;

    static bool change_speed;

    static time_msecs_t present_time;

    static float target_position;

    static float target_velocity;

    static PIDController motor_right_pid;
    static PIDController motor_left_pid;

    /**
     * @brief the const values
     */

    static float constexpr maximum_speed = 80.0f;

};


#endif //META_INFANTRY_SENTRY_CHASSIS_CALCULATOR_H
