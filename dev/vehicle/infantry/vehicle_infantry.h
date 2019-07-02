//
// Created by liuzikai on 2019-04-22.
// This file contains common parameters for infantry
//

#ifndef META_INFANTRY_VEHICLE_INFANTRY_H
#define META_INFANTRY_VEHICLE_INFANTRY_H

/// AHRS Configurations
static constexpr Matrix33 ON_BOARD_AHRS_MATRIX = {{0.0f, 0.0f, 1.0f},
                                                  {1.0f, 0.0f, 0.0f},
                                                  {0.0f, 1.0f, 0.0f}};

#define MPU6500_STORED_GYRO_BIAS {0.0f, 0.0f, 0.0f}


/// Gimbal and Shoot Installation Configurations
#define GIMBAL_YAW_INSTALL_DIRECTION    (GimbalSKD::POSITIVE)
#define GIMBAL_PITCH_INSTALL_DIRECTION  (GimbalSKD::POSITIVE)
#define SHOOT_BULLET_INSTALL_DIRECTION (ShootSKD::POSITIVE)
#define SHOOT_DEGREE_PER_BULLER 40.0f  // rotation degree of bullet loader for each bullet

static constexpr Matrix33 GIMBAL_AHRS_INSTALL_MATRIX = {{1.0f, 0.0f, 0.0f},
                                                        {0.0f, 1.0f, 0.0f},
                                                        {0.0f, 0.0f, 1.0f}};


/// Gimbal and Shoot PID Parameters
#define GIMBAL_PID_YAW_A2V_KP 5.3f
#define GIMBAL_PID_YAW_A2V_KI 0.0f
#define GIMBAL_PID_YAW_A2V_KD 0.18f
#define GIMBAL_PID_YAW_A2V_I_LIMIT 1000.0f
#define GIMBAL_PID_YAW_A2V_OUT_LIMIT 3000.0f
#define GIMBAL_PID_YAW_A2V_PARAMS \
    {GIMBAL_PID_YAW_A2V_KP, GIMBAL_PID_YAW_A2V_KI, GIMBAL_PID_YAW_A2V_KD, \
    GIMBAL_PID_YAW_A2V_I_LIMIT, GIMBAL_PID_YAW_A2V_OUT_LIMIT}

#define GIMBAL_PID_YAW_V2I_KP 18.0f
#define GIMBAL_PID_YAW_V2I_KI 0.5f
#define GIMBAL_PID_YAW_V2I_KD 0.0f
#define GIMBAL_PID_YAW_V2I_I_LIMIT 1000.0f
#define GIMBAL_PID_YAW_V2I_OUT_LIMIT 3000.0f
#define GIMBAL_PID_YAW_V2I_PARAMS \
    {GIMBAL_PID_YAW_V2I_KP, GIMBAL_PID_YAW_V2I_KI, GIMBAL_PID_YAW_V2I_KD, \
    GIMBAL_PID_YAW_V2I_I_LIMIT, GIMBAL_PID_YAW_V2I_OUT_LIMIT}

#define GIMBAL_PID_PITCH_A2V_KP 10.0f
#define GIMBAL_PID_PITCH_A2V_KI 0.0f
#define GIMBAL_PID_PITCH_A2V_KD 0.75f
#define GIMBAL_PID_PITCH_A2V_I_LIMIT 1000.0f
#define GIMBAL_PID_PITCH_A2V_OUT_LIMIT 3000.0f
#define GIMBAL_PID_PITCH_A2V_PARAMS \
    {GIMBAL_PID_PITCH_A2V_KP, GIMBAL_PID_PITCH_A2V_KI, GIMBAL_PID_PITCH_A2V_KD, \
    GIMBAL_PID_PITCH_A2V_I_LIMIT, GIMBAL_PID_PITCH_A2V_OUT_LIMIT}

#define GIMBAL_PID_PITCH_V2I_KP 19.0f
#define GIMBAL_PID_PITCH_V2I_KI 0.35f
#define GIMBAL_PID_PITCH_V2I_KD 0.0f
#define GIMBAL_PID_PITCH_V2I_I_LIMIT 1000.0f
#define GIMBAL_PID_PITCH_V2I_OUT_LIMIT 3000.0f
#define GIMBAL_PID_PITCH_V2I_PARAMS \
    {GIMBAL_PID_PITCH_V2I_KP, GIMBAL_PID_PITCH_V2I_KI, GIMBAL_PID_PITCH_V2I_KD, \
    GIMBAL_PID_PITCH_V2I_I_LIMIT, GIMBAL_PID_PITCH_V2I_OUT_LIMIT}

