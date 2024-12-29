#include <unistd.h>
#include <signal.h>

#include "ngx_global.h"
#include "ngx_c_log.h"
#include "ngx_func.h"
#include "ngx_c_config.h"
#include "ngx_macro.h"

bool ngx_stop_flg; // 程序关闭

pid_t ngx_ppid;    // ppid
pid_t ngx_pid;     // pid
int ngx_proc_type; // 进程类型

sig_atomic_t ngx_reap;             // 记录是否有子进程状态
sig_atomic_t ngx_working_proc_cnt; // 记录正在工作的子进程个数

CLog ngx_log;

#include <iostream>
using namespace std;

int main(int argc, char **argv)
{
    ngx_stop_flg = false;

    ngx_ppid = getppid();
    ngx_pid = getpid();
    ngx_proc_type = NGX_PROC_MASTER;

    ngx_reap = NGX_SUBPROC_INIT;
    ngx_working_proc_cnt = 0;

    // 读配置文件到内存中
    CConfig &conf = CConfig::getInstance();
    if (false == conf.load(NGX_CONFIG_FILE_PATH))
    {
        ngx_log.error_stderr(0, std::string("配置文件[") + NGX_CONFIG_FILE_PATH + "]载入失败，退出！");
        exit(EXIT_FAILURE);
    }

    // 日志系统初始化
    ngx_log.init();

    // 是否以守护进程运行，默认关闭
    if (1 == conf.getIntDefault("daemon", 0))
    {
        int daemonRet = ngx_daemon();
        if (-1 == daemonRet)
        {
            exit(EXIT_FAILURE);
        }
        else if (1 == daemonRet)
        {
            exit(EXIT_SUCCESS);
        }
    }

    // 初始化信号系统
    if (false == ngx_signal_init())
    {
        exit(EXIT_FAILURE);
    }

    int fd = fork();
    ngx_working_proc_cnt++;

    ngx_log.error_stderr(0, "mynginx程序退出");
    return 0;
}