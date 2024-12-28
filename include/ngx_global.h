// 该文件为全局可见声明
#ifndef NGX_GLOBAL_H
#define NGX_GLOBAL_H

#include "ngx_c_log.h"

// 日志
extern CLog ngx_log;

extern pid_t ngx_pid;
extern pid_t ngx_ppid;

#endif