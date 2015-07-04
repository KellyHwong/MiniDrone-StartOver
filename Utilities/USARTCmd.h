#ifndef USARTCMD_H_
#define USARTCMD_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

class USARTCmd {
private:
  uint8_t exe_flag_;
  uint8_t** cmd_string_; // 字符串指针

public:
  USARTCmd();
  USARTCmd(uint8_t** cmd_string_addr);
  void Execute(void); // 这个只是把exe_flag_置1，真正的处理是跳读例程去做的
  void Routine(void); // 调度器例程
}; // class USARTCmd

#ifdef __cplusplus
}
#endif

#endif
