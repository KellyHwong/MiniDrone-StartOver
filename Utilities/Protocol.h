/**
* @Filename: Protocol.h
* @Author: Kelly Hwong
* @Update: 2015.7.5
* @Description: 串口通信协议类，通用跨平台
*  安卓端APP蓝牙串口助手提供了协议，可以用来直接发送数据，现在只要把协议解析出来
*/
#ifndef PROTOCOL_H_
#define PROTOCOL_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include <stdint.h>

#define PROTOCOL_RXBUFFLE_SIZE 80

class Protocol {
private:
  uint8_t cmd_string_[PROTOCOL_RXBUFFLE_SIZE];
public:
  Protocol(void);
  void cmd_string(uint8_t cmd*);
}; // class Protocol

#ifdef __cplusplus
}
#endif

#endif
