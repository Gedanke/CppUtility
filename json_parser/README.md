# readme

项目地址 [https://github.com/Gedanke/CppUtility](https://github.com/Gedanke/CppUtility)

---

## json 格式

JSON 格式有以下基本类型：

* `null` 类型：值为 `null`，表示为空
* `bool` 类型：值为 `true` 和 `false`
* `number` 类型：值为 `int`、`double`，即整数或小数
* `string` 类型：形如 `"abc"`
* `list` 类型：也称 `array` 类型
* `dict` 类型：也称 `object` 类型

---

## 解析 json 字符串 

* 序列化过程：
    * Json 串通过 `fromString()` 转化为 `JObject` 进而转化为任意类型
* 反序列化过程：
    *  任意类型通过 `toString()` 转化为 `JObject` 进而转化为 Json 串

---

## 创建 JObject 类

我们需要把 json 的类型对应到计算机语言的类型。由于 json 的数据在我们看来都是字符串，那么有如下对应关系：

* `"null"` 对应我们构造的 `null` 类型
* `true`，`false` 对应内部的 `bool` 类型即可
* `number` 类型数据对应 `int`、`double` 类型
* `string` 类型数据对应 `string` 即可
* `list` 类型对应 C++ 中的 `vector`
* `dict` 类型对应 C++ 中的 `map` 或 `unordered_map`

在计算机语言中，需要构造一个对象类型，用于将以上类型全部涵盖。

在 C++ 中我们通过 `std::variant` 来进行，还需要一个枚举 `tag` 来表示当前对象内存储的数据类型

当然还可以通过一个 `void* + 申请堆内存` 来解决，然后再强转为对应类型来操作。对应的代码如下：

```h
enum TYPE
{
    T_NULL,
    T_BOOL,
    T_INT,
    T_DOUBLE,
    T_STR,
    T_LIST,
    T_DICT
};

using null_t = string;
using int_t = int32_t;
using bool_t = bool;
using double_t = double;
using str_t = string;
using list_t = vector<JObject>;
using dict_t = map<string, JObject>;

class JObject
{
public:
    using value_t = variant<bool_t, int_t, double_t, str_t, list_t, dict_t>;
    // edit
private:
    TYPE m_type;
    value_t m_value;
};
```




---
