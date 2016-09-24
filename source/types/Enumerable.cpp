#include "types/Enumerable.hpp"
#include "types/Enumerator.hpp"
#include "types/Array.hpp"
#include "types/Hash.hpp"
#include "types/Proc.hpp"
#include "Function.hpp"
#include <sstream>
#include <algorithm>
#include <deque>

namespace slim
{
    //Need to make an object that keeps "this" alive, but also can call a function not derived from Object

    template<class T>
    Ptr<Enumerator> make_enumerator(
        ObjectPtr save_self,
        T *forward_self,
        ObjectPtr(T::*func)(const FunctionArgs &args),
        const char *name,
        const FunctionArgs &args = {})
    {
        return create_object<FunctionEnumerator>(
            [save_self, forward_self, func](const FunctionArgs &args) { return (forward_self->*func)(args); },
            args);
    }

    ObjectPtr Enumerable::each2(
        const FunctionArgs &args,
        std::function<ObjectPtr(const FunctionArgs &args)> func)
    {
        auto args2 = args;
        args2.push_back(create_object<FunctionProc>(func));
        return each(args2);
    }

    ObjectPtr Enumerable::each_single(
        const FunctionArgs &args,
        std::function<ObjectPtr(Object *arg)> func)
    {
        return each2(args, [func](const FunctionArgs &args) {
            auto v = args.size() == 1 ? args[0] : make_value(args);
            return func(v.get());
        });
    }

    Ptr<Boolean> Enumerable::all_q(const FunctionArgs &args)
    {
        Proc *proc = nullptr;
        try_unpack<0>(args, &proc);
        try
        {
            if (proc)
            {
                each2({}, [proc](const FunctionArgs &args) {
                    if(!proc->call(args)->is_true())
                        throw SpecialFlowException();
                    else return NIL_VALUE;
                });
            }
            else
            {
                each2({}, [](const FunctionArgs &args) {
                    if (args.size() == 1 && !args[0]->is_true())
                        throw SpecialFlowException();
                    else return NIL_VALUE;
                });
            }
            return TRUE_VALUE;
        }
        catch (const SpecialFlowException &) { return FALSE_VALUE; }
    }

    Ptr<Boolean> Enumerable::any_q(const FunctionArgs &args)
    {
        Proc *proc = nullptr;
        try_unpack<0>(args, &proc);
        try
        {
            if (proc)
            {
                each2({}, [proc](const FunctionArgs &args) {
                    if (proc->call(args)->is_true())
                        throw SpecialFlowException();
                    else return NIL_VALUE;
                });
            }
            else
            {
                each2({}, [](const FunctionArgs &args) {
                    if ((args.size() > 1 || args[0]->is_true()))
                        throw SpecialFlowException();
                    else return NIL_VALUE;
                });
            }
            return FALSE_VALUE;
        }
        catch (const SpecialFlowException &) { return TRUE_VALUE; }
    }

    Ptr<Number> Enumerable::count(const FunctionArgs &args)
    {
        unsigned count = 0;
        if (args.empty())
        {
            each2({}, [&count](const FunctionArgs &args) {
                return ++count, NIL_VALUE;
            });
        }
        else if (args.size() == 1)
        {
            auto proc = dynamic_cast<Proc*>(args[0].get());
            if (proc)
            {
                each2({}, [proc, &count](const FunctionArgs &args) {
                    if (proc->call(args)->is_true()) ++count;
                    return NIL_VALUE;
                });
            }
            else
            {
                auto itm = args[0].get();
                each2({}, [itm, &count](const FunctionArgs &args) {
                    if (eq(itm, args[0].get())) ++count;
                    return NIL_VALUE;
                });
            }
        }
        else throw ArgumentCountError(args.size(), 0, 1);
        return make_value(count);
    }

    Ptr<Array> Enumerable::drop(Number *n)
    {
        auto ret = create_object<Array>();
        unsigned i = 0;
        each_single([&ret, &i, n](Object *arg) {
            if (++i > n->get_value())
                ret->push_back(arg);
            return NIL_VALUE;
        });
        return ret;
    }

    ObjectPtr Enumerable::drop_while(const FunctionArgs &args)
    {
        if (args.empty()) return make_enumerator(this_obj(), this, &Enumerable::drop_while, "drop_while");
        else if (args.size() == 1)
        {
            auto ret = create_object<Array>();
            bool start = false;
            auto proc = coerce<Proc>(args[0].get());
            each2({}, [proc, &ret, &start](const FunctionArgs &args) {
                start = start || !proc->call(args)->is_true();
                if (start)
                {
                    if (args.size() == 1) ret->push_back(args[0]);
                    else ret->push_back(make_value(args));
                }
                return NIL_VALUE;
            });
            return ret;
        }
        else throw ArgumentCountError(args.size(), 0, 1);
    }


