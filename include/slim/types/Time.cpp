#include "types/Time.hpp"
#include "types/Array.hpp"
#include "types/Nil.hpp"
#include "types/Number.hpp"
#include "types/String.hpp"
#include "FunctionHelpers.hpp"

namespace slim
{
    namespace
    {
        struct Flags
        {
            enum Padding
            {
                DEFAULT,
                SPACES,
                ZEROS
            };
            bool not_numeric = false;
            Padding padding = DEFAULT;
            bool upcase = false;
            bool colon_tz = false;
        };
        class Format
        {
        public:
            Flags flags;
            int width = -1;

            void parse(const std::string &fmt, size_t *i)
            {
                parse_flags(fmt, i);
                parse_width(fmt, i);
                parse_modifiers(fmt, i);
            }
            void fmt_num(std::string *out, int default_width, long long value)const
            {
                fmt_num(out, default_width, Flags::ZEROS, value);
            }
            void fmt_num(std::string *out, int default_width, Flags::Padding default_padding, long long value)const
            {
                assert(value >= 0);
                auto padd_width = width >= 0 ? width : default_width;
                auto str = std::to_string(value);
                if ((long)str.size() < padd_width && !flags.not_numeric)
                {
                    auto padd = padd_width - str.size();
                    Flags::Padding mode = flags.padding == Flags::DEFAULT ? default_padding : flags.padding;
                    char chr = mode == Flags::ZEROS ? '0' : ' ';
                    while (padd--) out->push_back(chr);
                }
                out->append(str);
            }
            
