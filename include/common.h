#ifndef COMMON_H
#define COMMON_H

#include <Arduino.h>

#define TTL_RS_enable       4
#define RECEIVE_ENABLE      0
#define TRANSMIT_ENABLE     1
#define BUFFER_SIZE 100
#define UART_BUFFER_SIZE 40  // Kích thước mỗi mảng con
#define TOTAL_UARTS 4  // Số lượng mảng con
#define id1 "lave 1"  // esp8266 rem
#define id2 "lave 2"  // esp8266 be ca
extern char cmd[BUFFER_SIZE];
extern char data_main[BUFFER_SIZE];
extern char data1[UART_BUFFER_SIZE];
extern char data2[UART_BUFFER_SIZE];
extern char data3[UART_BUFFER_SIZE];
extern char data4[UART_BUFFER_SIZE];
extern char *data[TOTAL_UARTS];

extern bool flag;
extern int8_t  indexDataSubArr;                    //so thu tu mang con de ghi
extern uint8_t indexArrCpt;                        //vi tri cua mang nhan lenh tu may tinh de debug
extern uint8_t indexArr;                           //vi tri luu tai mang chung
extern uint8_t indexReadPos;                       //vi tri doc tai mang chung
extern uint8_t indexDataWritePos[TOTAL_UARTS];    //vi tri ghi cua tung mang con
extern uint8_t indexDataReadPos;                   //vi tri doc tai mang con

//cac bien du lieu

//thông số ở slave 1
extern int8_t curtainPosition;
extern uint8_t switchState;

//thông số ở slave 2
extern int8_t temperature;
extern int turbidity;
extern uint8_t waterPump;

void init();
void running();
void receive_cmd();
void send_cmd();
void receive_status();
void write_SubArr();
void update();
void reset();
void check_status();
#endif