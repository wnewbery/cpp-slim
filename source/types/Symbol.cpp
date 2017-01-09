#include "types/Symbol.hpp"
#include "types/Boolean.hpp"
#include "types/Number.hpp"
#include "types/Proc.hpp"
#include "types/String.hpp"
#include "Operators.hpp"
#include "Function.hpp"
#include "CachedMethod.hpp"
#include <unordered_map>
#include <mutex>

namespace slim
{
    namespace
    {
        class SymbolProc : public Proc
        {
        public:
            virtual Ptr<Object> call(const FunctionArgs &args)override
            {
                if (args.empty()) throw ArgumentError("no receiver given");
                auto self = args[0].get();
                auto method = cache.get(self, name);
                FunctionArgs args2(args.begin() + 1, args.end());
                return (*method)(self, args2);
            }
            Ptr<Symbol> name;
            CachedMethod cache;
            SymbolProc(Ptr<Symbol> name) : name(name) {}
        };
        class UnaryProc : public Proc
        {
        public:
            typedef Ptr<Object>(Object::*Op)();
            virtual Ptr<Object> call(const FunctionArgs &args)override
            {
                if (args.size() != 1) throw ArgumentCountError(args.size(), 1, 1);
                return (args[0].get()->*op)();
            }
            Op op;
            UnaryProc(Op op) : op(op) {}
        };
        template<class T> class UnaryProcT : public Proc
        {
        public:
            typedef T(Object::*Op)()const;
            virtual Ptr<Object> call(const FunctionArgs &args)override
            {
                if (args.size() != 1) throw ArgumentCountError(args.size(), 1, 1);
                return make_value((args[0].get()->*op)());
            }
            Op op;
            UnaryProcT(Op op) : op(op) {}
        };
        class BinaryProc : public Proc
        {
        public:
            typedef Ptr<Object>(Object::*Op)(Object *rhs);
            virtual Ptr<Object> call(const FunctionArgs &args)override
            {
                if (args.size() != 2) throw ArgumentCountError(args.size(), 2, 2);
                return (args[0].get()->*op)(args[1].get());
            }
            Op op;
            BinaryProc(Op op) : op(op) {}
        };
        template<class T> class BinaryProcT : public Proc
        {
        public:
            typedef T(Object::*Op)(const Object *rhs)const;
            virtual Ptr<Object> call(const FunctionArgs &args)override
            {
                if (args.size() != 2) throw ArgumentCountError(args.size(), 2, 2);
                return make_value((args[0].get()->*op)(args[1].get()));
            }
            Op op;
            BinaryProcT(Op op) : op(op) {}
        };
    }
    SymPtr symbol(const std::string & str)
    {
        static std::unordered_map<std::string, std::shared_ptr<Symbol>> map;
        static std::mutex mutex;
        std::unique_lock<std::mutex> lock(mutex);
        auto x = map.emplace(str, nullptr);
        if (x.second)
        {
            x.first->second.reset(new Symbol(make_value(str)));
        }

        return x.first->second;
    }
    SymPtr symbol(std::shared_ptr<String> str)
    {
        return symbol(str->get_value());
    }

    Symbol::Symbol(std::shared_ptr<String> str) : _str(str) {}
    Symbol::~Symbol() {}
    std::string Symbol::to_string() const
    {
        return _str->to_string();
    }
    std::string Symbol::inspect() const
    {
        return ":" + _str->to_string();
    }
    size_t Symbol::hash() const
    {
        return std::hash<ObjectPtr>()(_str);
    }
    int Symbol::cmp(const Object * rhs) const
    {
        return _str->cmp(coerce<Symbol>(rhs)->_str.get());
    }
    const std::string &Symbol::str()const
    {
        return _str->get_value();
    }
    const char *Symbol::c_str()const
    {
        return str().c_str();
    }
    Ptr<Proc> Symbol::to_proc()
    {
        auto this_ptr = std::dynamic_pointer_cast<Symbol>(shared_from_this());
        auto &str = _str->get_value();
        if (str.size() == 1)
        {
            switch (str[0])
            {
            case '!': return create_object<UnaryProcT<bool>>(&Object::is_false);
            case '~': return create_object<UnaryProc>(&Object::bit_not);
            case '*': return create_object<BinaryProc>(&Object::mul);
            case '/': return create_object<BinaryProc>(&Object::div);
            case '%': return create_object<BinaryProc>(&Object::mod);
            case '+': return create_object<BinaryProc>(&Object::add);
            case '-': return create_object<BinaryProc>(&Object::sub);
            case '&': return create_object<BinaryProc>(&Object::bit_and);
            case '|': return create_object<BinaryProc>(&Object::bit_or);
            case '^': return create_object<BinaryProc>(&Object::bit_xor);
            case '<': return create_object<BinaryProcT<bool>>(&Object::cmp_lt);
            case '>': return create_object<BinaryProcT<bool>>(&Object::cmp_gt);
            }
        }
        else if (str.size() == 2)
        {
            if      (str == "==") return create_object<BinaryProcT<bool>>(&Object::eq);
            else if (str == "!=") return create_object<BinaryProcT<bool>>(&Object::ne);
            else if (str == "<=") return create_object<BinaryProcT<bool>>(&Object::cmp_le);
            else if (str == "<<") return create_object<BinaryProc>(&Object::bit_lshift);
            else if (str == ">=") return create_object<BinaryProcT<bool>>(&Object::cmp_ge);
            else if (str == ">>") return create_object<BinaryProc>(&Object::bit_rshift);
            else if (str == "+@") return create_object<UnaryProc>(&Object::unary_plus);
            else if (str == "-@") return create_object<UnaryProc>(&Object::negate);
        }
        else if (str == "<=>") return create_object<BinaryProcT<int>>(&Object::cmp);
        return create_object<SymbolProc>(this_ptr);
    }

    const MethodTable & Symbol::method_table() const
    {
        static const MethodTable table(Object::method_table(),
        {
            {&Symbol::to_proc, "to_proc"}
        });
        return table;
    }
}
