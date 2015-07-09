/**
* @Filename: BluetoothCmd
* @Author: Kelly Hwong
* @Update: 2015.7.7
* @Description: 原来叫USARTCmd, 但是做不到协议上通用, 所以改为BluetoothCmd
*   具体功能: 针对A5 5A(两个字节), 05(组选), F1(功能选), value, checksum形式
*     的字符串协议写解释器
*/

#ifndef BLUETOOTH_H_
#define BLUETOOTH_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

class BluetoothCmd {
private:
  uint8_t cmd_begin_[2];
  uint8_t cmd_group_sel_;
  uint8_t cmd_func_sel_;
  uint8_t cmd_value_;
  uint8_t cmd_checksum_;
  uint8_t CheckString(uint8_t* cmd_string);
  uint8_t Checksum(uint8_t* str, uint8_t n);

public:
  BluetoothCmd();
  //BluetoothCmd(void(&ptr)(uint8_t));
  void Execute(uint8_t* cmd_string);
  /* 使用数字命名很不规范! 现在先实现 */
  /* 把它 public 了 */
  void(*func1_ptr_)(uint8_t); // 函数指针, 执行动作的接口
  void(*func2_ptr_)(uint8_t); // 函数指针, 执行动作的接口
  void(*func3_ptr_)(uint8_t); // 函数指针, 执行动作的接口
}; // class BluetoothCmd

#ifdef __cplusplus
}
#endif

#endif
