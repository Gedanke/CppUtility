#include "parser.h"
#include "error.h"

namespace civitasv
{
    namespace json
    {
        using Type = JsonElement::Type;

        // parse scaner to JsonElement
        JsonElement *Parser::parse()
        {
            // 返回的 JsonElement
            JsonElement *element = new JsonElement();
            // token 类型
            JsonTokenType token_type_ = this->scanner_.scan();

            if (token_type_ != JsonTokenType::END_OF_SOURCE)
            {
                // 判断并解析对应的 token
                switch (token_type_)
                {
                case JsonTokenType::BEGAIN_OBJECT:
                {
                    JsonObject *object = this->parseObject();
                    element->value(object);
                    break;
                }
                case JsonTokenType::BEGAIN_ARRAY:
                {
                    JsonArray *array = this->parseArray();
                    element->value(array);
                    break;
                }
                case JsonTokenType::VALUE_STRING:
                {
                    std::string *val = new std::string(this->scanner_.getStringValue());
                    element->value(val);
                    break;
                }
                case JsonTokenType::VALUE_NUMBER:
                {
                    float number = this->scanner_.getNumberValue();
                    element->value(number);
                    break;
                }
                case JsonTokenType::LITERAL_TRUE:
                {
                    element->value(true);
                    break;
                }
                case JsonTokenType::LITERAL_FALSE:
                {

                    element->value(false);
                    break;
                }
                case JsonTokenType::LITERAL_NULL:
                {
                    break;
                }
                default:
                {
                    break;
                }
                }
            }

            return element;
        }

        // 返回 JsonObject
        JsonObject *Parser::parseObject()
        {
            JsonObject *res = new JsonObject();

            // 判断下一个 token 是不是对象结束标志
            JsonTokenType next = this->scanner_.scan();
            if (next == JsonTokenType::END_OBJECT)
            {
                return res;
            }
            // 回滚到上一个 token 位置
            this->scanner_.rollback();

            while (true)
            {
                next = this->scanner_.scan();
                // 判断当前是否为字符串或者字典的键
                if (next != JsonTokenType::VALUE_STRING)
                {
                    error("Key must be string!");
                }
                std::string key = this->scanner_.getStringValue();

                next = this->scanner_.scan();
                // 判断当前是否是 :
                if (next != JsonTokenType::NMAE_SEPARATOR)
                {
                    error("Expected ':' in object!");
                }

                // 解析值的类型
                (*res)[key] = this->parse();

                next = this->scanner_.scan();
                // 判断是否到达字典结尾
                if (next == JsonTokenType::END_OBJECT)
                {
                    break;
                }
                // 没有到达字典结尾，判断当前是否是 ,
                if (next != JsonTokenType::VALUE_SEPARATOR)
                {
                    error("Expected ',' in object!");
                }
            }

            return res;
        }

        // 返回 JsonArray
        JsonArray *Parser::parseArray()
        {
            JsonArray *res = new JsonArray();

            // 判断下一个 token 是不是数组结束标志
            JsonTokenType next = this->scanner_.scan();
            if (next == JsonTokenType::END_ARRAY)
            {
                return res;
            }
            // 回滚
            this->scanner_.rollback();

            while (true)
            {
                res->push_back(this->parse());
                next = this->scanner_.scan();

                // 判断是否到达数组结尾
                if (next == JsonTokenType::END_ARRAY)
                {
                    break;
                }

                // 没有到达数组结尾，判断当前是否为 ,
                if (next != JsonTokenType::VALUE_SEPARATOR)
                {
                    error("Expected ',' in array!");
                }
            }

            return res;
        }
    }
}
