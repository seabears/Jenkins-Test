// PWM.c
#include "PWM.h"

// FAN용
IfxGtm_Tom_Pwm_Config g_fanConfig;
IfxGtm_Tom_Pwm_Driver g_fanDriver;

// SERVO용
IfxGtm_Tom_Pwm_Config g_servoConfig;
IfxGtm_Tom_Pwm_Driver g_servoDriver;

void setFanDutyCycle(uint16);
static void setSERVODutyCycle(uint16);

void initFanTomPwm(void)
{
    IfxGtm_enable(&MODULE_GTM);                                     /* Enable GTM                                   */
    IfxGtm_Cmu_enableClocks(&MODULE_GTM, IFXGTM_CMU_CLKEN_FXCLK);   /* Enable the FXU clock                         */

    IfxGtm_Tom_Pwm_initConfig(&g_fanConfig, &MODULE_GTM);

    g_fanConfig.tom = FAN.tom;                                      /* Select the TOM depending on the LED          */
    g_fanConfig.tomChannel = FAN.channel;                           /* Select the channel depending on the LED      */
    g_fanConfig.period = FAN_PWM;                                   /* Set the timer period                         */
    g_fanConfig.dutyCycle = 0;
    g_fanConfig.pin.outputPin = &FAN;                               /* Set the LED port pin as output               */
    g_fanConfig.synchronousUpdateEnabled = TRUE;                    /* Enable synchronous update                    */

    IfxGtm_Tom_Pwm_init(&g_fanDriver, &g_fanConfig);                /* Initialize the GTM TOM                       */
    IfxGtm_Tom_Pwm_start(&g_fanDriver, TRUE);                       /* Start the PWM                                */
}


void initServoPwm(void)
{
//    IfxGtm_enable(&MODULE_GTM);                                     /* Enable GTM                                   */
//    IfxGtm_Cmu_enableClocks(&MODULE_GTM, IFXGTM_CMU_CLKEN_FXCLK);   /* Enable the FXU clock*/

    IfxGtm_Tom_Pwm_initConfig(&g_servoConfig, &MODULE_GTM);

    g_servoConfig.tom = SERVO.tom;                                      /* Select the TOM depending on the LED          */
    g_servoConfig.tomChannel = SERVO.channel;                           /* Select the channel depending on the LED      */
    g_servoConfig.clock = IfxGtm_Tom_Ch_ClkSrc_cmuFxclk2;
    g_servoConfig.period = SERVO_PWM;                                /* Set the timer period                         */
    g_servoConfig.dutyCycle = SERVO_PWM_MIN;
    g_servoConfig.pin.outputPin = &SERVO;                               /* Set the LED port pin as output               */
    g_servoConfig.synchronousUpdateEnabled = TRUE;                    /* Enable synchronous update                    */

    IfxGtm_Tom_Pwm_init(&g_servoDriver, &g_servoConfig);                /* Initialize the GTM TOM                       */
    IfxGtm_Tom_Pwm_start(&g_servoDriver, TRUE);                       /* Start the PWM                                */
}

void controlFan(uint8 speed)
{
    uint16 Fanduty = 0;

    switch(speed)
    {
        case STOP:
            Fanduty = 0;
            break;
        case LOW_SPEED:
            Fanduty = FAN_1;
            break;
        case MID_SPEED:
            Fanduty = FAN_2;
            break;
        case HIGH_SPEED:
            Fanduty = FAN_3;
            break;
        default:
            Fanduty = 0;
            break;
    }
    setFanDutyCycle(Fanduty);
}

void changeMode(uint8 mode)
{
    uint16 Servoduty = 0;
    switch(mode)
    {
        case EXT_MODE:
            Servoduty = SERVO_PWM_MIN; //SERVO_PWM_CENTER;
            break;
        case INT_MODE:
            Servoduty = SERVO_PWM_CENTER; //SERVO_PWM_MIN;
            break;
        default:
            Servoduty = 450;
            break;
    }
    setSERVODutyCycle(Servoduty);
}


void setFanDutyCycle(uint16 dutyCycle)
{
    g_fanConfig.dutyCycle = dutyCycle;                              /* Change the value of the duty cycle           */
    IfxGtm_Tom_Pwm_init(&g_fanDriver, &g_fanConfig);                /* Re-initialize the PWM                        */
}

void setSERVODutyCycle(uint16 dutyCycle)
{
    g_servoConfig.dutyCycle = dutyCycle;                              /* Change the value of the duty cycle           */
    IfxGtm_Tom_Pwm_init(&g_servoDriver, &g_servoConfig);                /* Re-initialize the PWM                        */

}