#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include "ngx_func.h"
#include "ngx_macro.h"
#include "ngx_c_log.h"
#include "ngx_global.h"

/// @brief 创建守护进程
/// @return 失败：-1，成功：父进程：1，子进程：0
int ngx_daemon()
{
    switch (fork())
    {
    case -1:
        return -1;
    case 0:
        break;
    default:
        // 回到主线程释放资源
        return 1;
    }

    ngx_ppid = ngx_pid;
    ngx_pid = getpid();

    // pid_t sid_before = getsid(0);
    // ngx_log.error_core(NGX_LOG_NOTICE, 0, std::string("setsid()前的会话ID: ") + std::to_string(sid_before));

    if (-1 == setsid())
    {
        ngx_log.error_core(NGX_LOG_EMERG, errno, "ngx_daemon()中setsid()失败!");
        return -1;
    }

    // pid_t sid_after = getsid(0);
    // ngx_log.error_core(NGX_LOG_NOTICE, 0, std::string("setsid()后的会话ID: ") + std::to_string(sid_after));

    umask(0);

    chdir("/");

    // 打开黑洞设备，以读写方式打开
    int fd = open("/dev/null", O_RDWR);
    if (-1 == fd)
    {
        ngx_log.error_core(NGX_LOG_EMERG, errno, "ngx_daemon()中open(\"/dev/null\")失败!");
        return -1;
    }
    if (-1 == dup2(fd, STDIN_FILENO))
    {
        ngx_log.error_core(NGX_LOG_EMERG, errno, "ngx_daemon()中dup2(STDIN)失败!");
        return -1;
    }
    if (-1 == dup2(fd, STDOUT_FILENO))
    {
        ngx_log.error_core(NGX_LOG_EMERG, errno, "ngx_daemon()中dup2(STDOUT)失败!");
        return -1;
    }
    if (fd > STDERR_FILENO)
    {
        if (-1 == close(fd))
        {
            ngx_log.error_core(NGX_LOG_EMERG, errno, "ngx_daemon()中close(fd)失败!");
            return -1;
        }
    }
    return 0;
}