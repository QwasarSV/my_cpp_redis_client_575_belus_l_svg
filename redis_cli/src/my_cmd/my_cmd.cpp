#include <main_header.hpp>

cmd_ptr_t cmd_ptr_map[] = {
    {_PRINT_FIRST_, print_first},
    {_PRINT_SECOND_, print_second},
    {_QUIT_, quit},
    {NULL, nullptr}
};

int print_first(MyGetOpt& GetOptObj)
{
    (void)GetOptObj;
    std::cout << "first" << std::endl;
    return EXIT_SUCCESS;
}

int print_second(MyGetOpt& GetOptObj)
{
    (void)GetOptObj;
    std::cout << "second" << std::endl;
    return EXIT_SUCCESS;
}

int quit(MyGetOpt& GetOptObj)
{
    GetOptObj.setExit();
    std::cout << "quit cmd: exiting program" << std::endl;
    return EXIT_SUCCESS;
}

int execute_cmd(MyGetOpt& GetOptObj)
{
    cmd_ptr_t* cf_ptr = cmd_ptr_map;

    while (cf_ptr->cmd != NULL)
    {
        if (_my_strcmp(GetOptObj.getKeyword(), cf_ptr->cmd) == 0)
        {
            return cf_ptr->func_ptr(GetOptObj);
        }
        cf_ptr++;
    }
    return EXIT_SUCCESS;
}