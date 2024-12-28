#include "ngx_global.h"
#include "ngx_c_log.h"
#include "ngx_func.h"
#include "ngx_c_config.h"
#include "ngx_macro.h"

pid_t ngx_ppid;
pid_t ngx_pid;

CLog ngx_log;

#include <iostream>
using namespace std;

int main(int argc, char **argv)
{
    // 读配置文件到内存中
    CConfig &conf = CConfig::getInstance();
    if (false == conf.load(NGX_CONFIG_FILE_PATH))
    {
        ngx_log.error_stderr(0, std::string("配置文件[") + NGX_CONFIG_FILE_PATH + "]载入失败，退出！");
    }

    // 日志系统初始化
    ngx_log.init();

    // 是否以守护进程运行，默认关闭
    if (1 == conf.getIntDefault("daemon", 0))
    {
        int daemonRet = ngx_daemon();
        if (-1 == daemonRet)
        {
        }
        else if (1 == daemonRet)
        {
        }
        else
        {
        }
    }
}