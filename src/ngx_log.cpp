#include <errno.h>
#include <string.h>
#include <time.h>

#include "ngx_c_log.h"
#include "ngx_c_config.h"
#include "ngx_macro.h"
#include "ngx_global.h"

// 错误等级，和ngx_macro.h里定义的日志等级宏是一一对应关系
static std::string err_levels[9]{
    {"stderr"}, // 0：控制台错误
    {"emerg"},  // 1：紧急
    {"alert"},  // 2：警戒
    {"crit"},   // 3：严重
    {"error"},  // 4：错误
    {"warn"},   // 5：警告
    {"notice"}, // 6：注意
    {"info"},   // 7：信息
    {"debug"}   // 8：调试
};

inline bool CLog::is_open()
{
    return ferr.is_open();
}

/// @brief 初始化日志
void CLog::init()
{
    CConfig &conf = CConfig::getInstance();

    // 从配置文件读取日志相关配置信息
    std::string logName = conf.getString("logFile");
    // 若配置文件中没有相关信息则使用默认值
    if (logName.empty())
    {
        logName = NGX_ERR_LOG_PATH;
    }
    logLevel = conf.getIntDefault("logLevel", NGX_LOG_NOTICE);

    ferr.open(logName, std::ios::out | std::ios::app);
    if (is_open())
    {
        return;
    }

    error_stderr(errno, std::string("[alert] could not open error log file: open() ") + logName + " failed");
}

/// @brief 将日志输出到标准错误，如果日志文件打开，也一并输入
/// @param err errno
/// @param str 报错信息
void CLog::error_stderr(int err, const std::string &str)
{
    std::string errStr;
    errStr += "mynginx: " + str;

    if (err)
    {
        errStr += "(" + std::to_string(err) + ": " + strerror(err) + ")";
    }

    if (errStr.size() > NGX_MAX_ERROR_STR)
    {
        errStr.resize(NGX_MAX_ERROR_STR);
    }

    errStr += '\n';

    // 往标准错误输出信息
    dft << errStr;

    // 如果日志文件打开，输出到日志文件
    if (is_open())
    {
        ferr << errStr;
    }
}

/// @brief 将日志信息写入到日志文件，若失败，输出到标准输出
/// @param level 日志等级
/// @param err errno
/// @param str 日志信息
void CLog::error_core(int level, int err, const std::string &str)
{
    // 错误等级大于配置等级，不用打印
    if (level > logLevel)
    {
        return;
    }

    std::string errStr;

    time_t curSec = time(nullptr);
    struct tm tm;
    bzero(&tm, sizeof(tm));
    localtime_r(&curSec, &tm);

    ++tm.tm_mon;
    tm.tm_year += 1900;

    // 年/月/日 时:分:秒
    char curTime[20]{0};
    sprintf(curTime, "%4d/%02d/%02d %02d:%02d:%02d",
            tm.tm_year, tm.tm_mon, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
    errStr += curTime;
    errStr += ' ';

    // pid
    errStr += std::to_string(ngx_pid);
    errStr += ' ';

    // 日志级别
    errStr += '[';
    errStr += err_levels[level];
    errStr += "]: ";

    // 日志信息
    errStr += str;

    if (err)
    {
        errStr += "(" + std::to_string(err) + ": " + strerror(err) + ")";
    }

    if (errStr.size() > NGX_MAX_ERROR_STR)
    {
        errStr.resize(NGX_MAX_ERROR_STR);
    }

    errStr += '\n';

    // 输出到日志文件
    ferr << errStr;

    // 如果写失败，输出到标准错误
    if (!ferr)
    {
        dft << errStr;
    }
}