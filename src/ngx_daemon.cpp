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
        ngx_log.error_core(NGX_LOG_EMERG, errno, "ngx_daemon()中fork()失败！");
        return -1;
    case 1:
        // 回到主线程释放资源
        return 1;
    case 0:
        break;
    }

    ngx_ppid = ngx_pid;
    ngx_pid = getpid();

    if (setsid() == -1)
    {
        ngx_log.error_core(NGX_LOG_EMERG, errno, "ngx_daemon()中setsid()失败!");
        return -1;
    }

    umask(0);

    chdir("/");

    // 打开黑洞设备，以读写方式打开
    int fd = open("/dev/null", O_RDWR);
    if (fd == -1)
    {
        ngx_log.error_core(NGX_LOG_EMERG, errno, "ngx_daemon()中open(\"/dev/null\")失败!");
        return -1;
    }
    if (dup2(fd, STDIN_FILENO) == -1)
    {
        ngx_log.error_core(NGX_LOG_EMERG, errno, "ngx_daemon()中dup2(STDIN)失败!");
        return -1;
    }
    if (dup2(fd, STDOUT_FILENO) == -1)
    {
        ngx_log.error_core(NGX_LOG_EMERG, errno, "ngx_daemon()中dup2(STDOUT)失败!");
        return -1;
    }
    if (fd > STDERR_FILENO)
    {
        if (close(fd) == -1)
        {
            ngx_log.error_core(NGX_LOG_EMERG, errno, "ngx_daemon()中close(fd)失败!");
            return -1;
        }
    }
    return 0;
}