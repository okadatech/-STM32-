/*
 * L6470.c
 *
 *  Created on: 2018/03/14
 *      Author: okada_tech
 */
#include "L6470.h"

void L6470_init(SPI_HandleTypeDef *handler){
	 L6470_SPI = handler;
	 L6470_Resets();
}

unsigned char L6470_send(unsigned char temp){

	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_6,0);
    uint8_t Rxtemp[1];
    uint8_t Txtemp[1];
    Txtemp[0]=temp;
    HAL_SPI_TransmitReceive(L6470_SPI,(uint8_t*)Txtemp,(uint8_t*)Rxtemp,1,2000);
    temp=Rxtemp[0];
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_6,1);
    delayUs(1);
    return(temp);
}

void L6470_send_bytes(unsigned char temp[],int i){
    while(0 < i--){
        temp[i] = L6470_send(temp[i]);
    }
}

void L6470_NOP(){
	L6470_send(0x00);
}

void L6470_SetParam(int param,int value){
    int n = (param>>8)/8;
    int m = (param>>8)%8;
//        pc.printf("palam = %x\n",param);
//        pc.printf("n = %x\n",n);
//        pc.printf("m = %x\n",m);
    if(m==0){
        unsigned char temp[n+1];
        temp[n] = 0x00|(unsigned char)(param&0xFF);
        while(0 < n--){
            temp[n]=(unsigned char) (value >> 8*n)&0xFF;
        }
        L6470_send_bytes(temp,sizeof temp/sizeof temp[0]);
    }else{
        unsigned char temp[n+2];
        temp[n+1] = 0x00|(unsigned char)(param&0xFF);
        temp[n] =(unsigned char) (value >> 8*n)&~(0xff<<m);
        while(0 < n--){
            temp[n]=(unsigned char) (value >> 8*n)&0xFF;
        }
        L6470_send_bytes(temp,sizeof temp/sizeof temp[0]);
    }
}




int L6470_GetParam(int param){
    int value = 0;
    int n = (param>>8)/8;
    int m = (param>>8)%8;
    if(m==0){
        unsigned char temp[n+1];
        for(int i = 0; i < n+1; i++){
            temp[i]=0;
        }
        temp[n] = 0x20|(unsigned char)(param&0xFF);
        L6470_send_bytes(temp,sizeof temp/sizeof temp[0]);
        while(0 < n--){
            value |= (int)temp[n] << 8*n;
        }
    }else{
        n++;
        unsigned char temp[n+1];
        for(int i = 0; i < n+2; i++){
            temp[i]=0;
        }
        temp[n] = 0x20|(unsigned char)(param&0xFF);
        L6470_send_bytes(temp,sizeof temp/sizeof temp[0]);
        while(0 < n--){
            value |= (int)temp[n] << 8*n;
        }
    }

return(value);
}



void L6470_Run(unsigned char dir,int spd){
	// 速度でまわす
    unsigned char temp[4];
    temp[3] = 0x50|dir;
    temp[2] = (unsigned char) (spd >> 16)&0x0F;
    temp[1] = (unsigned char) (spd >>  8)&0xFF;
    temp[0] = (unsigned char) (spd >>  0)&0xFF;
    L6470_send_bytes(temp,sizeof temp/sizeof temp[0]);
}

void L6470_StepClock(unsigned char dir){
	L6470_send(0x58|dir);
}


void L6470_Move(unsigned char dir,int n_step){
	//(マイクロステップ ステップ数)回す
    unsigned char temp[4];
    temp[3] = 0x40|dir;
    temp[2] = (unsigned char) (n_step >> 16)&0x3F;
    temp[1] = (unsigned char) (n_step >>  8)&0xFF;
    temp[0] = (unsigned char) (n_step >>  0)&0xFF;
    L6470_send_bytes(temp,sizeof temp/sizeof temp[0]);
}

void L6470_GoTo(int abs_pos){
	//絶対位置で回す
    unsigned char temp[4];
    temp[3] = 0x60;
    temp[2] = (unsigned char) (abs_pos >> 16)&0x3F;
    temp[1] = (unsigned char) (abs_pos >>  8)&0xFF;
    temp[0] = (unsigned char) (abs_pos >>  0)&0xFF;
    L6470_send_bytes(temp,sizeof temp/sizeof temp[0]);
}


