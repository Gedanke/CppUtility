#pragma once

#include <string>
#include <map>
#include <sstream>
#include <vector>
#include "error.h"

namespace civitasv
{
    namespace json
    {
        class JsonElement;
        using JsonObject = std::map<std::string, JsonElement *>;
        using JsonArray = std::vector<JsonElement *>;

        class JsonElement
        {
        public:
            enum class Type
            {
                JSON_NULL,
                JSON_BOOL,
                JSON_NUMBER,
                JSON_STRING,
                JSON_ARRAY,
                JSON_OBJECT
            };

            union Value
            {
                JsonObject *value_object;
                JsonArray *value_array;
                std::string *value_string;
                float value_number;
                bool value_bool;
            };

            // 对应的构造方法
            JsonElement() : JsonElement(Type::JSON_NULL) {}

            //
            JsonElement(Type type) : type_(type)
            {
                switch (type)
                {
                case Type::JSON_OBJECT:
                {
                    this->value_.value_object = new std::map<std::string, JsonElement *>;
                    break;
                }
                case Type::JSON_ARRAY:
                {
                    this->value_.value_array = new std::vector<JsonElement *>();
                    break;
                }
                case Type::JSON_STRING:
                {
                    this->value_.value_string = new std::string("");
                    break;
                }
                case Type::JSON_NUMBER:
                {
                    this->value_.value_number = 0;
                    break;
                }
                case Type::JSON_BOOL:
                {
                    this->value_.value_bool = false;
                    break;
                }
                case Type::JSON_NULL:
                {
                    break;
                }
                default:
                {
                    break;
                }
                }
            }

            JsonElement(JsonObject *value_object) : type_(Type::JSON_OBJECT)
            {
                this->value(value_object);
            }

            JsonElement(JsonArray *value_array) : type_(Type::JSON_ARRAY)
            {
                this->value(value_array);
            }

            JsonElement(std::string *value_string) : type_(Type::JSON_STRING)
            {
                this->value(value_string);
            }

            JsonElement(float value_number) : type_(Type::JSON_NUMBER)
            {
                this->value(value_number);
            }

            JsonElement(bool value_bool) : type_(Type::JSON_BOOL)
            {
                this->value(value_bool);
            }

            // 析构函数
            ~JsonElement()
            {
                if (this->type_ == Type::JSON_OBJECT)
                {
                    JsonObject *object = this->value_.value_object;
                    for (auto &[key, value] : *object)
                    {
                        delete value;
                    }
                    delete object;
                }
                else if (this->type_ == Type::JSON_ARRAY)
                {
                    JsonArray *array = this->value_.value_array;
                    for (auto &item : *array)
                    {
                        delete item;
                    }
                    delete array;
                }
                else if (this->type_ == Type::JSON_STRING)
                {
                    std::string *val = this->value_.value_string;
                    delete val;
                }
            }

            Type type()
            {
                return this->type_;
            }

            // 获取 JsonObject，转换为对应的值并存储到 type_ 和 value_ 中
            void value(JsonObject *object)
            {
                this->type_ = Type::JSON_OBJECT;
                this->value_.value_object = object;
            }

            void value(JsonArray *array)
            {
                this->type_ = Type::JSON_ARRAY;
                this->value_.value_array = array;
            }

            void value(std::string *val)
            {
                this->type_ = Type::JSON_STRING;
                this->value_.value_string = val;
            }

            void value(float number)
            {
                this->type_ = Type::JSON_NUMBER;
                this->value_.value_number = number;
            }

            void value(bool value)
            {
                this->type_ = Type::JSON_BOOL;
                this->value_.value_bool = value;
            }

            // 向下转型的方法
            JsonObject *asObject()
            {
                if (this->type_ == Type::JSON_OBJECT)
                {
                    return this->value_.value_object;
                }
                else
                {
                    error("Type of JsonElement isn't JsonObject!");
                    return nullptr;
                }
            }

            JsonArray *asArray()
            {
                if (this->type_ == Type::JSON_ARRAY)
                {
                    return this->value_.value_array;
                }
                else
                {
                    error("Type of JsonElement isn't JsonArray!");
                    return nullptr;
                }
            }

            std::string *asString()
            {
                if (this->type_ == Type::JSON_STRING)
                {
                    return this->value_.value_string;
                }
                else
                {
                    error("Type of JsonElement isn't String!");
                    return nullptr;
                }
            }

            float asNumber()
            {
                if (this->type_ == Type::JSON_NUMBER)
                {
                    return this->value_.value_number;
                }
                else
                {
                    error("Type of JsonElement isn't Number!");
                    return 0.0f;
                }
            }

            bool asBool()
            {
                if (this->type_ == Type::JSON_BOOL)
                {
                    return this->value_.value_bool;
                }
                else
                {
                    error("Type of JsonElement isn't Boolean!");
                    return false;
                }
            }

            // 转成字符串
            std::string dumps()
            {
                std::stringstream ss;
                switch (this->type_)
                {
                case Type::JSON_OBJECT:
                {
                    ss << *(this->value_.value_object);
                    break;
                }
                case Type::JSON_ARRAY:
                {
                    ss << *(this->value_.value_array);
                    break;
                }
                case Type::JSON_STRING:
                {
                    ss << '\"' << *(this->value_.value_string) << '\"';
                    break;
                }
                case Type::JSON_NUMBER:
                {
                    ss << this->value_.value_number;
                    break;
                }
                case Type::JSON_BOOL:
                {
                    ss << (this->value_.value_bool ? "true" : "false");
                    break;
                }
                case Type::JSON_NULL:
                {
                    ss << "null";
                    break;
                }
                default:
                {
                    break;
                }
                }

                return ss.str();
            }

            // 重载 JsonObject
            friend std::ostream &operator<<(std::ostream &os, const JsonObject &object)
            {
                os << "{";
                for (auto iter = object.begin(); iter != object.end(); iter++)
                {
                    os << "\"" << iter->first << "\""
                       << ": " << iter->second->dumps();

                    // 输出 ,
                    if (iter != --object.end())
                    {
                        os << ",";
                    }
                }
                os << "}";

                return os;
            }

            // 重载 JsonArray
            friend std::ostream &operator<<(std::ostream &os, const JsonArray &array)
            {
                os << "[";
                for (size_t i = 0; i < array.size(); i++)
                {
                    os << array[i]->dumps();
                    if (i != array.size() - 1)
                    {
                        os << ",";
                    }
                }
                os << "]";

                return os;
            }

        private:
            Type type_;
            Value value_;
        };
    }
}
