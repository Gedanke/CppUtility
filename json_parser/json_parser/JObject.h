#ifndef MYUTIL_JOBJECT_H
#define MYUTIL_JOBJECT_H

#include <stdexcept>
#include <utility>
#include <variant>
#include <map>
#include <vector>
#include <string>
#include <string_view>
#include <sstream>

namespace json
{
    using std::get_if;
    using std::map;
    using std::string;
    using std::string_view;
    using std::stringstream;
    using std::variant;
    using std::vector;

    // 枚举 json 类型
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

    class JObject;

    using null_t = string;
    using int_t = int64_t;
    using bool_t = bool;
    using double_t = double;
    using str_t = string;
    using list_t = vector<JObject>;
    using dict_t = map<string, JObject>;

    // std::is_same 判断模板的类型，std::decay 把类型退化为基本形态
#define IS_TYPE(type_a, type_b) std::is_same<type_a, type_b>::value

    // 将变量声明为 constexpr 类型让编译器来验证变量的值是否是一个常量表达式
    template <class T>
    constexpr bool is_basic_type()
    {
        if constexpr (IS_TYPE(T, str_t) || IS_TYPE(T, bool_t) || IS_TYPE(T, double_t) || IS_TYPE(T, int_t))
        {
            return true;
        }

        return false;
    }

    class JObject
    {
    private:
        using value_t = variant<bool_t, int_t, double_t, str_t, list_t, dict_t>;
        // 根据类型获取值的地址，直接硬转为 void* 类型，然后外界调用 Value 函数进行类型的强转
        void *value();

    private:
        TYPE m_type;
        value_t m_value;

    public:
        JObject()
        {
            // 默认为 null 类型
            this->Null();
        }

        JObject(int_t value)
        {
            Int(value);
        }

        JObject(bool_t value)
        {
            Bool(value);
        }

        JObject(double_t value)
        {
            Double(value);
        }

        JObject(str_t const &value)
        {
            Str(value);
        }

        JObject(list_t value)
        {
            List(std::move(value));
        }

        JObject(dict_t value)
        {
            Dict(std::move(value));
        }

        void Null()
        {
            this->m_value = "null";
            this->m_type = T_NULL;
        }

        void Int(int_t value)
        {
            this->m_value = value;
            this->m_type = T_INT;
        }

        void Bool(bool_t value)
        {
            this->m_value = value;
            this->m_type = T_BOOL;
        }

        void Double(double_t value)
        {
            this->m_value = value;
            this->m_type = T_DOUBLE;
        }

        void Str(str_t value)
        {
            this->m_value = value;
            this->m_type = T_STR;
        }

        void List(list_t value)
        {
            this->m_value = value;
            this->m_type = T_LIST;
        }

        void Dict(dict_t value)
        {
            this->m_value = value;
            this->m_type = T_DICT;
        }

        operator string()
        {
            return Value<string>();
        }

        operator int()
        {
            return Value<int>();
        }

        operator bool()
        {
            return Value<bool>();
        }

        operator double()
        {
            return Value<double>();
        }

#define THROW_GET_ERROR(erron) throw std::logic_error("type error in get " #erron " value!")

        template <class V>
        V &Value()
        {
            // 安全类型检查
            if constexpr (IS_TYPE(V, str_t))
            {
                if (this->m_type != T_STR)
                {
                    THROW_GET_ERROR(string);
                }
            }
            else if constexpr (IS_TYPE(V, bool_t))
            {
                if (this->m_type != T_BOOL)
                {
                    THROW_GET_ERROR(BOOL);
                }
            }
            else if constexpr (IS_TYPE(V, int_t))
            {
                if (this->m_type != T_INT)
                {
                    THROW_GET_ERROR(INT);
                }
            }
            else if constexpr (IS_TYPE(V, double_t))
            {
                if (this->m_type != T_DOUBLE)
                {
                    THROW_GET_ERROR(DOUBLE);
                }
            }
            else if constexpr (IS_TYPE(V, list_t))
            {
                if (this->m_type != T_LIST)
                {
                    THROW_GET_ERROR(LIST);
                }
            }
            else if constexpr (IS_TYPE(V, dict_t))
            {
                if (this->m_type != T_DICT)
                {
                    THROW_GET_ERROR(DICT);
                }
            }
            else
            {
            }

            void *v = value();
            if (v == nullptr)
            {
                throw std::logic_error("unknown type in JObject::Value()");
            }

            return *((V *)v);
        }

        TYPE Type()
        {
            return this->m_type;
        }

        string to_string();

        void push_back(JObject item)
        {
            // 判断是否是 list 类型
            if (this->m_type == T_LIST)
            {
                auto &list = Value<list_t>();
                list.push_back(std::move(item));
                return;
            }
            throw std::logic_error("not a list type! JObject::push_back()");
        }

        void pop_back()
        {
            // 判断是否是 list 类型
            if (this->m_type == T_LIST)
            {
                auto &list = Value<list_t>();
                list.pop_back();
                return;
            }
            throw std::logic_error("not list type! JObject::pop_back()");
        }

        JObject &operator[](string const &key)
        {
            // 判断是否是 dict 类型
            if (this->m_type == T_DICT)
            {
                auto &dict = Value<dict_t>();
                return dict[key];
            }
            throw std::logic_error("not dict type! JObject::opertor[]()");
        }
    };

}

#endif // MYUTIL_JOBJECT_H