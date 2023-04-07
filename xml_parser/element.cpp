#include "element.h"
#include <sstream>

using namespace xml;
using std::ostringstream;

string Element::_to_string()
{
    if (this->_name == "")
    {
        return "";
    }

    std::ostringstream os;
    os << "<" << this->_name;

    // 遍历属性字典
    for (auto &&[key, value] : this->_attributes)
    {
        os << " " << key << "=\"" << value << "\"";
    }

    // 判断是否为单元素
    if (this->_text.empty() && this->_children.empty())
    {
        os << "/>";
        return os.str();
    }
    os << ">";
    os << this->_text;

    // 子节点
    if (!this->_children.empty())
    {
        // 递归所有子节点
        for (auto &&child : this->_children)
        {
            os << child._to_string();
        }
    }

    os << "</" << this->_name << ">";

    return os.str();
}
