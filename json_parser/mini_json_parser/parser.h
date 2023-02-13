#pragma once

#include "scanner.h"
#include "jsonElement.h"

namespace civitasv
{
    namespace json
    {
        class Parser
        {
            using JsonTokenType = Scanner::JsonTokenType;

        public:
            Parser(Scanner scanner) : scanner_(scanner) {}

            // parse scaner to JsonElement
            JsonElement *parse();

        private:
            // 返回 JsonObject
            JsonObject *parseObject();

            // 返回 JsonArray
            JsonArray *parseArray();

        private:
            // 使用 scanner_ 可以接收一个字符串或者文件
            Scanner scanner_;
        };
    }
}
