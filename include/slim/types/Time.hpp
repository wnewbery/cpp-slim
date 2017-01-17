#pragma once
#include "Object.hpp"
#include "Number.hpp"
#include "Type.hpp"
#include <ctime>
namespace slim
{
    class Array;
    class Boolean;
    class Number;

    /**Time object using time_t (seconds since 1970).
     *
     * Has most of the Ruby methods, and some Rails methods, but only works with UTC internally.
     */
    class Time : public Object
    {
    public:
        static const int TICKS_SECOND = 1;
        static const int TICKS_MIN = TICKS_SECOND * 60;
        static const int TICKS_HOUR = TICKS_MIN * 60;
        static const int TICKS_DAY = TICKS_HOUR * 24;
        static const int TICKS_WEEK = TICKS_DAY * 7;
        Time(time_t t) : v(t) {}
        Time(const FunctionArgs &args);

        static const std::string &name()
        {
            static const std::string TYPE_NAME = "Time";
            return TYPE_NAME;
        }
        virtual const std::string& type_name()const override { return name(); }

        time_t get_value()const { return v; }
        tm get_tm()const;

        virtual std::string to_string()const override;
        virtual std::string inspect()const override { return to_string(); }
        virtual size_t hash()const override;
        virtual bool eq(const Object *rhs)const override;
        virtual int cmp(const Object *rhs)const override;
        virtual ObjectPtr add(Object *rhs)override;
        virtual ObjectPtr sub(Object *rhs)override;

        Ptr<Array> to_a()const;
        double to_f()const { return (double)v; }
        long long to_i()const { return v; }



        std::string asctime()const;
        std::string ctime()const;
        std::string strftime(const std::string &fmt)const;

        bool dst_q() { return false; }
        bool isdst() { return false; }

        Ptr<Time> getgm() { return this_ptr<Time>(); }
        //Ptr<Time> getlocal(const FunctionArgs &args);
        Ptr<Time> getutc() { return this_ptr<Time>(); }
        Ptr<Time> utc() { return this_ptr<Time>(); }
        Ptr<Time> gmtime() { return utc(); }

        bool gmt_q()const { return true; }
        bool utc_q()const { return true; }
        int gmt_offset()const { return 0; }
        int utc_offset()const { return 0; }
        int gmtoff()const { return 0; }
        std::string zone()const { return "UTC"; }

        /**Day of month, 1..n*/
        int day()const;
        int hour()const;
        int min()const;
        /**Month of the year, 1..12*/
        int month()const;
        int nsec()const { return 0; }
        int sec()const;
        float subsec()const { return 0; }
        int usec()const { return 0; }
        /**Day of week, 0..6*/
        int wday()const;
        /**Day of year, 1..366*/
        int yday()const;
        int year()const;

        bool sunday_q()const { return wday() == 0; }
        bool monday_q()const { return wday() == 1; }
        bool tuesday_q()const { return wday() == 2; }
        bool wednesday_q()const { return wday() == 3; }
        bool thursday_q()const { return wday() == 4; }
        bool friday_q()const { return wday() == 5; }
        bool saturday_q()const { return wday() == 6; }
    protected:
        virtual const MethodTable &method_table()const;
    private:
        time_t v;
    };

    class TimeType : public Type
    {
    public:
        static const std::string &name()
        {
            static const std::string TYPE_NAME = "Class";
            return TYPE_NAME;
        }
        virtual const std::string& type_name()const override { return name(); }

        Ptr<Time> at(time_t seconds)const
        {
            return create_object<Time>(seconds);
        }
        Ptr<Time> gm(const FunctionArgs &args)const { return utc(args); }
        Ptr<Time> local(const FunctionArgs &args)const;
        Ptr<Time> mktime(const FunctionArgs &args)const { return local(args); }
        Ptr<Time> new_instance(const FunctionArgs &args)const
        {
            return create_object<Time>(args);
        }
        Ptr<Time> now()const
        {
            return at(time(nullptr));
        }
        Ptr<Time> utc(const FunctionArgs &args)const;
    protected:
        virtual const MethodTable &method_table()const;
    };

    // Rails number extensions
    Ptr<Time> number_ago(const Number *self, const FunctionArgs &args);
    Ptr<Time> number_from_now(const Number *self, const FunctionArgs &args);
    inline Ptr<Number> number_fortnights(const Number *self)
    {
        return make_value(self->get_value() * 2 * Time::TICKS_WEEK);
    }
    inline Ptr<Number> number_weeks(const Number *self)
    {
        return make_value(self->get_value() * Time::TICKS_WEEK);
    }
    inline Ptr<Number> number_days(const Number *self)
    {
        return make_value(self->get_value() * Time::TICKS_DAY);
    }
    inline Ptr<Number> number_hours(const Number *self)
    {
        return make_value(self->get_value() * Time::TICKS_HOUR);
    }
    inline Ptr<Number> number_minutes(const Number *self)
    {
        return make_value(self->get_value() * Time::TICKS_MIN);
    }
    inline Ptr<Number> number_seconds(const Number *self)
    {
        return make_value(self->get_value() * Time::TICKS_SECOND);
    }
}
