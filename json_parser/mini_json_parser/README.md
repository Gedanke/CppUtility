# README

[参考链接](https://github.com/Civitasv/mini-json-parser)

* 实现 Scanner，用于词法分析，扫描 json 字符串，获取 TokenType
* 实现 Parser，用于语法分析，根据 json spec 中描述的 json 语法格式，对 Scanner 生成的 Tokens 进行解析，从而解析为 C++ 中的 `map` 和 `vector`

第二阶段很类似于抽象语法树(AST)的生成。可以理解为 AST 是对 Tokens 分析后的另一种输出格式

编译器在编译时，几乎也是按照上述步骤，首先词法分析，但不同于 json 解析器的是，在语法分析阶段，则是将源语言转化为目标语言的格式

---
