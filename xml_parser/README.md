# README

---

## xml 格式简单介绍


```xml
<?xml version="1.0"?>
<!--这是注释-->
<workflow>
    <work name="1" switch="on">
        <plugin name="echoplugin.so" switch="on"/>
    </work>
</workflow>
```

xml 格式和 html 格式十分类似，一般用于存储需要属性的配置或者需要多个嵌套关系的配置

xml 一般使用于项目的配置文件，相比于其他的 ini 格式或者 yaml 格式，它的优势在于可以将一个标签拥有多个属性，比如上述 xml 文件格式是用于配置工作流的，其中有 name 属性和 switch 属性，且在 work 标签中又嵌套了 plugin 标签，相比较其他配置文件格式是要灵活很多的

具体的应用场景有很多，比如使用过 Java 中 Mybatis 的同学应该清楚，Mybatis 的配置文件就是 xml 格式，而且也可以通过 xml 格式进行 sql 语句的编写，同样 Java 的 maven 项目的配置文件也是采用的 xml 文件进行配置

---

## xml 格式解析过程浅析

同样回到之前的那段代码，实际上已经把 xml 文件格式的不同情况都列出来了

从整体上看，所有的 xml 标签分为：

* xml 声明(包含版本、编码等信息)
* 注释
* xml 元素：
    * 单标签元素
    * 成对标签元素

其中 xml 声明和注释都是非必须的。 而 xml 元素，至少需要一个成对标签元素，而且在最外层有且只能有一个，它作为根元素

从 xml 元素来看，分为：

* 名称
* 属性
* 内容
* 子节点

根据之前的例子，很明显，名称是必须要有的而且是唯一的，其他内容则是可选

根据元素的结束形式，我们把它们分为单标签和双标签元素

---

## 代码实现

实现存储解析数据的类—— Element

代码如下：

```h
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
```

上述代码，我们主要看成员变量

* 用 string 类型表示元素的 name 和 text
* 用 vector 嵌套表示孩子节点
* 用 map 表示 key-value 对的属性

其余的方法要么是 Getter/Setter，要么是方便操作孩子节点和属性。 当然还有一个 to_string() 方法这个待会讲

### 关键代码一：实现整体的解析

关于整体结构分解为下面的情形：

得到字符 token '<'

* `<?xml` 对应声明
* `<!--` 对应注释
* 后面有 name 的字符则对应元素

代码如下：

```cpp
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
```

上述代码我们用 while 循环进行嵌套的原因在于注释可能有多个

### 关键代码二：解析所有元素

解析元素--解析 name

* token 为 `/` 表示单标签元素结束符
* token 为 `<`
    * 如为 `</` 表示双标签结束符
    * 如为 `<!--` 表示注释
    * 其余情况则是遇到了下个子元素的起始位置
* token 为 `>`
    * 若下个 token 为 `<`，则继续下一次循环解析
    * 若下个 token 为其他，则表示当前标签的 text
* 其余情况为属性的解析

对应代码

```cpp
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
```

---

## 开发技巧

无论是 C++ 开发，还是各种其他语言的造轮子，在这个造轮子的过程中，不可能是一帆风顺的，需要不断的 debug，然后再测试，然后再 debug

实际上这类格式的解析，单纯的进行程序的调试效率是非常低下的。特别是你用的语言还是 C++，那么如果出现意外宕机行为，debug 几乎是不可能简单的找出原因的，所以为了方便调试，或者是意外宕机行为，我们还是多做一些错误、异常处理的工作比较好

比如上述的代码中，我们大量的用到了 THROW_ERROR 这个宏，实际上这个宏输出的内容是有便于调试和快速定位的。 具体代码如下：

```cpp
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
```

它会打印出了两个非常关键的信息：

* 内部的 C++ 代码解析抛出异常的位置
* 解析发生错误的字符串

---

## 有关 C++ 的优化

众所周知在 C++ 中，一个类有八个默认函数：

* 默认构造函数
* 默认拷贝构造函数
* 默认析构函数
* 默认重载赋值运算符函数
* 默认重载取址运算符函数
* 默认重载取址运算符 const 函数
* 默认移动构造函数(C++11)
* 默认重载移动赋值操作符函数(C++11)

我们一般情况需要注意的构造函数和赋值函数函数需要的是以下三类：

* 拷贝构造
* 移动构造
* 析构函数

以下面的代码为例来说明默认的行为：

```cpp
class Data{
    // ...
}
class test{
pvivate:
    Data m_data;
}
```

### 额外注意

默认情况的模拟

```cpp
class Data{
    ...
}
class test{
public:
    // 拷贝构造
    test(test const&src) = default;// 等价于下面的代码
    // test(test const& src):m_data(src.m_data){}
    // 移动构造
    test(test &&src) = default;// 等价于下面代码
    // tset(test&& src):m_data(std::move(src.m_data)){}
pvivate:
    Data m_data;
}
```

从上述情况可以看出，如果一个类的数据成员中含有原始指针数据，那么拷贝构造和移动构造都需要自定义，如果成员中全都用的标准库里的东西，那么我们就用默认的就行，因为标准库的所有成员都自己实现了拷贝和移动构造！比如我们目前的Element就全都用默认的就好

需要特别注意的点

* 显式定义了某个构造函数或者赋值函数，那么相应的另一个构造或者赋值就会被删除默认，需要再次显式定义了。 举个例子：比如我显式定义了移动构造(关于显式定义，手动创建算显式，手动写 default 也算显示)，那么就会造成所有的默认拷贝(拷贝构造和拷贝赋值)被删除。相反显式定义了移动赋值也是类似的，默认的拷贝行为被删除。拷贝的对于显式的默认行为处理也是一模一样
* 如果想要使用默认的构造/赋值函数，那么对应的成员也都必须支持。 例如以下代码：

```cpp
class Data{
    // ...
}
class test{
pvivate:
    Data m_data;
}
```

由于 test 类没有写任何构造函数，那么这 8 个默认构造函数按理来说都是有的，但如果 Data 类中的拷贝构造由于某些显式定义情况而被删除了，那么 test 类就不再支持拷贝构造(对我们造成的影响就是：没法再直接通过等号初始化)

最后，通过上述规则我们发现，如果想要通过默认的构造函数偷懒，那么首先你的成员得支持对应的构造函数，还有就是不要画蛇添足：比如本来我什么都不用写，它自动生成8大默认函数，然后你手写了一个拷贝构造 =default，好了，你的默认函数从此少了两个，又得你一个个手动去补了

故如果成员变量对移动和拷贝行为都是支持的，那么就千万不要再画蛇添足了，除非你需要自定义某些特殊行为(比如打日志什么的)。如果你的成员变量中含有原始指针，那么一定需要手动写好移动和拷贝行为。如果成员变量对拷贝和移动行为部分支持，那么根据你的使用情况来进行选择是否需要手动补充这些行为(后面的线程池会用到这个结论

---
