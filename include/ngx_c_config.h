// 该文件为配置单例类定义
#ifndef NGX_C_CONFIG_H
#define NGX_C_CONFIG_H

#include <string>
#include <unordered_map>

#include "ngx_c_instance.h"

class CConfig final : CInstance
{
public:
    static CConfig &getInstance()
    {
        static CConfig c;
        return c;
    }

public:
    bool load(const std::string &fileName);
    const std::string getString(const std::string &itemName);
    int getIntDefault(const std::string &itemName, const int dft);

private:
    // itemName-itemContent
    std::unordered_map<std::string, std::string> confItemList;
};

#endif