            void fmt_full_month(std::string *out, int value)const
            {
                static const char MONTHS[12][10] = {
                    "January", "February", "March", "April", "May", "June",
                    "July", "August", "September", "October", "November", "December" };
                assert(value >= 0 && value < 12);
                fmt_str(out, MONTHS[value]);
            }
            void fmt_short_month(std::string *out, int value)const
            {
                static const char MONTHS[12][4] = {
                    "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
                assert(value >= 0 && value < 12);
                fmt_str(out, MONTHS[value]);
            }
            void fmt_full_day(std::string *out, int value)const
            {
                static const char DAYS[7][10] = {
                    "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };
                assert(value >= 0 && value < 7);
                fmt_str(out, DAYS[value]);
            }
            void fmt_short_day(std::string *out, int value)const
            {
                static const char DAYS[7][4] = {
                    "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
                assert(value >= 0 && value < 7);
                fmt_str(out, DAYS[value]);
            }
            void fmt_str(std::string *out, const char *str)const
            {
                if (width > 0)
                {
                    size_t len = strlen(str);
                    size_t pad = len < width ? width - len : 0;
                    auto chr = flags.padding == Flags::ZEROS ? '0' : ' ';
                    while (pad--) out->push_back(chr);
                }
                if (flags.upcase)
                {
                    for (; *str; ++str)
                    {
                        if (*str >= 'a' && *str <= 'z')
                            out->push_back((char)(*str - 'a' + 'A'));
                        else out->push_back(*str);
                    }
                }
                else out->append(str);
            }
            void fmt_zone_offset(std::string *out, int offset_seconds)const
            {
                assert(offset_seconds == 0);
                if (flags.colon_tz) out->append("+00:00");
                else out->append("+0000");
            }
            void fmt_12_hour(std::string *out, Flags::Padding default_padding, int hours)const
            {
                auto h = hours % 12;
                if (h == 0) h = 12;
                fmt_num(out, 2, default_padding, h);
            }

            /**%a %b %e %T %Y*/
            void fmt_date_and_time(std::string *out, const tm &tm)const
            {
                std::string tmp;
                Format fmt;
                fmt.fmt_short_day(&tmp, tm.tm_wday); tmp += ' ';
                fmt.fmt_short_month(&tmp, tm.tm_mon); tmp += ' ';
                fmt.fmt_num(&tmp, 2, Flags::SPACES, tm.tm_mday); tmp += ' ';
                fmt.fmt_24_time_seconds(&tmp, tm); tmp += ' ';
                fmt.fmt_num(&tmp, 4, tm.tm_year + 1900);
                fmt_str(out, tmp.c_str());
            }
            /**%m/%d/%y*/
            void fmt_date(std::string *out, const tm &tm)const
            {
                std::string tmp;
                Format fmt;
                fmt.fmt_num(&tmp, 2, tm.tm_mon + 1); tmp += '/';
                fmt.fmt_num(&tmp, 2, tm.tm_mday); tmp += '/';
                fmt.fmt_num(&tmp, 2, (tm.tm_year + 1900) % 100);
                fmt_str(out, tmp.c_str());
            }
            /**%Y-%m-%d*/
            void fmt_iso_8601(std::string *out, const tm &tm)const
            {
                std::string tmp;
                Format fmt;
                fmt.fmt_num(&tmp, 4, tm.tm_year + 1900); tmp += '-';
                fmt.fmt_num(&tmp, 2, tm.tm_mon + 1); tmp += '-';
                fmt.fmt_num(&tmp, 2, tm.tm_mday);
                fmt_str(out, tmp.c_str());
            }
            /**%e-%^b-%4Y*/
            void fmt_vms_date(std::string *out, const tm &tm)const
            {
                std::string tmp;
                Format fmt;
                fmt.flags.upcase = true;
                fmt.fmt_num(&tmp, 2, Flags::SPACES, tm.tm_mday); tmp += '-';
                fmt.fmt_short_month(&tmp, tm.tm_mon); tmp += '-';
                fmt.fmt_num(&tmp, 4, tm.tm_year + 1900);
                fmt_str(out, tmp.c_str());
            }
            /**%I:%M:%S %p*/
            void fmt_12_time(std::string *out, const tm &tm)const
            {
                std::string tmp;
                Format fmt;
                fmt.fmt_12_hour(&tmp, Flags::ZEROS, tm.tm_hour); tmp += ':';
                fmt.fmt_num(&tmp, 2, tm.tm_min); tmp += ':';
                fmt.fmt_num(&tmp, 2, tm.tm_sec); tmp += ' ';
                fmt.fmt_str(&tmp, tm.tm_hour >= 12 ? "PM" : "AM");
                fmt_str(out, tmp.c_str());
            }
            /**%H:%M*/
            void fmt_24_time(std::string *out, const tm &tm)const
            {
                std::string tmp;
                Format fmt;
                fmt.fmt_num(&tmp, 2, tm.tm_hour); tmp += ':';
                fmt.fmt_num(&tmp, 2, tm.tm_min);
                fmt_str(out, tmp.c_str());
            }
            /**%H:%M:%S*/
            void fmt_24_time_seconds(std::string *out, const tm &tm)const
            {
                std::string tmp;
                Format fmt;
                fmt.fmt_num(&tmp, 2, tm.tm_hour); tmp += ':';
                fmt.fmt_num(&tmp, 2, tm.tm_min); tmp += ':';
                fmt.fmt_num(&tmp, 2, tm.tm_sec);
                fmt_str(out, tmp.c_str());
            }
        private:
            void parse_flags(const std::string &fmt, size_t *i)
            {
                while (*i < fmt.size())
                {
                    auto c = fmt[*i];
                    switch (c)
                    {
                    case '-': flags.not_numeric = true; break;
                    case '_': flags.padding = Flags::SPACES; break;
                    case '0': flags.padding = Flags::ZEROS; break;
                    case '^': flags.upcase = true; break;
                    case ':': flags.colon_tz = true; break;
                    default: return;
                    }
                    ++(*i);
                }
            }

            void parse_width(const std::string &fmt, size_t *i)
            {
                // First digit
                if (*i < fmt.size())
                {
                    auto c = fmt[*i];
                    if (c >= '0' && c <= '9')
                    {
                        width = c - '0';
                        ++(*i);
                        // Possible 2nd digit
                        if (*i < fmt.size())
                        {
                            auto c = fmt[*i];
                            if (c >= '0' && c <= '9')
                            {
                                ++(*i);
                                width = width * 10 + (c - '0');
                            }
                        }
                    }
                }
            }

            void parse_modifiers(const std::string &fmt, size_t *i)
            {
                if (*i < fmt.size())
                {
                    auto c = fmt[*i];
                    if (c == 'E' || c == 'O')
                        ++(*i);
                }
            }
        };
        bool fmt_token(std::string *out, time_t t, const tm &tm, const Format &fmt, char conversion)
        {
            if (conversion == 'z')
            {
                fmt.fmt_zone_offset(out, 0);
                return true;
            }
            else if (fmt.flags.colon_tz) return false; //invalid flag

            switch (conversion) // except 'z', already handled
            {
            // years
            case 'Y': fmt.fmt_num(out, 4, tm.tm_year + 1900); return true;
            case 'C': fmt.fmt_num(out, 2, (tm.tm_year + 1900) / 100); return true;
            case 'y': fmt.fmt_num(out, 2, (tm.tm_year + 1900) % 100); return true;
            // months
            case 'm': fmt.fmt_num(out, 2, tm.tm_mon + 1); return true;
            case 'B': fmt.fmt_full_month(out, tm.tm_mon); return true;
            case 'b': fmt.fmt_short_month(out, tm.tm_mon); return true;
            case 'h': fmt.fmt_short_month(out, tm.tm_mon); return true;
            // weeks
            // days
            case 'd': fmt.fmt_num(out, 2, tm.tm_mday); return true;
            case 'e': fmt.fmt_num(out, 2, Flags::SPACES, tm.tm_mday); return true;
            case 'j': fmt.fmt_num(out, 3, tm.tm_yday + 1); return true;
            case 'A': fmt.fmt_full_day(out, tm.tm_wday); return true;
            case 'a': fmt.fmt_short_day(out, tm.tm_wday); return true;
            case 'u':
            {
                auto day = tm.tm_wday; //sunday = 0, want monday = 1 and sunday = 7
                if (day == 0) day = 7;
                fmt.fmt_num(out, 1, day); return true;
            }
            case 'w': fmt.fmt_num(out, 1, tm.tm_wday); return true;
            // hours
            case 'H': fmt.fmt_num(out, 2, Flags::ZEROS, tm.tm_hour); return true;
            case 'k': fmt.fmt_num(out, 2, Flags::SPACES, tm.tm_hour); return true;
            case 'I': fmt.fmt_12_hour(out, Flags::ZEROS, tm.tm_hour); return true;
            case 'l': fmt.fmt_12_hour(out, Flags::SPACES, tm.tm_hour); return true;
            case 'P': fmt.fmt_str(out, tm.tm_hour < 12 ? "am" : "pm"); return true;
            case 'p': fmt.fmt_str(out, tm.tm_hour < 12 ? "AM" : "PM"); return true;
            // minutes
            case 'M': fmt.fmt_num(out, 2, tm.tm_min); return true;
            // seconds
            case 'S': fmt.fmt_num(out, 2, tm.tm_sec); return true;
            case 's': fmt.fmt_num(out, 1, t); return true;
            // sub sec
            case 'L': fmt.fmt_num(out, 3, 0); return true;
            case 'N': fmt.fmt_num(out, 9, 0); return true;
            // time zone
            case 'Z': fmt.fmt_str(out, "UTC"); return true;
            // combination
            case 'c': fmt.fmt_date_and_time(out, tm); return true;
            case 'x':
            case 'D': fmt.fmt_date(out, tm); return true;
            case 'F': fmt.fmt_iso_8601(out, tm); return true;
            case 'v': fmt.fmt_vms_date(out, tm); return true;
            case 'r': fmt.fmt_12_time(out, tm); return true;
            case 'R': fmt.fmt_24_time(out, tm); return true;
            case 'X':
            case 'T': fmt.fmt_24_time_seconds(out, tm); return true;
            default: return false;
            }
        }
        /**threadsafe, locale and timezone independent version of time.h strftime with Ruby strftime extensions
         * http://ruby-doc.org/core-2.2.0/Time.html#method-i-strftime
         *
         * - '#' flag (change case) is not supported.
         * - week numbers are not supported
         * - '%::z' is not supported ('%:z' is)
         * - Padding is limited to 99, restricting the possible size of the output relative to the format string.
         */
        std::string strftime(time_t t, const tm &tm, const std::string &fmt)
        {
            std::string out;
            out.reserve(fmt.size() * 2);
            size_t i = 0;

            for (i = 0; i < fmt.size();)
            {
                if (fmt[i] == '%')
                {
                    if (i + 1 == fmt.size())
                    {
                        out += '%';
                        break;
                    }

                    auto c2 = fmt[i + 1];
                    if (c2 == 'n' || c2 == 't' || c2 == '%')
                    {
                        i += 2;
                        if (c2 == 'n') out += '\n';
                        if (c2 == 't') out += '\t';
                        if (c2 == '%') out += '%';
                        continue;
                    }

                    auto j = ++i;
                    //<flags><width><modifier>
                    Format token_fmt;
                    token_fmt.parse(fmt, &j);

                    if (j == fmt.size()) continue;

                    auto conversion = fmt[j++];
                    if (fmt_token(&out, t, tm, token_fmt, conversion))
                        i = j;
                    else out += '%';
                }
                else out += fmt[i++];
            }
            return out;
        }


        tm get_tm(const FunctionArgs &args)
        {
            int year;
            Object *o_month = nullptr, *o_day = nullptr, *o_hour = nullptr, *o_min = nullptr, *o_sec = nullptr;
            unpack<1>(args, &year, &o_month, &o_day, &o_hour, &o_min, &o_sec);

            int month, day, hour, min, sec;

            if (auto o_str = dynamic_cast<String*>(o_month))
            {
                auto &str = o_str->get_value();
                if (str == "jan") month = 1;
                else if (str == "feb") month = 2;
                else if (str == "mar") month = 3;
                else if (str == "apr") month = 4;
                else if (str == "may") month = 5;
                else if (str == "jun") month = 6;
                else if (str == "jul") month = 7;
                else if (str == "aug") month = 8;
                else if (str == "sep") month = 9;
                else if (str == "oct") month = 10;
                else if (str == "nov") month = 11;
                else if (str == "dec") month = 12;
                else throw ArgumentError("unknown month " + str);
            }
            else if (!o_month || o_month == NIL_VALUE.get()) month = 1;
            else month = (int)coerce<Number>(o_month)->get_value();

            if (!o_day || o_day == NIL_VALUE.get()) day = 1;
            else day = (int)coerce<Number>(o_day)->get_value();

            if (!o_hour || o_hour == NIL_VALUE.get()) hour = 0;
            else hour = (int)coerce<Number>(o_hour)->get_value();

            if (!o_min || o_min == NIL_VALUE.get()) min = 0;
            else min = (int)coerce<Number>(o_min)->get_value();

            if (!o_sec || o_sec == NIL_VALUE.get()) sec = 0;
            else sec = (int)coerce<Number>(o_sec)->get_value();

            if (year < 1900 || year > 3000 ||
                month < 1 || month > 12 ||
                day < 1 || day > 31 ||
                hour < 0 || hour > 24 ||
                min < 0 || min > 60 ||
                sec < 0 || sec > 61)
            {
                throw ArgumentError("argument out of range");
            }
            tm tm = { 0 };
            tm.tm_year = year - 1900;
            tm.tm_mon = month - 1;
            tm.tm_mday = day;
            tm.tm_hour = hour;
            tm.tm_min = min;
            tm.tm_sec = sec;

            return tm;
        }

        int parse_utc_offset(Object *o)
        {
            static const std::string err = "\"+HH:MM\" or \"-HH:MM\" expected for utc_offset";
            auto parse_num = [](const char *str)
            {
                if (str[0] < '0' || str[0] > '9' || str[1] < '0' || str[1] > '9')
                    throw ArgumentError(err);
                return (str[0] - '0') * 10 + (str[1] - '0');
            };
            if (auto n = dynamic_cast<Number*>(o))
            {
                return (int)n->get_value();
            }
            else
            {
                //<sign>hh:mm
                auto &str = coerce<String>(o)->get_value();
                if (str.size() != 1 + 2 + 1 + 2)
                    throw ArgumentError(err);

                bool positive;
                if (str[0] == '+') positive = true;
                else if (str[0] == '-') positive = false;
                else throw ArgumentError(err);

                if (str[3] != ':') throw ArgumentError(err);
                int hours = parse_num(str.data() + 1);
                int minutes = parse_num(str.data() + 4);

                return (positive ? 1 : -1) * (Time::TICKS_HOUR * hours + Time::TICKS_MIN * minutes);
            }
        }
    }
    
    Ptr<Time> TimeType::local(const FunctionArgs &args)const
    {
        auto tm = slim::get_tm(args);
        auto t = ::mktime(&tm);
        return at(t);
    }
    Ptr<Time> TimeType::utc(const FunctionArgs &args)const
    {
        auto tm = slim::get_tm(args);
        auto t = _mkgmtime(&tm);
        return at(t);
    }
    const MethodTable &TimeType::method_table()const
    {
        static const MethodTable table = MethodTable(Object::method_table(), {
            { &TimeType::at, "at" },
            { &TimeType::gm, "gm" },
            { &TimeType::local, "local" },
            { &TimeType::mktime, "mktime" },
            { &TimeType::new_instance, "new" },
            { &TimeType::now, "now" },
            { &TimeType::utc, "utc" }
        });
        return table;
    }

    Time::Time(const FunctionArgs &args)
    {
        if (args.empty())
        {
            v = time(nullptr);
        }
        else
        {
            tm tm;
            int offset = 0;
            if (args.size() == 7)
            {
                offset = parse_utc_offset(args.back().get());
                auto args2 = args;
                args2.pop_back();
                tm = slim::get_tm(args2);
            }
            else tm = slim::get_tm(args);

            v = _mkgmtime(&tm);
            v -= offset;
        }
    }
    std::string Time::to_string()const
    {
        return strftime("%Y-%m-%d %H:%M:%S %z");
    }
    tm Time::get_tm()const
    {
        tm tm;
        gmtime_s(&tm, &v);
        return tm;
    }


    size_t Time::hash()const
    {
        return std::hash<time_t>()(v);
    }
    bool Time::eq(const Object *rhs)const
    {
        return coerce<Time>(rhs)->v == v;
    }
    int Time::cmp(const Object *rhs)const
    {
        auto v2 =  coerce<Time>(rhs)->v;
        if (v < v2) return -1;
        else if (v > v2) return 1;
        else return 0;
    }
    ObjectPtr Time::add(Object *rhs)
    {
        return create_object<Time>(v + (time_t)coerce<Number>(rhs)->get_value()*TICKS_SECOND);
    }
    ObjectPtr Time::sub(Object *rhs)
    {
        return create_object<Time>(v - (time_t)coerce<Number>(rhs)->get_value()*TICKS_SECOND);
    }

    Ptr<Array> Time::to_a()const
    {
        tm tm = get_tm();
        std::vector<Ptr<Object>> arr = {
            make_value(sec()), make_value(min()), make_value(hour()),
            make_value(day()), make_value(month()), make_value(year()),
            make_value(wday()), make_value(yday()),
            FALSE_VALUE, make_value("UTC")
        };
        return make_array(std::move(arr));
    }

    int Time::day()const
    {
        return get_tm().tm_mday;
    }
    int Time::hour()const
    {
        return get_tm().tm_hour;
    }
    int Time::min()const
    {
        return get_tm().tm_min;
    }
    int Time::month()const
    {
        return get_tm().tm_mon + 1;
    }
    int Time::sec()const
    {
        return get_tm().tm_sec;
    }
    int Time::wday()const
    {
        return get_tm().tm_wday;
    }
    int Time::yday()const
    {
        return get_tm().tm_yday + 1;
    }
    int Time::year()const
    {
        return get_tm().tm_year + 1900;
    }


    std::string Time::asctime()const
    {
        return ctime();
    }
    std::string Time::ctime()const
    {
        return strftime("%a %b %e %T %Y");
    }
    std::string Time::strftime(const std::string &fmt)const
    {
        return slim::strftime(v, get_tm(), fmt);
    }

    const MethodTable &Time::method_table()const
    {
        static const MethodTable table = MethodTable(Object::method_table(), {
                { &Time::to_a, "to_a" },
                { &Time::to_f, "to_f" },
                { &Time::to_i, "to_i" },
                { &Time::asctime, "asctime" },
                { &Time::ctime, "ctime" },
                { &Time::strftime, "strftime" },

                { &Time::dst_q, "dst?" },
                { &Time::isdst, "isdst" },

                { &Time::getgm, "getgm" },
                { &Time::getutc, "getutc" },
                { &Time::utc, "utc" },
                { &Time::gmtime, "gmtime" },
                { &Time::gmt_q, "gmt?" },
                { &Time::utc_q, "utc?" },
                { &Time::gmt_offset, "gmt_offset" },
                { &Time::utc_offset, "utc_offset" },
                { &Time::gmtoff, "gmtoff" },
                { &Time::zone, "zone" },

                { &Time::day, "day" },
                { &Time::mday, "mday" },
                { &Time::hour, "hour" },
                { &Time::min, "min" },
                { &Time::month, "month" },
                { &Time::mon, "mon" },
                { &Time::nsec, "nsec" },
                { &Time::sec, "sec" },
                { &Time::usec, "usec" },
                { &Time::subsec, "subsec" },
                { &Time::wday, "wday" },
                { &Time::yday, "yday" },
                { &Time::year, "year" },

                { &Time::sunday_q, "sunday?" },
                { &Time::monday_q, "monday?" },
                { &Time::tuesday_q, "tuesday?" },
                { &Time::wednesday_q, "wednesday?" },
                { &Time::thursday_q, "thursday?" },
                { &Time::friday_q, "friday?" },
                { &Time::saturday_q, "saturday?" }
        });
        return table;
    }
}
