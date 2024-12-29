// 该文件为全局函数声明
#ifndef NGX_FUNC_H
#define NGX_FUNC_H

#include <string>

int ngx_daemon();
bool ngx_signal_init();

bool ngx_rTrim(std::string &str);
bool ngx_lTrim(std::string &str);

#endif