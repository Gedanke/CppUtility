#include "scanner.h"
#include "error.h"

namespace civitasv
{
    namespace json
    {
        // 判断是否到字符串尾部
        bool Scanner::isAtEnd()
        {
            return current_ >= this->source_.size();
        }

        // 下一个字符
        char Scanner::advance()
        {
            return this->source_[this->current_++];
        }

        // 扫描判断是否是 true
        void Scanner::scanTrue()
        {
            if (this->source_.compare(this->current_, 3, "rue") == 0)
            {
                this->current_ += 3;
            }
            else
            {
                error("Scan 'true' error");
            }
        }

        // 扫描判断是否是 false
        void Scanner::scanFalse()
        {
            if (this->source_.compare(this->current_, 4, "alse") == 0)
            {
                this->current_ += 4;
            }
            else
            {
                error("Scan 'false' error");
            }
        }

        // 扫描判断是否是 null
        void Scanner::scanNull()
        {
            if (this->source_.compare(this->current_, 3, "ull") == 0)
            {
                this->current_ += 3;
            }
            else
            {
                error("Scan 'null' error");
            }
        }

        // 扫描判断是否是 number 类型
        void Scanner::scanNumber()
        {
            // 当前位置
            size_t pos = this->current_ - 1;

            // 负数
            while (this->isDigit(this->peek()))
            {
                this->advance();
            }

            // 含有小数点
            if (this->peek() == '.' && this->isDigit(this->peekNext()))
            {
                // 移动，跳过小数点
                this->advance();

                // 小数点后的数字
                while (this->isDigit(this->peek()))
                {
                    this->advance();
                }
            }

            // 保存数字
            this->value_number_ = std::atof(this->source_.substr(pos, this->current_ - pos).c_str());
        }

        // 扫描判断是否是 string 类型
        void Scanner::scanString()
        {
            size_t pos = this->current_;

            // 字符串起始
            while (this->peek() != '\"' && !this->isAtEnd())
            {
                this->advance();

                // 转义字符
                if (this->peek() == '\\')
                {
                    // 跳过转移 `\`
                    this->advance();

                    // `\` 后的字符
                    switch (this->peek())
                    {
                    case 'b':
                    case 't':
                    case 'n':
                    case 'f':
                    case 'r':
                    case '\"':
                    case '\\':
                    {
                        this->advance();
                        break;
                    }
                    case 'u':
                    {
                        // unicode 字符长度为 4
                        for (int i = 0; i < 4; i++)
                        {
                            this->advance();
                        }
                        break;
                    }
                    default:
                    {
                        break;
                    }
                    }
                }
            }

            // 缺少字符串结束的 "
            if (this->isAtEnd())
            {
                error("invalid string: missing closing quote");
            }

            this->advance();

            // 保留字符串结果
            this->value_string_ = this->source_.substr(pos, this->current_ - pos - 1);
        }

        // 判断是否是数字
        bool Scanner::isDigit(char c)
        {
            return c >= '0' && c <= '9';
        }

        // 当前字符
        char Scanner::peek()
        {
            if (this->isAtEnd())
            {
                return '\0';
            }
            else
            {
                return this->source_[this->current_];
            }
        }

        // 返回下一个字符，但不移动
        char Scanner::peekNext()
        {
            if (this->current_ + 1 >= this->source_.size())
            {
                return '\0';
            }
            else
            {
                return this->source_[this->current_ + 1];
            }
        }

        // 扫描字符串，返回对应的 json 内容类型
        Scanner::JsonTokenType Scanner::scan()
        {
            // 是否扫描完毕
            if (isAtEnd())
            {
                return JsonTokenType::END_OF_SOURCE;
            }

            // 获取下一个字符
            char c = advance();
            // 判断该字符

            switch (c)
            {
            case '[':
            {
                return JsonTokenType::BEGAIN_ARRAY;
            }
            case ']':
            {
                return JsonTokenType::END_ARRAY;
            }
            case '{':
            {
                return JsonTokenType::BEGAIN_OBJECT;
            }
            case '}':
            {
                return JsonTokenType::END_OBJECT;
            }
            case ',':
            {
                return JsonTokenType::VALUE_SEPARATOR;
            }
            case ':':
            {
                return JsonTokenType::NMAE_SEPARATOR;
            }
            case 't':
            {
                scanTrue();
                return JsonTokenType::LITERAL_TRUE;
            }
            case 'f':
            {
                scanFalse();
                return JsonTokenType::LITERAL_FALSE;
            }
            case 'n':
            {
                scanNull();
                return JsonTokenType::LITERAL_NULL;
            }
            case '-':
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
            {
                scanNumber();
                return JsonTokenType::VALUE_NUMBER;
            }
            case '\"':
            {
                scanString();
                return JsonTokenType::VALUE_STRING;
            }
            case ' ':
            case '\r':
            case '\n':
            case '\t':
            {
                return scan();
            }
            default:
            {
                // error
                std::string message = "Unsupported Token: ";
                message += c;
                error(std::string(message));

                return JsonTokenType::END_OF_SOURCE;
            }
            }
        }

        // 回滚
        void Scanner::rollback()
        {
            this->current_ = this->prev_pos_;
        }
    }
}
