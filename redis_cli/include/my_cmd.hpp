#ifndef _MY_CMD_FN_PTR_
#define _MY_CMD_FN_PTR_

// #include <main_CLI_header.h>
// #include <stdio.h>
#include <functional> 

// #ifdef __cplusplus
// extern "C" {
// #endif

#define _PRINT_FIRST_ "PRINT_FIRST"
#define _PRINT_SECOND_ "PRINT_SECOND"
#define _QUIT_ "QUIT"

class MyGetOpt;

#ifndef _CMD_PTR_S_
#define _CMD_PTR_S_
struct cmd_ptr_s
{
    const char* cmd;
    std::function<int(MyGetOpt&)> func_ptr;
};
typedef struct cmd_ptr_s cmd_ptr_t;
#endif

int print_first(MyGetOpt& GetOptObj);
int print_second(MyGetOpt& GetOptObj);
int quit(MyGetOpt& GetOptObj);
int execute_cmd(MyGetOpt& GetOptObj);

// #ifdef __cplusplus
// }
// #endif
#endif