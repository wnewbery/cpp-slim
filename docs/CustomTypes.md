# Custom Script Types
Defining a custom script type is mostly a matter of deriving your type from `slim::Object` and
adding an instance to the `slim::ViewModel` (either as a module/class Constant or as part of the
data model) used to render the template.
In the case of helper methods, you might also choose to derived `ViewModel` itself.

Full example code can be found in `examples/custom-type`.

## Minimal type
All types must ultimately derive `slim::Object` and have their own type name.

Header:
```C++
/**2D vector.*/
class Vector2 : public slim::Object
{
public:
    //Constructor
    Vector2(double x, double y) : x(x), y(y) {}
    //Required static and override
    static const std::string &name()
    {
        static const std::string TYPE_NAME = "Vector2";
        return TYPE_NAME;
    }
    virtual const std::string& type_name()const override { return type_name(); }
private:
    double x, y;
};
```
Source:
```C++
const std::string Vector2::TYPE_NAME = "Vector2";
```

## Overriding Core Methods

Many of the core script methods, such as those used to implement the operators are virtual methods
of the `slim::Object` class, and so custom types override those C++ methods, rather than adding new
methods to the types method table.

```C++

class Vector2 : public slim::Object
{
public:
    ...
    virtual std::string inspect()const override
    {
        std::stringstream ss;
        ss << "(" << x << ", " << y << ")";
        return ss.str();
    }
    virtual ObjectPtr add(slim::Object *rhs)override
    {
        auto rhs2 = slim::coerce<Vector2>(rhs);
        return slim::create_object<Vector2>(x + rhs2->x, y + rhs2->y);
    }
    virtual slim::ObjectPtr sub(slim::Object *rhs)override
    {
        auto rhs2 = slim::coerce<Vector2>(rhs);
        return slim::create_object<Vector2>(x - rhs2->x, y - rhs2->y);
    }
```

## Adding custom methods

New member methods can easily be added to a type as long as they follow certain rules:

   * Methods may be C++ instance functions, or static/global functions.
   * The method should not be overloaded, the method table implementation itself currently
     has no means to do runtime overload resolution.
   * The method should not have default parameters, as C++ function pointer types do not
     retain the default parameters, the method table templates have no means to determine them.

   * The methods return type must either be:
      * `void`, which will be returned to the script as `nil`.
      * A `shared_ptr` containing either `slim::Object` or a subtype.

   * The methods paramters may be exactly `const slim::FunctionArgs &` which stores an array of
     objects passed by the script (including any blocks, as a `slim::Proc` instance) for the
     method to then unpack and handle as it likes, or it may be a fixed set of parameters
     (they will be mapped 1 to 1 by the script) that follow the rules for
     `slim::unpack(args, &arg1, &arg2, etc)`.

Additionally getters may be automatically created from a member variable pointer using `Method::getter`
for any type for which a `make_value` overload (including ADL) is defined.

If it is desired to have overloading or default values for the methods paramters, it is suggested
to used the `const FunctionArgs &` parameter list, and then determine them within the method, such
as by using `slim::try_unpack` and `slim::unpack`.

```C++
class Vector2 : public slim::Object
{
public:
    ...
    //Dot product
    std::shared_ptr<slim::Number> dot(Vector2 *rhs)
    {
        return slim::make_value(x * rhs->x + y * rhs->y);
    }
```

The `Object::method_table` method can then be overriden to make those C++ member methods available
to the script when the method calls `self` object is an instance of `Vector2`.

```C++
class Vector2 : public slim::Object
{
protected:
    virtual const MethodTable &Vector2::method_table()const override
    {
        static const slim::MethodTable table(slim::Object::method_table(),
        {
            slim::Method::getter(&Vector2::x, "x"),
            slim::Method::getter(&Vector2::y, "y"),
            { &Vector2::dot, "dot" }
        });
        return table;
    }
```

Because the C++ member method and variable signatures are a form that `slim::Method` understands,
no additional effort is required, and the checking of argument counts and types when the method is
called is done by `slim::Method::call`.

## Making it creatable in scripts

If it is desired that scripts are able to create new instances of this type, e.g. by `Vector2.new x, y`,
then a `Vector2` constant also needs to be set for that template/script, with a method `new`.

This is done by creating another type derived from `slim::Object` (e.g. `class Vector2Type : public Object`),
this time with a `new` method, where the implementation of that method creates the new instance. The
same can be done for any other desired static/class methods.

You then assign an instance of this "type class" as a constant.
