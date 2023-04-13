#include <cstdio>
#include <memory>
#include <typeinfo>

namespace nonstd
{
    /*
        1. 能赋值任意类型 => 模板
        2. 需要内存安全 => 智能指针
        3. 类型信息 => type_id()
        类型擦除 => 虚函数的多态
     */

    class BadCast : public std::exception
    {
    public:
        /*
            C++11 新标准引入的 noexcept 运算符，可以用于指定某个函数不抛出异常。
            预先知道函数不会抛出异常有助于简化调用该函数的代码，而且编译器确认函数不会抛出异常，它就能执行某些特殊的优化操作。
        */
        auto what() const noexcept -> const char * override
        {
            return "bad_cast exception";
        }
    };

    // 用于标识该对象可安全复制
    class Copyable
    {
    protected:
        Copyable(const Copyable &) = default;
        Copyable &operator=(const Copyable &) = default;
        Copyable() = default;
        ~Copyable() = default;
    };

    class Any : public Copyable
    {
        struct TypeInternal
        {
            TypeInternal() = default;
            virtual ~TypeInternal() = default;
            virtual const std::type_info &get_type() const = 0;
            virtual std::unique_ptr<TypeInternal> clone() const = 0;
        };

        template <class T>
        struct TypeImpl : public TypeInternal
        {
            T data_;
            explicit TypeImpl(const T &data) : data_(data) {}
            explicit TypeImpl(T &&data) : data_(std::move(data)) {}

            template <class... Args>
            // forward 完美转发
            explicit TypeImpl(Args &&...args) : data_(std::forward<Args>(args)...)
            {
            }

            auto get_type() const -> const std::type_info & override
            {
                return typeid(T);
            }

            auto clone() const -> std::unique_ptr<TypeInternal> override
            {
                return std::unique_ptr<TypeImpl>(new TypeImpl(this->data_));
            }
        };

    public:
        Any() = default;
        ~Any() = default;

        Any(const Any &other)
        {
            if (other._data)
            {
                this->_data = other._data->clone();
            }
        }

        auto operator=(const Any &other) -> Any &
        {
            if (other._data)
            {
                this->_data = other._data->clone();
            }

            return *this;
        }

        Any(Any &&other) noexcept : _data(std::move(other._data)) {}

        auto operator=(Any &&other) noexcept -> Any &
        {
            this->_data.swap(other._data);
            return *this;
        }

        // 类型完全退化
        template <class T>
        using decay = typename std::decay<T>::type;

        template <class T, typename std::enable_if<!std::is_same<decay<T>, Any>::value, bool>::type = true>
        Any(T &&data) : _data(new TypeImpl<decay<T>>(std::forward<T>(data))) {}

        template <class T, typename std::enable_if<!std::is_same<decay<T>, Any>::value, bool>::type = true>
        auto operator=(T &&data) -> Any &
        {
            this->_data.reset(new TypeImpl<decay<T>>(std::forward<T>(data)));
            return *this;
        }

        // 是否被赋值
        auto has_value() const -> bool
        {
            return this->_data.operator bool();
        }

        // 获取类型
        auto type() const -> const std::type_info &
        {
            return this->has_value() ? this->_data->get_type() : typeid(void);
        }

        // 重置
        void reset() noexcept
        {
            this->_data.reset();
        }

        // 交换
        void swap(Any &other)
        {
            this->_data.swap(other._data);
        }

        template <class T, class... Args>
        void emplace(Args &&...args)
        {
            this->_data.reset(new TypeImpl<decay<T>>(std::forward<Args>(args)...));
        }

        template <class T>
        const T &cast() const
        {
            if (this->check_type<T>() && this->check_not_null())
            {
                return static_cast<const TypeImpl<T> *>(this->_data.get())->data_;
            }
            throw BadCast();
        }

        template <class T>
        T &cast()
        {
            if (this->check_type<T>() && this->check_not_null())
            {
                return static_cast<TypeImpl<T> *>(this->_data.get())->data_;
            }
            return BadCast();
        }

    private:
        template <class T>
        auto check_type() const -> bool
        {
            if (this->type().hash_code() != typeid(T).hash_code())
            {
                fprintf(stderr, "in file %s: %d\n%s => %s type cat error!\n", __FILE__, __LINE__, this->type().name(), typeid(T).name());
                return false;
            }
            return true;
        }

        auto check_not_null() const -> bool
        {
            if (!this->_data)
            {
                fprintf(stderr, "in file %s: %d\n nullpter data", __FILE__, __LINE__);
                return false;
            }
            return true;
        }

        std::unique_ptr<TypeInternal> _data;
    };

    inline void swap(Any &lhs, Any &rhs)
    {
        lhs.swap(rhs);
    }

    template <class T>
    auto any_cast(const Any &any) -> const T &
    {
        return any.template cast<T>();
    }

    template <class T>
    auto ant_cast(Any &any) -> T &
    {
        return any.template cast<T>();
    }
}