void L6470_GoTo_DIR(unsigned char dir,int abs_pos){
    unsigned char temp[4];
    temp[3] = 0x68|dir;
    temp[2] = (unsigned char) (abs_pos >> 16)&0x3F;
    temp[1] = (unsigned char) (abs_pos >>  8)&0xFF;
    temp[0] = (unsigned char) (abs_pos >>  0)&0xFF;
    L6470_send_bytes(temp,sizeof temp/sizeof temp[0]);
}


void L6470_GoUntil(unsigned char act,unsigned char dir,int spd){
    unsigned char temp[4];
    temp[3] = 0x82|(act << 3)|dir;
    temp[2] = (unsigned char) (spd >> 16)&0x0F;
    temp[1] = (unsigned char) (spd >>  8)&0xFF;
    temp[0] = (unsigned char) (spd >>  0)&0xFF;
    L6470_send_bytes(temp,sizeof temp/sizeof temp[0]);
}

void L6470_ReleaseSW(unsigned char act,unsigned char dir){
	L6470_send(0x92|(act << 3)|dir);
}

void L6470_GoHome(){
	L6470_send(0x70);
}

void L6470_GoMark(){
	L6470_send(0x78);
}

void L6470_ResetPos(){
	L6470_send(0xD8);
}

void L6470_ResetDevice(){
	L6470_send(0xC0);
}

void L6470_SoftStop(){
	//停まる
	L6470_send(0xB0);
}

void L6470_HardStop(){
	L6470_send(0xB8);
}


void L6470_SoftHiZ(){
	L6470_send(0xA0);
}

void L6470_HardHiZ(){
	L6470_send(0xA8);
}

void L6470_Resets(){
	// モータに初期パラメータを書き込む
    L6470_SoftStop();
    L6470_ResetDevice();
    L6470_SetParam(ABS_POS,INI_ABS_POS);
    L6470_SetParam(EL_POS,INI_EL_POS);
    L6470_SetParam(MARK,INI_MARK);
    L6470_SetParam(SPEED,INI_SPEED);
    L6470_SetParam(ACC,INI_ACC);
    L6470_SetParam(DEC,INI_DEC);
    L6470_SetParam(MAX_SPEED,INI_MAX_SPEED);
    L6470_SetParam(MIN_SPEED,INI_MIN_SPEED);
    L6470_SetParam(KVAL_HOLD,INI_KVAL_HOLD);
    L6470_SetParam(KVAL_RUN,INI_KVAL_RUN);
    L6470_SetParam(KVAL_ACC,INI_KVAL_ACC);
    L6470_SetParam(KVAL_DEC,INI_KVAL_DEC);
    L6470_SetParam(INT_SPD,INI_INT_SPD);
    L6470_SetParam(ST_SLP,INI_ST_SLP);
    L6470_SetParam(FN_SLP_ACC,INI_FN_SLP_ACC);
    L6470_SetParam(FN_SLP_DEC,INI_FN_SLP_DEC);
    L6470_SetParam(K_THERA,INI_K_THERA);
    L6470_SetParam(OCR_TH,INI_OCR_TH);
    L6470_SetParam(STALL_TH,INI_STALL_TH);
    L6470_SetParam(FS_SPD,INI_FS_SPD);
    L6470_SetParam(STEP_MODE,INI_STEP_MODE);
    L6470_SetParam(ARARM_FN,INI_ARARM_FN);
    L6470_SetParam(CONFIG,INI_CONFIG);
}

void L6470_busydelay(int time){
	  while(!HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_8)){
		  asm("nop");
	  }
	  L6470_HardHiZ();
	  HAL_Delay(time);
}



uint32_t getUs(void) {
uint32_t usTicks = HAL_RCC_GetSysClockFreq() / 1000000;
register uint32_t ms, cycle_cnt;
do {
ms = HAL_GetTick();
cycle_cnt = SysTick->VAL;
} while (ms != HAL_GetTick());
return (ms * 1000) + (usTicks * 1000 - cycle_cnt) / usTicks;
}

void delayUs(uint16_t micros) {
uint32_t start = getUs();
while (getUs()-start < (uint32_t) micros) {
	asm("nop");
}
}
