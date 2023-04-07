#include "parser.h"
#include <fstream>
#include <sstream>
#include <filesystem>
#include <algorithm>

using namespace xml;
using std::ifstream;
using std::ostringstream;

// 用于返回较为详细的错误信息，方便错误追踪
#define THROW_ERROR(error_info, error_detail)      \
    do                                             \
    {                                              \
        string info = "parse error in ";           \
        string file_pos = __FILE__;                \
        file_pos.append(":");                      \
        file_pos.append(std::to_string(__LINE__)); \
        info += file_pos;                          \
        info += ", ";                              \
        info += error_info;                        \
        info += "\ndetail:";                       \
        info += (error_detail);                    \
        throw std::logic_error(info);              \
    } while (false)

void xml::Parser::loadFile(const string &fileName)
{
    ifstream ifs(fileName);

    if (!ifs)
    {
        ostringstream info;
        info << "load file error: file path not exist! path: " << fileName;
        info << " current path: " << std::filesystem::current_path();
        throw std::runtime_error(info.str());
    }

    // 把一个文本文件拷贝到一个字符串对象中
    this->_str = std::move(string(std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>()));
    this->_idx = 0;
    this->_trim_right();
}

void xml::Parser::loadString(string_view content)
{
    this->_str = content;
    this->_idx = 0;
    this->_trim_right();
}

Element xml::Parser::fromFile(const string &fileName)
{
    static Parser instance;
    instance.loadFile(fileName);
    return instance.parse();
}

Element xml::Parser::fromString(string_view content)
{
    static Parser instance;
    instance.loadString(content);
    return instance.parse();
}

char xml::Parser::_get_next_token()
{
    while (this->_idx < this->_str.size() && isspace(this->_str[this->_idx]))
    {
        this->_idx++;
    }
    if (this->_idx >= this->_str.size())
    {
        THROW_ERROR("invaild format", "out of length");
    }

    return this->_str[this->_idx];
}

void xml::Parser::_trim_right()
{
    // 从其 reverse_iterator 还原基础迭代器
    this->_str.erase(std::find_if(this->_str.rbegin(), this->_str.rend(), [](char x)
                                  { return !isspace(x); })
                         .base(),
                     this->_str.end());

    // 格式验证
    if (this->_str.back() != '>')
    {
        THROW_ERROR("format error, expected '>'", this->_str.substr(this->_str.size() - detail_len));
    }
}

Element xml::Parser::parse()
{
    while (true)
    {
        char t = this->_get_next_token();

        if (t != '<')
        {
            THROW_ERROR("invalid format", this->_str.substr(this->_idx, detail_len));
        }

        // 解析版本号
        if (this->_idx + 4 < this->_str.size() && this->_str.compare(this->_idx, 5, "<?xml") == 0)
        {
            if (!this->_parse_version())
            {
                THROW_ERROR("version parse error", this->_str.substr(this->_idx, detail_len));
            }
            continue;
        }

        // 解析注释
        if (this->_idx + 3 < this->_str.size() && this->_str.compare(this->_idx, 4, "<!--") == 0)
        {
            if (!this->_parse_comment())
            {
                THROW_ERROR("comment parse error", this->_str.substr(this->_idx, detail_len));
            }
            continue;
        }

        // 解析 element
        if (this->_idx + 1 < this->_str.size() && (isalpha(this->_str[this->_idx + 1]) || this->_str[this->_idx + 1] == '_'))
        {
            return this->_parse_element();
        }

        // 出现未定义情况，抛出异常
        THROW_ERROR("error format", this->_str.substr(this->_idx, detail_len));
    }
}

bool xml::Parser::_parse_version()
{
    auto pos = this->_str.find("?>", this->_idx);
    // 找不到
    if (pos == string::npos)
    {
        return false;
    }
    // 挪动下标
    this->_idx = pos + 2;
    return true;
}

bool xml::Parser::_parse_comment()
{
    auto pos = this->_str.find("-->", this->_idx);
    // 找不到
    if (pos == string::npos)
    {
        return false;
    }
    // 挪动下半
    this->_idx = pos + 3;
    return true;
}

