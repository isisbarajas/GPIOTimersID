#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MKL25Z4.h"
#include "fsl_debug_console.h"
#include "fsl_tpm.h"

#define Rojo 18u
#define Azul 1u
#define Verde 19u

typedef struct{
	uint8_t ledRojo;
	uint8_t ledAzul;
	uint8_t ledVerde;
	uint16_t TIMER_MOD;

} state;

int main(void) {
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitBootPeripherals();
    BOARD_InitDebugConsole();

    tpm_config_t config;
    TPM_GetDefaultConfig(&config);
    config.prescale= kTPM_Prescale_Divide_128;
    TPM_Init(TPM0, &config);

    state FSM[3];
    FSM[0]=(state){.ledRojo=0u, .ledVerde=1u, .ledAzul=1u, .TIMER_MOD=256u}; //1s
    FSM[1]=(state){.ledRojo=1u, .ledVerde=0u, .ledAzul=1u, .TIMER_MOD=768u}; //3s
    FSM[2]=(state){.ledRojo=1u, .ledVerde=1u, .ledAzul=0u, .TIMER_MOD=1280u}; //5s

    uint8_t estados=0;
    uint8_t TIMER_ON=0;
    uint32_t timerbandera;


    while(1) {
    	GPIO_WritePinOutput(GPIOB, Rojo, FSM[estados].ledRojo);
    	GPIO_WritePinOutput(GPIOB, Verde, FSM[estados].ledVerde);
    	GPIO_WritePinOutput(GPIOD, Azul, FSM[estados].ledAzul);

     	timerbandera=TPM_GetStatusFlags(TPM0);

     	switch (estados){
     	case 0:
     		if(TIMER_ON==0){
     			TIMER_ON=1;
     	      	TPM_SetTimerPeriod(TPM0, FSM[0].TIMER_MOD);
     			TPM_StartTimer(TPM0, kTPM_SystemClock);
     		}
     		if(timerbandera){
     		    TPM_ClearStatusFlags(TPM0, 1u<8u);
     		    TIMER_ON=0;
     		    TPM_StopTimer(TPM0);
     		    TPM0->CNT=0;
     		    estados =1;
     		}
     		break;
     	case 1:
     	     if(TIMER_ON==0){
     	        TPM_SetTimerPeriod(TPM0, FSM[1].TIMER_MOD);
     	     	TPM_StartTimer(TPM0, kTPM_SystemClock);
     	     	TIMER_ON=1;
     	     	}
     	     if(timerbandera){
     	        TPM_ClearStatusFlags(TPM0, 1u<8u);
     	        TIMER_ON=0;
     	     	TPM_StopTimer(TPM0);
     	     	TPM0->CNT=0;
     	     	estados =2;
     	     	}
     	     break;
     	case 2:
     	      if(TIMER_ON==0){
     	        TPM_SetTimerPeriod(TPM0, FSM[2].TIMER_MOD);
     	        TPM_StartTimer(TPM0, kTPM_SystemClock);
     	        TIMER_ON=1;
     	       }
     	       if(timberbandera){
     	         TPM_ClearStatusFlags(TPM0, 1u<8u);
     	         TIMER_ON=0;
     	         TPM_StopTimer(TPM0);
     	         TPM0->CNT=0;
     	         estados=0;
     	        }
     	       break;
     	}
     	}

}
