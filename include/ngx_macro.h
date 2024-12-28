// 该文件为宏定义
#ifndef NGX_MACRO_H
#define NGX_MACRO_H

// 日志相关
// 把日志一共分成八个等级【级别从高到低，数字最小的级别最高，数字大的级别最低】，以方便管理、显示、过滤等等
#define NGX_LOG_STDERR 0 // 控制台错误【stderr】：最高级别日志，日志的内容写入log参数指定的文件，同时也尝试直接将日志输出到标准错误设备比如控制台屏幕
#define NGX_LOG_EMERG 1  // 紧急 【emerg】
#define NGX_LOG_ALERT 2  // 警戒 【alert】
#define NGX_LOG_CRIT 3   // 严重 【crit】
#define NGX_LOG_ERR 4    // 错误 【error】：属于常用级别
#define NGX_LOG_WARN 5   // 警告 【warn】：属于常用级别
#define NGX_LOG_NOTICE 6 // 注意 【notice】
#define NGX_LOG_INFO 7   // 信息 【info】
#define NGX_LOG_DEBUG 8  // 调试 【debug】：最低级别

// 日志默认目录
#define NGX_ERR_LOG_PATH "error.log"

// 单条日志最大长度
#define NGX_MAX_ERROR_STR 2048

// 配置文件目录
#define NGX_CONFIG_FILE_PATH "mynginx.conf"

#endif