    ObjectPtr Enumerable::each_with_index(const FunctionArgs &args)
    {
        Proc *proc = nullptr;
        if (!args.empty()) proc = dynamic_cast<Proc*>(args.back().get());

        if (proc)
        {
            auto args2 = args;
            args2.pop_back();
            unsigned i = 0;
            return each_single(args2, [&i, proc](Object *arg) {
                return proc->call({ arg->shared_from_this(), make_value(i++) });
            });
        }
        else
        {
            return make_enumerator(this_obj(), this, &Enumerable::each_with_index, "each_with_index", args);
        }
    }

    ObjectPtr Enumerable::find(const FunctionArgs &args)
    {
        if (args.size() > 2) throw ArgumentCountError(args.size(), 0, 2);
        auto proc = args.empty() ? nullptr : dynamic_cast<Proc*>(args.back().get());
        if (proc)
        {
            try
            {
                each2({}, [proc](const FunctionArgs &args) {
                    if (proc->call(args)->is_true())
                        throw SpecialFlowException(args.size() == 1 ? args[0] : make_value(args));
                    return NIL_VALUE;
                });
                return args.size() == 2 ? args.front() : NIL_VALUE;
            }
            catch (const SpecialFlowException &e) { return e.value; }
        }
        else
        {
            if (args.size() > 1) throw TypeError(args.back().get(), Proc::name());
            return make_enumerator(this_obj(), this, &Enumerable::find, "find", args);
        }
    }

    ObjectPtr Enumerable::find_index(const FunctionArgs &args)
    {
        if (args.empty()) return make_enumerator(this_obj(), this, &Enumerable::find_index, "find_index", args);
        if (args.size() > 1) throw ArgumentCountError(args.size(), 0, 1);
        
        try
        {
            unsigned i = 0;
            if (auto proc = dynamic_cast<Proc*>(args[0].get()))
            {
                each2({}, [&i, proc](const FunctionArgs &args) {
                    if (proc->call(args)->is_true())
                        throw SpecialFlowException(make_value(i));
                    ++i;
                    return NIL_VALUE;
                });
            }
            else //find value
            {
                auto value = args[0].get();
                each2({}, [&i, value](const FunctionArgs &args) {
                    if (eq(value, args.size() == 1 ? args[0].get() : make_value(args).get()))
                        throw SpecialFlowException(make_value(i));
                    ++i;
                    return NIL_VALUE;
                });
            }
            return NIL_VALUE;
        }
        catch (const SpecialFlowException &e) { return e.value; }
    }

    ObjectPtr Enumerable::map(const FunctionArgs &args)
    {
        Proc *proc = nullptr;
        unpack<0>(args, &proc);
        if (proc)
        {
            auto ret = create_object<Array>();
            each2({}, [ret, proc](const FunctionArgs &args) {
                ret->push_back(proc->call(args));
                return NIL_VALUE;
            });
            return ret;
        }
        else return make_enumerator(this_obj(), this, &Enumerable::map, "map");
    }

    namespace
    {
        void minmax_args(const FunctionArgs &args, Proc **proc, int *n)
        {
            if (args.size() == 0)
            {
                *proc = nullptr;
                *n = 1;
            }
            else if (args.size() == 1)
            {
                *proc = dynamic_cast<Proc*>(args[0].get());
                if (*proc) *n = 1;
                else *n = (int)coerce<Number>(args[0])->get_value();
            }
            else if (args.size() == 2)
            {
                *n = (int)coerce<Number>(args[0])->get_value();
                *proc = coerce<Proc>(args[1]).get();
            }
            else throw ArgumentCountError(args.size(), 0, 2);
        }
    
        template<class T> ObjectPtr do_min(Enumerable *self, int n, T cmp)
        {
            if (n == 0) return make_array({});
            else if (n == 1)
            {
                ObjectPtr min = nullptr;
                self->each_single([&min, cmp](Object *nextp) {
                    auto next = nextp->shared_from_this();
                    if (!min) min = next;
                    else if (cmp(next, min)) min = next;
                    return NIL_VALUE;
                });
                return min ? min : NIL_VALUE;
            }
            else if (n > 1)
            {
                std::deque<ObjectPtr> arr;
                self->each_single([&arr, cmp, n](Object *nextp) {
                    auto next = nextp->shared_from_this();
                    arr.insert(std::upper_bound(arr.begin(), arr.end(), next, cmp), next);
                    if (arr.size() > (size_t)n) arr.pop_back();
                    return NIL_VALUE;
                });
                return make_array({arr.begin(), arr.end()});
            }
            else throw ArgumentError("negative size (" + std::to_string(n) + ")");
        }
    }
    ObjectPtr Enumerable::max(const FunctionArgs &args)
    {
        Proc *proc;
        int n;
        minmax_args(args, &proc, &n);
        if (proc)
        {
            return do_min(this, n, [proc](ObjectPtr a, ObjectPtr b) {
                return coerce<Number>(proc->call({ a, b }))->get_value() > 0;
            });
        }
        else
        {
            return do_min(this, n, [](ObjectPtr a, ObjectPtr b) {
                return slim::cmp(a.get(), b.get()) > 0;
            });
        }
    }
    ObjectPtr Enumerable::max_by(const FunctionArgs &args)
    {
        Proc *proc;
        int n;
        minmax_args(args, &proc, &n);
        if (proc)
        {
            return do_min(this, n, [proc](ObjectPtr a, ObjectPtr b) {
                return slim::cmp(proc->call({a}).get(), proc->call({b}).get()) > 0;
            });
        }
        else return make_enumerator(this_obj(), this, &Enumerable::max_by, "max_by", args);
    }
    ObjectPtr Enumerable::min(const FunctionArgs &args)
    {
        Proc *proc;
        int n;
        minmax_args(args, &proc, &n);
        if (proc)
        {
            return do_min(this, n, [proc](ObjectPtr a, ObjectPtr b) {
                return coerce<Number>(proc->call({ a, b }))->get_value() < 0;
            });
        }
        else
        {
            return do_min(this, n, [](ObjectPtr a, ObjectPtr b) {
                return slim::cmp(a.get(), b.get()) < 0;
            });
        }
    }
    ObjectPtr Enumerable::min_by(const FunctionArgs &args)
    {
        Proc *proc;
        int n;
        minmax_args(args, &proc, &n);
        if (proc)
        {
            return do_min(this, n, [proc](ObjectPtr a, ObjectPtr b) {
                return slim::cmp(proc->call({ a }).get(), proc->call({ b }).get()) < 0;
            });
        }
        else return make_enumerator(this_obj(), this, &Enumerable::min_by, "min_by", args);
    }

