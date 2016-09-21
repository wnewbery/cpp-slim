#pragma once
#include <slim/Value.hpp>
#include <slim/Function.hpp>
#include <sstream>

class Vector2 : public slim::Object
{
public:
    //Constructors
    Vector2(double x, double y) : x(x), y(y) {}
    static std::shared_ptr<Vector2> new_instance(slim::Number *x, slim::Number *y)
    {
        return slim::create_object<Vector2>(x->get_value(), y->get_value());
    }

    //core utilities
    static const std::string &name()
    {
        static const std::string TYPE_NAME = "Vector2";
        return TYPE_NAME;
    }
    virtual const std::string& type_name()const override { return name(); }
    virtual std::string inspect()const override
    {
        std::stringstream ss;
        ss << "(" << x << ", " << y << ")";
        return ss.str();
    }

    //operators
    virtual bool eq(const slim::Object *rhs)const override
    {
        auto rhs2 = slim::coerce<Vector2>(rhs);
        return x == rhs2->x && y == rhs2->y;
    }
    virtual slim::ObjectPtr add(slim::Object *rhs)override
    {
        auto rhs2 = slim::coerce<Vector2>(rhs);
        return slim::create_object<Vector2>(x + rhs2->x, y + rhs2->y);
    }
    virtual slim::ObjectPtr sub(slim::Object *rhs)override
    {
        auto rhs2 = slim::coerce<Vector2>(rhs);
        return slim::create_object<Vector2>(x - rhs2->x, y - rhs2->y);
    }
    std::shared_ptr<slim::Number> dot(Vector2 *rhs)
    {
        return slim::make_value(x * rhs->x + y * rhs->y);
    }

protected:
    virtual const slim::MethodTable &method_table()const override
    {
        static const slim::MethodTable table(slim::Object::method_table(),
        {
            slim::Method::getter(&Vector2::x, "x"),
            slim::Method::getter(&Vector2::y, "y"),
            { &Vector2::dot, "dot" }
        });
        return table;
    }
private:
    double x, y;
};
