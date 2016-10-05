The embedded script syntax for templates is based on Ruby, using the same naming conventions, and with a comparable set of operators and literal values syntax.

A small number of [basic types](Types.md) are provided with similar methods to Ruby.

The main limitation is that the script syntax only currently supports fragments that form a single expression
(e.g. `5 + 2`, `5 + Math.sqrt 9`) and scripts are not able to define classes or methods. Such things can only
be added by implementing them as C++ classes that derive the `slim::Object` class.

Most of the types also do not provide script module/class instances (e.g. there is no `String` constant for
`slim::String`, so you can not do `String.new`) along with most of the other dynamic type methods
(`Object#class`, `Object#is_a?`, `Module#ancestors`, etc.).

# Literals
All numeric literals use the `Number` type with uses a C `double` as storage.
There is no seperate integer and big-number types.

Array and hash literals are supported using `[]` and `{}`. Hash literals may use either the `symbol: value` or `key => value` syntax.

String literals are supported including `#{}` interpolation.

Symbol literals are supported with the colon (`:`) prefix.

Regex literals use forward slash delimiters, e.g. `/Error: +(.+)/`.

Boolean and nil values, `true`, `false` and `nil`.

# Operators

   * Conditional operator: `cond ? true : false`
   * Logical or: `||`
   * Logical and: `&&`
   * Equality and comparison: `==`, `!=`, `<=>`
   * Comparison: `<`, `<=`, `>`, `>=`
   * Bitwise or and xor: `|`, `^`
   * Bitwise and: `&`
   * Bitshifts: `<<`, `>>`
   * Addition and subtraction: `+`, `-`
   * Multiplication, division and modulus: `*`, `/`, `%`
   * Unary operators: `!`, `~`, `+`, `-`
   * Power: `**`
   * Object members: `.`, `&.`, `::`
     Note that the `::` operator may only be used to access constants.
     Attempting to use it for a method call is an error.

# Method Calls

## Hash arguments
A hash argument may be implicitly created to be passed after all positional arguments, but before a
possible block argument by using the hash literal key-values syntax. Delimiting brackets are not required.

    mymethod 1, 5, name: 'Fred'

Is the same as:

    mymethod(1, 5, {name: 'Fred'})

## Block/Proc
Blocks can be passed to methods using the `{||}` syntax.
Unlike Ruby there no special concept of a block parameter (`block_given?`, `yield`, `&block`, etc.).
Blocks are always turned into a `Proc` instance and passed as the final method argument.

# Division and Regex Literal Ambiguity
When a division or regex literal is the first parameter to a method call, it is ambiguous as to if
the forward slash is a division operator on the method result or variable, or the start of a regex.

In Ruby, if there is a space after the forward slash then it is division, and if there is not a space
then its a regex literal. But the regex case raises a syntax order.
The template expression parser in this project however, disallows it and throws a `SyntaxError`.
Use parentheses for a regex literal or a space after the division operator

```
$ irb -w

 > "string".match /./
warning: ambiguous first argument; put parentheses or a space even after `/' operator
 > "string".match /5
/> /
warning: ambiguous first argument; put parentheses or a space even after `/' operator
 > "string".match(/./)
 > "string".match / 5
ArgumentError: wrong number of arguments (given 0, expected 1..2)
```
