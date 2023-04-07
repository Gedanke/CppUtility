#ifndef MYUTIL_PARSER_H
#define MYUTIL_PARSER_H

#include "element.h"

static const int detail_len = 64;

namespace xml
{
    class Parser
    {
    public:
        // 加载文件
        void loadFile(const string &fileName);
        // 加载字符串
        void loadString(string_view content);
        // 解析
        Element parse();
        // 加载文件
        static Element fromFile(const string &fileName);
        // 加载字符串
        static Element fromString(string_view content);

    private:
        // 获取下一个 token
        char _get_next_token();
        // 去除左右边多余空白字符
        void _trim_right();
        // 版本
        bool _parse_version();
        // 注释
        bool _parse_comment();
        // 解析节点
        Element _parse_element();
        // 解析属性的键
        string _parse_attr_key();
        // 解析属性的值
        string _parse_attr_value();

    private:
        // xml 字符串
        string _str;
        // token 索引
        size_t _idx{};
    };
}

#endif // MYUTIL_PARSER_H