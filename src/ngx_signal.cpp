#include <string>
#include <signal.h>
#include <string.h>
#include <wait.h>

#include "ngx_global.h"
#include "ngx_macro.h"
#include "ngx_func.h"

struct ngx_signal_t
{
    using handle = void (*)(int sigNo);

    int sigNo;           // 信号值
    std::string sigName; // 信号名
    handle callback;     // 信号处理函数
};

void ngx_signal_handler(int sigNo);
void ngx_proc_get_status();
void ngx_close_all();

static ngx_signal_t signals[]{
    {SIGHUP, "SIGHUP", ngx_signal_handler},
    {SIGINT, "SIGINT", ngx_signal_handler},
    {SIGTERM, "SIGTERM", ngx_signal_handler},
    {SIGCHLD, "SIGCHLD", ngx_signal_handler},
    {SIGQUIT, "SIGQUIT", ngx_signal_handler},
    {SIGIO, "SIGIO", ngx_signal_handler},
    {SIGSYS, "SIG_SYS, SIG_IGN", nullptr},
};

/// @brief 初始化信号系统，注册信号处理函数
/// @return 成功：true，失败：false
bool ngx_signal_init()
{
    struct sigaction act;

    for (const auto &signal : signals)
    {
        bzero(&act, sizeof(act));
        sigemptyset(&act.sa_mask);
        act.sa_flags = 0;
        act.sa_handler = nullptr == signal.callback ? SIG_IGN : signal.callback;

        if (-1 == sigaction(signal.sigNo, &act, nullptr))
        {
            ngx_log.error_core(NGX_LOG_EMERG, errno, std::string("sigaction(") + signal.sigName + ") failed!");
            return false;
        }
    }

    return true;
}

/// @brief 信号处理函数
/// @param sigNo 信号值
void ngx_signal_handler(int sigNo)
{
    ngx_signal_t sig;
    for (const auto &signal : signals)
    {
        if (signal.sigNo == sigNo)
        {
            sig = signal;
            break;
        }
    }

    if (ngx_proc_type == NGX_PROC_MASTER)
    {
        switch (sigNo)
        {
        case SIGCHLD:
            ngx_reap = NGX_SUBPROC_EXIT;
            break;

        case SIGTERM:
        case SIGINT:
            ngx_close_all();
            return;

        default:
            break;
        }
    }
    else if (ngx_proc_type == NGX_PROC_WORKER)
    {
        switch (sigNo)
        {
        case SIGTERM:
            // exit(0);
            ngx_stop_flg = true;
            return;

        default:
            break;
        }
    }

    ngx_log.error_core(NGX_LOG_NOTICE, 0, std::string("signal ") + std::to_string(sigNo) + "(" + sig.sigName + ")");

    if (sigNo == SIGCHLD)
    {
        ngx_proc_get_status();
    }
}

/// @brief 取得子进程退出状态
void ngx_proc_get_status()
{
    int one = 0;
    for (;;)
    {
        int status;
        pid_t pid = waitpid(-1, &status, WNOHANG);

        if (0 == pid) // 所有退出的子进程均已处理，退出
        {
            break;
        }
        else if (-1 == pid) // 发生错误，仿照nginx处理
        {
            int err = errno;
            if (EINTR == err) // 发生中断，无碍
            {
                continue;
            }
            if (ECHILD == err && one) // 没有子进程
            {
                return;
            }
            if (ECHILD == err)
            {
                ngx_log.error_core(NGX_LOG_INFO, err, "waitpid() failed!");
                return;
            }

            ngx_log.error_core(NGX_LOG_ALERT, err, "waitpid() failed!");
            return;
        }

        --ngx_working_proc_cnt;
        one = 1;

        if (WTERMSIG(status))
        {
            ngx_log.error_core(NGX_LOG_ALERT, 0, std::string("pid = ") + std::to_string(pid) + " exited on signal " + std::to_string(WTERMSIG(status)) + "!");
        }
        else
        {
            ngx_log.error_core(NGX_LOG_NOTICE, 0, std::string("pid = ") + std::to_string(pid) + " exited with code " + std::to_string(WEXITSTATUS(status)) + "!");
        }
    }
}

/// @brief 关闭所有的子进程
void ngx_close_all()
{
    // 过滤二次进入函数
    if (true == ngx_stop_flg)
    {
        return;
    }
    ngx_stop_flg = true;

    ngx_log.error_core(NGX_LOG_NOTICE, 0, "准备关闭，向所有子进程发送SIGTERM...");

    // 给当前进程组内所有进程发送SIGTERM
    kill(0, SIGTERM);

    while (ngx_working_proc_cnt > 0)
    {
        ngx_log.error_core(NGX_LOG_NOTICE, 0, std::string("等待") + std::to_string(ngx_working_proc_cnt) + "个worker进程退出...");

        ngx_proc_get_status();

        // 放弃时间片，让子进程退出
        usleep(1);
    }
}