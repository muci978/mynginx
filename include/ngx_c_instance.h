// 该文件为单例类基类定义
#ifndef NGX_C_INSTANCE
#define NGX_C_INSTANCE

class CInstance
{
protected:
    CInstance() = default;
    CInstance(const CInstance &) = delete;
    CInstance(CInstance &&) = delete;
    CInstance &operator=(const CInstance &);
    CInstance &&operator=(CInstance &&);
    virtual ~CInstance() = default;
};

#endif