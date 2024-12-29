// 该文件为全局可见声明
#ifndef NGX_GLOBAL_H
#define NGX_GLOBAL_H

#include <signal.h>

#include "ngx_c_log.h"

extern bool ngx_stop_flg;
extern CLog ngx_log;
extern pid_t ngx_pid;
extern pid_t ngx_ppid;
extern int ngx_proc_type;
extern sig_atomic_t ngx_reap;
extern sig_atomic_t ngx_working_proc_cnt;

#endif