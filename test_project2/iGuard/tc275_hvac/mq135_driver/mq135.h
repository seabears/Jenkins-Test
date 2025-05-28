//mq135.h
#ifndef MQ135_H
#define MQ135_H

#include"asclin_driver/asclin.h"
#include "Ifx_Types.h"
#include "IfxVadc.h"
#include "IfxVadc_Adc.h"
#include "math.h"

typedef struct
{
    IfxVadc_Adc vadc; /* VADC handle */
    IfxVadc_Adc_Group adcGroup;
} App_VadcAutoScan;


IFX_EXTERN App_VadcAutoScan g_VadcAutoScan;


void Driver_MQ135_Init(void);
void Driver_Adc0_ConvStart(void);
uint16 Driver_Adc0_DataObtain(void);



#endif // MQ135_H
