// 该文件为日志类定义
#ifndef NGX_C_LOG_H
#define NGX_C_LOG_H

#include <string>
#include <iostream>
#include <fstream>

struct CLog
{
private:
    int logLevel;
    std::ostream &dft = std::cerr; // 标准错误输出流
    std::ofstream ferr;            // 日志文件输出
private:
    bool is_open();

public:
    void init();
    void error_stderr(int err, const std::string &str);
    void error_core(int level, int err, const std::string &str);

public:
    ~CLog()
    {
        if (is_open())
        {
            ferr.flush();
            ferr.close();
        }
        dft.flush();
    }
};

#endif