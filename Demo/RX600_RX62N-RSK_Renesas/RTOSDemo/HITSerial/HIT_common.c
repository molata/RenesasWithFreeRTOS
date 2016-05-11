#include "HIT_common.h"
/*************** 207通信相关的变量 *********************/
uchar ucCapture_success = 0;               // 指令捕获结果
ST_SERIAL_DATA stSerial_data = {0};        // 内部状态的结构体， 接收到其他传感器后，修改这个结构体就可以
ST_PC_CMD st_pc_cmd = {0};                 // 计算机指令
UNSENSOR_STATES unAllsensor_states = {0};  // 内部所有传感器的状态
