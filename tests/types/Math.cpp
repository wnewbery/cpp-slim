#include <boost/test/unit_test.hpp>
#include "expression/Parser.hpp"
#include "expression/Ast.hpp"
#include "expression/Lexer.hpp"
#include "expression/Scope.hpp"
#include "types/Number.hpp"
#include "types/Math.hpp"
#include <cmath>

using namespace slim;
using namespace slim::expr;
BOOST_AUTO_TEST_SUITE(TestMath)

ObjectPtr eval2(const std::string &str)
{
    Lexer lexer(str);
    expr::LocalVarNames vars;
    Parser parser(vars, lexer);
    auto expr = parser.full_expression();
    auto model = create_view_model();
    model->add_constant("Math", create_object<Math>());
    Scope scope(model);
    return expr->eval(scope);
}
double eval(const std::string &str)
{
    auto result = eval2(str);
    return coerce<Number>(result)->get_value();
}

BOOST_AUTO_TEST_CASE(basic_methods)
{
    BOOST_CHECK_EQUAL("Math", create_object<Math>()->type_name());
    BOOST_CHECK_EQUAL("Math", create_object<Math>()->inspect());
    BOOST_CHECK_EQUAL("Math", create_object<Math>()->to_string());

    BOOST_CHECK_CLOSE(2.718281828459045, eval("Math::E"), 0.0001);
    BOOST_CHECK_CLOSE(3.141592653589793, eval("Math::PI"), 0.0001);
    BOOST_CHECK_THROW(eval("X"), NoConstantError);

    BOOST_CHECK_CLOSE(1.5707963267948966, eval("Math.acos(0)"), 0.0001);
    BOOST_CHECK_CLOSE(1.5707963267948966, eval("Math.asin(1)"), 0.0001);
    BOOST_CHECK_CLOSE(0.7853981633974483, eval("Math.atan(1)"), 0.0001);
    BOOST_CHECK_CLOSE(0.4636476090008061, eval("Math.atan2(1,2)"), 0.0001);
    BOOST_CHECK_CLOSE(1.3169578969248166, eval("Math.acosh(2)"), 0.0001);
    BOOST_CHECK_CLOSE(1.4436354751788103, eval("Math.asinh(2)"), 0.0001);
    BOOST_CHECK_CLOSE(0.5493061443340548, eval("Math.atanh(0.5)"), 0.0001);
    BOOST_CHECK_CLOSE(0.5403023058681398, eval("Math.cos(1)"), 0.0001);
    BOOST_CHECK_CLOSE(0.8414709848078965, eval("Math.sin(1)"), 0.0001);
    BOOST_CHECK_CLOSE(1.5574077246549023, eval("Math.tan(1)"), 0.0001);
    BOOST_CHECK_CLOSE(1.5430806348152437, eval("Math.cosh(1)"), 0.0001);
    BOOST_CHECK_CLOSE(1.1752011936438014, eval("Math.sinh(1)"), 0.0001);
    BOOST_CHECK_CLOSE(0.7615941559557649, eval("Math.tanh(1)"), 0.0001);

    BOOST_CHECK_CLOSE(3, eval("Math.cbrt(27)"), 0.0001);
    BOOST_CHECK_CLOSE(3, eval("Math.sqrt(9)"), 0.0001);
    BOOST_CHECK_CLOSE(2.718281828459045, eval("Math.exp(1)"), 0.0001);
    BOOST_CHECK_CLOSE(0.6931471805599453, eval("Math.log(2)"), 0.0001);
    BOOST_CHECK_CLOSE(3, eval("Math.log2(8)"), 0.0001);
    BOOST_CHECK_CLOSE(4, eval("Math.log10(10000)"), 0.0001);

    BOOST_CHECK_CLOSE(0.8427007929497149, eval("Math.erf(1)"), 0.0001);
    BOOST_CHECK_CLOSE(0.15729920705028513, eval("Math.erfc(1)"), 0.0001);
    BOOST_CHECK_CLOSE(1.772453850905516, eval("Math.gamma(0.5)"), 0.0001);
    BOOST_CHECK_CLOSE(std::sqrt(2 * 2 + 3 * 3), eval("Math.hypot(2, 3)"), 0.0001);
    BOOST_CHECK_CLOSE(4, eval("Math.ldexp(1, 2)"), 0.0001);
    BOOST_CHECK_EQUAL("[0.572365, 1]", eval2("Math.lgamma(0.5)")->inspect());
}

BOOST_AUTO_TEST_SUITE_END()
