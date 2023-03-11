# readme

`ini` 文件格式

* `section`: `[section]`
* `key-value`: `key=value`
* `comment`: `#comment`

示例：

```cpp
int main()
{
    IniFile f;
    // 加载配置文件
    f.Load("../test_resources/test.ini");
    // 打印结果
    f.Show();
    // 保存结果到文件
    f.Save("../test_resources/pp.ini");

    // 根据section和key value
    string ip = f["redis"]["host"];
    int port = f["redis"]["port"];
    std::cout << "redis ip:" << ip << "\n";
    std::cout << "redis port:" << port << "\n";

    return 0;
}
```

结果：

```shell
~$ g++ -o main main.cpp ini.cpp -std=gnu++17 && ./main 
[mysql]
host=1.23.23
port=3306

[redis]
host=127.0.0.1
port=1200
thread_num=31

redis ip:127.0.0.1
redis port:1200
```

---
