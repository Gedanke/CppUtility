#ifndef MYUTIL_PARSER_H
#define MYUTIL_PARSER_H

#include <string>
#include <string_view>
#include <sstream>

namespace json
{
#define FUNC_TO_NAME _to_json
#define FUNC_FROM_NAME _from_json

#define START_TO_JSONvoid            \
    FUNC_TO_NAME(json::JObject &obj) \
    const                            \
    {
#define to(key) obj[key]
// push 一个自定义类型的成员
#define to_struct(key, struct_member)    \
    json::JObject tmp((json::dict_t())); \
    struct_member.FUNC_TO_NAME(tmp);     \
    obj[key] = tmp
#define END_TO_JSON }

#define START_FROM_JSON                     \
    void FUNC_FROM_NAME(json::JObejct &obj) \
    {
#define from(key, type) obj[key].Value<type>()
#define from_struct(key, struct_member) struct_member.FUNC_FROM_NAME(obj[key])
#define END_FROM_JSON }

    using std::string;
    using std::string_view;
    using std::stringstream;

    class JObject;

    class Parser
    {
    private:
        string m_str;
        size_t m_idx{};

    public:
        Parser() = default;

        static JObject fromString(string_view content);

        template<class T>
        static string toJson(T const& src){
            // 基本类型
            if constexpr(IS_TYPE(T,int_t)){
                JObject object(src);
                return object.to_string();
            }
        }
    };
}

#endif // MYUTIL_PARSER_H