// TODO: select a better params
#define SHOOT_PID_BULLET_LOADER_A2V_KP 0.0f
#define SHOOT_PID_BULLET_LOADER_A2V_KI 0.0f
#define SHOOT_PID_BULLET_LOADER_A2V_KD 0.0f
#define SHOOT_PID_BULLET_LOADER_A2V_I_LIMIT 0.0f
#define SHOOT_PID_BULLET_LOADER_A2V_OUT_LIMIT 360.0f
#define SHOOT_PID_BULLET_LOADER_A2V_PARAMS \
    {SHOOT_PID_BULLET_LOADER_A2V_KP, SHOOT_PID_BULLET_LOADER_A2V_KI, SHOOT_PID_BULLET_LOADER_A2V_KD, \
    SHOOT_PID_BULLET_LOADER_A2V_I_LIMIT, SHOOT_PID_BULLET_LOADER_A2V_OUT_LIMIT}

// TODO: select a better params
#define SHOOT_PID_BULLET_LOADER_V2I_KP 20.0f
#define SHOOT_PID_BULLET_LOADER_V2I_KI 0.0f
#define SHOOT_PID_BULLET_LOADER_V2I_KD 0.0f
#define SHOOT_PID_BULLET_LOADER_V2I_I_LIMIT 0.0f
#define SHOOT_PID_BULLET_LOADER_V2I_OUT_LIMIT 3000.0f
#define SHOOT_PID_BULLET_LOADER_V2I_PARAMS \
    {SHOOT_PID_BULLET_LOADER_V2I_KP, SHOOT_PID_BULLET_LOADER_V2I_KI, SHOOT_PID_BULLET_LOADER_V2I_KD, \
    SHOOT_PID_BULLET_LOADER_V2I_I_LIMIT, SHOOT_PID_BULLET_LOADER_V2I_OUT_LIMIT}


/// Chassis Mechanism Parameters
#define CHASSIS_WHEEL_BASE  420.0f                     // distance between front axle and the back axle [mm]
#define CHASSIS_WHEEL_TREAD 372.0f                     // distance between left and right wheels [mm]
#define CHASSIS_WHEEL_CIRCUMFERENCE 478.0f             // circumference of wheels [mm]


/// Chassis PID Parameters
#define CHASSIS_PID_V2I_KP 26.0f
#define CHASSIS_PID_V2I_KI 0.1f
#define CHASSIS_PID_V2I_KD 0.02f
#define CHASSIS_PID_V2I_I_LIMIT 2000.0f
#define CHASSIS_PID_V2I_OUT_LIMIT 6000.0f
#define CHASSIS_PID_V2I_PARAMS \
    {CHASSIS_PID_V2I_KP, CHASSIS_PID_V2I_KI, CHASSIS_PID_V2I_KD, \
    CHASSIS_PID_V2I_I_LIMIT, CHASSIS_PID_V2I_OUT_LIMIT}

#define CHASSIS_PID_THETA2V_KP 1.0f
#define CHASSIS_PID_THETA2V_KI 0.0f
#define CHASSIS_PID_THETA2V_KD 0.0f
#define CHASSIS_PID_THETA2V_I_LIMIT 0.0f
#define CHASSIS_PID_THETA2V_OUT_LIMIT 150.0f
#define CHASSIS_PID_THETA2V_PARAMS \
    {CHASSIS_PID_THETA2V_KP, CHASSIS_PID_THETA2V_KI, CHASSIS_PID_THETA2V_KD, \
    CHASSIS_PID_THETA2V_I_LIMIT, CHASSIS_PID_THETA2V_OUT_LIMIT}

/// Thread Priority List
#define THREAD_CAN1_PRIO                    (HIGHPRIO - 1)
#define THREAD_MPU_PRIO                     (HIGHPRIO - 2)
#define THREAD_IST_PRIO                     (HIGHPRIO - 3)
#define THREAD_AHRS_PRIO                    (HIGHPRIO - 4)
#define THREAD_GIMBAL_SKD_PRIO              (NORMALPRIO + 3)
#define THREAD_CHASSIS_SKD_PRIO             (NORMALPRIO + 2)
#define THREAD_USER_PRIO                    (NORMALPRIO)
#define THREAD_SHOOT_SKD_PRIO               (NORMALPRIO + 1)
#define THREAD_CHASSIS_LG_DODGE_PRIO        (NORMALPRIO - 1)
#define THREAD_SHOOT_LG_STUCK_DETECT_PRIO   (NORMALPRIO - 2)
#define THREAD_INSPECTOR_PRIO               (NORMALPRIO - 10)
#define THREAD_SHELL_PRIO                   (LOWPRIO + 10)
#define THREAD_BUZZER_PRIO                  (LOWPRIO + 1)
#define THREAD_IDEAL_PRIO                   (LOWPRIO)

#endif //META_INFANTRY_VEHICLE_INFANTRY_H
