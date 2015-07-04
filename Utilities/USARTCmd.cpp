#include "USARTCmd.h"
#include <stdint.h>

  USARTCmd();
  USARTCmd(uint8_t** cmd_string_addr);
  void Execute(void); // 这个只是把exe_flag_置1，真正的处理是跳读例程去做的
  void Routine(void); // 调度器例程

void USARTCmd::USARTCmd() {

}

void USARTCmd::USARTCmd(uint8_t** cmd_string_addr) {
  this->
}

void USARTCmd::Execute(void) {
  this->exe_flag_ = 1;
}

void USARTCmd::Routine(void) {
  if (exe_flag_) {

    exe_flag_ = 0;
  }
}

void USARTCmd::Parse(void) {

}
