#include "any.hpp"
#include <iostream>
#include <vector>

using namespace std;
using namespace nonstd;

struct Student
{
    Student() = default;
    Student(int id, std::string name) : id_(id), name_(std::move(name)) {}
    int id_{};
    std::string name_;
};

void test_any()
{
    Any t = "你好";
    auto test_construct = [&]()
    {
        cout << t.type().name() << endl;
        t = 345456;
        cout << t.type().name() << endl;
        t = Student();
        cout << t.type().name() << endl;
        t = Any(Student());
        cout << t.type().name() << endl;
        t = std::move(Any("hello我k$"));
    };

    auto test_cast = [&]()
    {
        cout << any_cast<const char *>(t) << endl;
        t = std::to_string(43324324);
        cout << "typename:" << t.type().name() << " value: " << any_cast<std::string>(t) << endl;
    };

    auto test_other = [&]()
    {
        t.emplace<Student>(323, std::string("fdfsafsda"));
        cout << "id: " << any_cast<Student>(t).id_ << " name: " << any_cast<Student>(t).name_ << endl;
        t.reset();
        cout << t.type().name() << endl;
        Any p = "再次测试看看";
        swap(t, p);
        cout << "p: " << p.type().name() << ", t: " << t.type().name() << ", t_value: " << any_cast<const char *>(t) << endl;
        t = vector<std::string>{"fdsfasddfas", "fsaffafa", "fdsaadsf"};
        // cout << "type: " << t.type().name() << "t: " << any_cast<vector<std::string>>(t);
        // const 修饰的常量转变量出错
        // t = "你好";
        // cout << any_cast<char *>(t) << endl;
    };

    test_construct();
    test_cast();
    test_other();
}
int main()
{
    test_any();

    return 0;
}