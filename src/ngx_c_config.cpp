#include <fstream>

#include "ngx_c_config.h"
#include "ngx_func.h"

/// @brief 装载配置文件
/// @param fileName 配置文件路径
/// @return 成功：true，失败：false
bool CConfig::load(const std::string &fileName)
{
    std::ifstream in;
    in.open(fileName, std::ios::in);

    if (!in.is_open())
    {
        return false;
    }

    std::string lineBuf;

    // 循环读取每一行
    while (getline(in, lineBuf))
    {
        if (lineBuf.empty() || 0 == lineBuf[0])
        {
            continue;
        }

        if (!ngx_lTrim(lineBuf))
        {
            continue;
        }

        // 处理注释行
        if (';' == lineBuf[0] || '#' == lineBuf[0] || '\t' == lineBuf[0] || '[' == lineBuf[0])
        {
            continue;
        }

        // 处理行尾的换行、回车和空格
        while (!lineBuf.empty())
        {
            if (10 == lineBuf[lineBuf.size() - 1] || 13 == lineBuf[lineBuf.size() - 1] || 32 == lineBuf[lineBuf.size() - 1])
            {
                lineBuf.resize(lineBuf.size() - 1);
            }
            else
            {
                break;
            }
        }

        if (lineBuf.empty())
        {
            continue;
        }

        // xxx=xxx
        size_t pos = lineBuf.find('=');
        if (std::string::npos != pos)
        {
            std::string name = lineBuf.substr(0, pos);
            std::string content = lineBuf.substr(pos + 1);
            ngx_lTrim(name);
            ngx_rTrim(name);
            ngx_lTrim(content);
            ngx_rTrim(content);
            confItemList[name] = content;
        }
    }

    in.close();
    return true;
}

/// @brief 根据配置名找到配置信息，返回字符串
/// @param itemName
/// @return 失败返回空字符串
const std::string CConfig::getString(const std::string &itemName)
{
    if (confItemList.end() == confItemList.find(itemName))
    {
        return "";
    }
    return confItemList[itemName];
}

/// @brief 根据配置名找到配置信息，返回整型
/// @param itemName
/// @param dft 默认值
/// @return 失败返回默认值
int CConfig::getIntDefault(const std::string &itemName, const int dft)
{
    if (confItemList.end() == confItemList.find(itemName))
    {
        return dft;
    }
    return stoi(confItemList[itemName]);
}
