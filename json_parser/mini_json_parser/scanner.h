#pragma once

#include <iostream>
#include <string>

namespace civitasv
{
    namespace json
    {
        class Scanner
        {
        public:
            // 构造函数
            Scanner(std::string source) : source_(source), current_(0) {}

            enum class JsonTokenType
            {
                // {
                BEGAIN_OBJECT,
                // }
                END_OBJECT,

                // ,
                VALUE_SEPARATOR,
                // :
                NMAE_SEPARATOR,

                // string
                VALUE_STRING,
                // 1, 2, 3.4
                VALUE_NUMBER,

                // true
                LITERAL_TRUE,
                // false
                LITERAL_FALSE,
                // null
                LITERAL_NULL,

                // [
                BEGAIN_ARRAY,
                // ]
                END_ARRAY,

                // EOF
                END_OF_SOURCE
            };

            // 重载输出运算符
            friend std::ostream &operator<<(std::ostream &os, const JsonTokenType &type)
            {
                switch (type)
                {
                case Scanner::JsonTokenType::BEGAIN_ARRAY:
                {
                    os << "[";
                    break;
                }
                case Scanner::JsonTokenType::END_ARRAY:
                {
                    os << "]";
                    break;
                }
                case Scanner::JsonTokenType::BEGAIN_OBJECT:
                {
                    os << "{";
                    break;
                }
                case Scanner::JsonTokenType::END_OBJECT:
                {
                    os << "}";
                    break;
                }
                case Scanner::JsonTokenType::NMAE_SEPARATOR:
                {
                    os << ":";
                    break;
                }
                case Scanner::JsonTokenType::VALUE_SEPARATOR:
                {
                    os << ",";
                    break;
                }
                case Scanner::JsonTokenType::VALUE_NUMBER:
                {
                    os << "number";
                    break;
                }
                case Scanner::JsonTokenType::VALUE_STRING:
                {
                    os << "string";
                    break;
                }
                case Scanner::JsonTokenType::LITERAL_TRUE:
                {
                    os << "true";
                    break;
                }
                case Scanner::JsonTokenType::LITERAL_FALSE:
                {
                    os << "false";
                    break;
                }
                case Scanner::JsonTokenType::LITERAL_NULL:
                {
                    os << "null";
                    break;
                }
                case Scanner::JsonTokenType::END_OF_SOURCE:
                {
                    os << "EOF";
                    break;
                }
                default:
                {
                    break;
                }
                }

                return os;
            }

            // 获取下一个 token 类型
            // 如果是 string 或 number，则赋值给 value_string_ 或 value_number_
            JsonTokenType scan();

            // 回滚到上一个 token
            void rollback();

            // 获取 value_number_
            float getNumberValue() { return this->value_number_; }

            // 获取
            std::string getStringValue() { return this->value_string_; }

        private:
            // json source
            std::string source_;
            // 当前正在处理的索引
            size_t current_;
            // 保存字符串的值
            std::string value_string_;
            // 保存数字的值
            float value_number_;
            // 前一个字符索引
            size_t prev_pos_;
            // 当前行
            size_t line_;

        private:
            // 是否到达结尾
            bool isAtEnd();

            // 移动，并返回下一个字符
            char advance();

            // 扫描判断是否是 true
            void scanTrue();

            // 扫描判断是否是 false
            void scanFalse();

            // 扫描判断是否是 null
            void scanNull();

            // 扫描判断是否是 number 类型
            void scanNumber();

            // 扫描判断是否是 string 类型
            void scanString();

            // 判断是否是数字
            bool isDigit(char c);

            // 当前字符
            char peek();

            // 返回下一个字符，但不移动
            char peekNext();
        };
    }
}
