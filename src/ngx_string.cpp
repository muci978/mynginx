#include <string>

#include "ngx_func.h"

/// @brief 去除字符串右边的空格
/// @param str
/// @return 若空串返回false
bool ngx_rTrim(std::string &str)
{
    size_t pos = str.find_last_not_of(' ');
    if (std::string::npos == pos)
    {
        return false;
    }
    str = str.substr(0, pos + 1);
    return true;
}

/// @brief 去除字符串左边的空格
/// @param str
/// @return 若空串返回false
bool ngx_lTrim(std::string &str)
{
    size_t pos = str.find_first_not_of(' ');
    if (std::string::npos == pos)
    {
        return false;
    }
    str = str.substr(pos);
    return true;
}