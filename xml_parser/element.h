#ifndef MYUTIL_ELEMENT_H
#define MYUTIL_ELEMENT_H

#include <map>
#include <vector>
#include <string>
#include <string_view>

namespace xml
{
    using std::map;
    using std::string;
    using std::string_view;
    using std::vector;

    // Element 类
    class Element
    {
    public:
        // 子节点
        using children = vector<Element>;
        // 属性键值对
        using attributes = map<string, string>;
        // 迭代器
        using iterator = vector<Element>::iterator;
        // 常量迭代器
        using const_iterator = vector<Element>::const_iterator;

        string &Name()
        {
            return this->_name;
        }

        string &Text()
        {
            return this->_text;
        }

        // 遍历子节点
        iterator begin()
        {
            return this->_children.begin();
        }

        [[nodiscard]] const_iterator begin() const
        {
            return this->_children.begin();
        }

        iterator end()
        {
            return this->_children.end();
        }

        [[nodiscard]] const_iterator end() const
        {
            return this->_children.end();
        }

        // 存入子节点
        void push_back(Element const &element)
        {
            this->_children.push_back(element);
        }

        void push_back(Element &&element)
        {
            this->_children.push_back(std::move(element));
        }

        // key-value 的存取
        string &operator[](string const &key)
        {
            return this->_attributes[key];
        }

        string to_string()
        {
            return this->_to_string();
        }

    private:
        string _to_string();

    private:
        // 节点名称
        string _name;
        // 节点属性
        string _text;
        // 子节点列表
        children _children;
        // 属性字典
        attributes _attributes;
    };

}

#endif // MYUTIL_ELEMENT_H
