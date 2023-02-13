#include <iostream>
#include "parser.h"
#include "scanner.h"

using namespace civitasv::json;

void test_scanner();
void test_parser();

int main()
{
    // g++ -o main test.cpp scanner.cpp parser.cpp -std=c++17 && ./main
    test_scanner();
    std::cout << "======================>\n";
    test_parser();

    return 0;
}

void test_scanner()
{
    auto source = R"(
    {
        "glossary" : 
        {
            "test": true,
            "hello": null,
            "hello2": "miao\"miao"
        }
    }
    )";

    auto source2 = "[1, 2, 3, 4, 5, \"Hello\"]";

    Scanner scanner(source);
    Scanner::JsonTokenType type;
    while ((type = scanner.scan()) != Scanner::JsonTokenType::END_OF_SOURCE)
    {
        std::cout << "Type: " << type;
        if (type == Scanner::JsonTokenType::VALUE_NUMBER)
        {
            std::cout << " Value: " << scanner.getNumberValue();
        }
        else if (type == Scanner::JsonTokenType::VALUE_STRING)
        {
            std::cout << " Value: " << scanner.getStringValue();
        }
        std::cout << '\n';
    }
}

void test_parser()
{
    auto source = R"(
      {
      "glossary" : {
        "test": true,
        "hello": null,
        "hello2": "miao\"miao"
      }
      }
  )";

    auto source2 = "[1, 2, 3, 4, 5, \"Hello\"]";

    Scanner scanner(source2);
    Scanner::JsonTokenType type;
    while ((type = scanner.scan()) != Scanner::JsonTokenType::END_OF_SOURCE)
    {
        std::cout << "Type: " << type;
        if (type == Scanner::JsonTokenType::VALUE_NUMBER)
        {
            std::cout << " Value: " << scanner.getNumberValue();
        }
        else if (type == Scanner::JsonTokenType::VALUE_STRING)
        {
            std::cout << " Value: " << scanner.getStringValue();
        }
        std::cout << '\n';
    }
}
