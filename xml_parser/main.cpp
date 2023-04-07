#include <iostream>
#include <fstream>
#include <string>
#include "parser.h"
#include "../benchmark/timer.hpp"

using namespace std;

void test_xml_parse()
{
    ifstream ifs("../test_resources/test.xml");
    if (!ifs)
    {
        std::cerr << "ifs read error";
        exit(1);
    }

    string text((istreambuf_iterator<char>(ifs)), istreambuf_iterator<char>());
    xml::Element element;
    {
        Timer t;
        element = std::move(xml::Parser::fromString(text));
    }
    ofstream ofs("../test_resources/test_out.xml");
    ofs << element.to_string();
}

int main()
{
    // g++ -o main main.cpp element.cpp parser.cpp -std=c++17 && ./main

    test_xml_parse();

    return 0;
}