Element xml::Parser::_parse_element()
{
    Element element;
    // 过滤掉 <
    this->_idx++;
    auto pre_pos = this->_idx;

    // 判断 name 字段的合法性
    if (!(this->_idx < this->_str.size() && (std::isalpha(this->_str[this->_idx]) || this->_str[this->_idx] == '_')))
    {
        THROW_ERROR("error occur in parse name", this->_str.substr(this->_idx, detail_len));
    }

    // 解析 name
    while (this->_idx < this->_str.size() && (isalpha(this->_str[this->_idx]) || this->_str[this->_idx] == ':' || this->_str[this->_idx] == '-' || this->_str[this->_idx] == '_' || this->_str[this->_idx] == '.'))
    {
        this->_idx++;
    }

    if (this->_idx > this->_str.size())
    {
        THROW_ERROR("error occur in parse name", this->_str.substr(this->_idx, detail_len));
    }

    element.Name() = this->_str.substr(pre_pos, this->_idx - pre_pos);

    // 解析内部
    while (this->_idx < this->_str.size())
    {
        auto token = this->_get_next_token();

        // 单元素，直接解析后结束
        if (token == '/')
        {
            if (this->_str[this->_idx + 1] == '>')
            {
                this->_idx += 2;
                return element;
            }
            else
            {
                THROW_ERROR("parse single element failed", this->_str.substr(this->_idx, detail_len));
            }
        }

        // 对应两种情况：该标签的 text 内容，下个标签的开始或者注释(直接 continue 跳到下次循环即可)
        if (token == '>')
        {
            this->_idx++;
            // 判断下一个 token 是否为 text，如果不是则 continue
            char x = this->_get_next_token();
            // 不可能是结束符，因为 xml 元素不能为空，出现这种情况可能是中间夹杂了注释
            if (x == '<')
            {
                continue;
            }

            // 解析 text 再解析 child
            auto pos = this->_str.find('<', this->_idx);
            if (pos == string::npos)
            {
                THROW_ERROR("parse text error", this->_str.substr(this->_idx, detail_len));
            }

            element.Text() = this->_str.substr(this->_idx, pos - this->_idx);
            this->_idx = pos;
            // 有可能直接碰上结束符，所以需要 continue，让 element 里面的逻辑进行判断
            continue;
        }

        // 对应三种情况：结束符，注释，下个节点
        if (token == '<')
        {
            // 结束符
            if (this->_str[this->_idx + 1] == '/')
            {
                // 重复的节点名称
                if (this->_str.compare(this->_idx + 2, element.Name().size(), element.Name()) != 0)
                {
                    THROW_ERROR("parse end tag error", this->_str.substr(this->_idx, detail_len));
                }

                // 跨过 name 和 </
                this->_idx += 2 + element.Name().size();

                char x = _get_next_token();
                if (x != '>')
                {
                    THROW_ERROR("parse end tag error", this->_str.substr(this->_idx, detail_len));
                }

                // 解析完成，过滤掉 >
                this->_idx++;
                return element;
            }

            // 注释
            if (this->_idx + 3 < this->_str.size() && this->_str.compare(this->_idx, 4, "<!--") == 0)
            {
                if (!this->_parse_comment())
                {
                    THROW_ERROR("parse comment error", this->_str.substr(this->_idx, detail_len));
                }
                continue;
            }

            // 子节点
            element.push_back(this->parse());
            continue;
        }

        // 解析属性
        auto key = this->_parse_attr_key();
        auto x = this->_get_next_token();
        if (x != '=')
        {
            THROW_ERROR("parse attrs error", this->_str.substr(this->_idx, detail_len));
        }

        this->_idx++;
        auto value = this->_parse_attr_value();
        element[key] = value;
    }

    THROW_ERROR("parse element error", this->_str.substr(this->_idx, detail_len));
}

string xml::Parser::_parse_attr_key()
{
    char token = this->_get_next_token();
    auto start_pos = this->_idx;

    if (isalpha(token) || token == '_')
    {
        while (this->_idx < this->_str.size() && (isalpha(this->_str[this->_idx]) || isdigit(this->_str[this->_idx]) || this->_str[this->_idx] == '_' || this->_str[this->_idx] == '-' || this->_str[this->_idx] == ':') && this->_str[this->_idx] != '=')
        {
            this->_idx++;
        }

        return this->_str.substr(start_pos, this->_idx - start_pos);
    }

    THROW_ERROR("parse attr key error", this->_str.substr(this->_idx, detail_len));
}

string xml::Parser::_parse_attr_value()
{
    char token = this->_get_next_token();
    if (token == '"')
    {
        auto start_pos = ++this->_idx;
        auto pos = this->_str.find('"', this->_idx);

        if (pos == string::npos)
        {
            THROW_ERROR("parse attr value error", this->_str.substr(this->_idx, detail_len));
        }

        // 跳过 "
        this->_idx = pos + 1;
        return this->_str.substr(start_pos, pos - start_pos);
    }

    THROW_ERROR("parse attr value error", this->_str.substr(this->_idx, detail_len));
}
