#include <iostream>
#include <fstream>
#include "JObject.h"
#include "Parser.h"
#include "../benchmark/timer.hpp"
#include "../magic_template/scienum.h"

using namespace std;

enum T
{
    MYSD,
    sf
};

struct Base
{
    int pp;
    string qq;

    // START_FROM JSON
};

struct Mytest
{
};

void test_class_serialization();
void test_string_parser();

int main()
{
    // g++ -o main main.cpp JObject.cpp Parser.cpp -I ..\benchmark -I ..\magic_template && ./main
    test_string_parser();

    return 0;
}

void test_class_serialization()
{
    cout << "test" << endl;
}

void test_string_parser()
{
    cout<<"test string to parser"<<endl;
    std::ifstream fin(R"(../test_resources/test.json)");
}