    namespace
    {
        template<class T> ObjectPtr do_minmax(Enumerable *self, T cmp)
        {
            ObjectPtr min = nullptr, max = nullptr;
            self->each_single([&min, &max, cmp](Object *nextp) {
                auto next = nextp->shared_from_this();
                if (!min) min = max = next;
                else
                {
                    if (cmp(next, min) < 0) min = next;
                    if (cmp(next, max) > 0) max = next;
                }
                return NIL_VALUE;
            });
            return make_array({min ? min : NIL_VALUE, max ? max : NIL_VALUE});
        }
    }
    ObjectPtr Enumerable::minmax(const FunctionArgs &args)
    {
        Proc *proc = nullptr;
        unpack<0>(args, &proc);
        if (proc)
        {
            return do_minmax(this, [proc](ObjectPtr a, ObjectPtr b) {
                return (int)coerce<Number>(proc->call({ a, b }))->get_value();
            });
        }
        else
        {
            return do_minmax(this, [](ObjectPtr a, ObjectPtr b) {
                return slim::cmp(a.get(), b.get());
            });
        }
    }

    ObjectPtr Enumerable::minmax_by(const FunctionArgs &args)
    {
        Proc *proc = nullptr;
        unpack<0>(args, &proc);
        if (proc)
        {
            return do_minmax(this, [proc](ObjectPtr a, ObjectPtr b) {
                return slim::cmp(proc->call({a}).get(), proc->call({b}).get());
            });
        }
        else return make_enumerator(this_obj(), this, &Enumerable::minmax_by, "minmax_by");
    }

    ObjectPtr Enumerable::reject(const FunctionArgs &args)
    {
        Proc *proc = nullptr;
        unpack<0>(args, &proc);
        if (proc)
        {
            auto ret = create_object<Array>();
            each2({}, [ret, proc](const FunctionArgs &args) {
                if (!proc->call(args)->is_true())
                    ret->push_back(args.size() == 1 ? args[0] : make_value(args));
                return NIL_VALUE;
            });
            return ret;
        }
        else return make_enumerator(this_obj(), this, &Enumerable::reject, "reject", args);
    }

    ObjectPtr Enumerable::select(const FunctionArgs &args)
    {
        Proc *proc = nullptr;
        unpack<0>(args, &proc);
        if (proc)
        {
            auto ret = create_object<Array>();
            each2({}, [ret, proc](const FunctionArgs &args) {
                if (proc->call(args)->is_true())
                    ret->push_back(args.size() == 1 ? args[0] : make_value(args));
                return NIL_VALUE;
            });
            return ret;
        }
        else return make_enumerator(this_obj(), this, &Enumerable::select, "select", args);
    }

    Ptr<Array> Enumerable::to_a(const FunctionArgs &args)
    {
        auto ret = create_object<Array>();
        each2(args, [ret](const FunctionArgs &args)
        {
            if (args.size() == 1) ret->push_back(args[0]);
            else ret->push_back(make_value(args));
            return NIL_VALUE;
        });
        return ret;
    }

    Ptr<Hash> Enumerable::to_h(const FunctionArgs &args)
    {
        auto ret = create_object<Hash>();
        each2(args, [ret](const FunctionArgs &args)
        {
            if (args.size() == 2) ret->set(args[0], args[1]);
            else if (args.size() == 1)
            {
                auto arr = coerce<Array>(args[0]);
                if (arr->get_value().size() == 2)
                {
                    ret->set(arr->get_value()[0], arr->get_value()[1]);
                }
                else
                {
                    std::stringstream ss;
                    ss << "wrong array length. expected 2, was " << arr->get_value().size();
                    throw ArgumentError(ss.str());
                }
            }
            else throw ArgumentCountError(args.size(), 2, 2);
            return NIL_VALUE;
        });
        return ret;
    }
}
