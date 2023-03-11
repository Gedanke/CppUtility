#include <iostream>
#include "ini.h"

int main()
{
    IniFile f;
    // 加载配置文件
    f.Load("../test_resources/test.ini");
    // 打印结果
    f.Show();
    // 保存结果到文件
    f.Save("../test_resources/pp.ini");

    // 根据 section 和key value
    string ip = f["redis"]["host"];
    int port = f["redis"]["port"];

    std::cout << "redis ip:" << ip << "\n";
    std::cout << "redis port:" << port << "\n";

    return 0;
}
