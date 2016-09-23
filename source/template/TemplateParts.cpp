#include "template/TemplateParts.hpp"
#include "template/Attributes.hpp"
#include "expression/Expression.hpp"
#include "types/Array.hpp"
#include "types/Boolean.hpp"
#include "types/Nil.hpp"
#include "types/Enumerator.hpp"
#include "types/Symbol.hpp"
#include "types/Proc.hpp"
#include "Util.hpp"
namespace slim
{
    namespace tpl
    {
        TemplateOutputExpr::TemplateOutputExpr(std::unique_ptr<Expression>&& expression)
            : expression(std::move(expression))
        {}
        TemplateOutputExpr::~TemplateOutputExpr()
        {}
        std::string TemplateOutputExpr::to_string() const
        {
            return "<%= " + expression->to_string() + " %>";
        }
        void TemplateOutputExpr::render(std::string & buffer, expr::Scope &scope) const
        {
            auto val = expression->eval(scope);
            buffer += html_escape(val);
        }


        TemplateTagAttr::TemplateTagAttr(
            const std::string &attr,
            std::vector<std::string> &&static_values,
            std::vector<std::unique_ptr<Expression>> &&dynamic_values)
            : attr(attr)
            , static_values(std::move(static_values))
            , dynamic_values(std::move(dynamic_values))
        {}
        TemplateTagAttr::~TemplateTagAttr()
        {}

        std::string TemplateTagAttr::to_string()const
        {
            std::string buf = "<%=attr('" + attr + "'";
            for (auto &x : static_values) buf += ", '" + x + "'";
            for (auto &x : dynamic_values) buf += ", " + x->to_string();
            buf += ")%>";
            return buf;
        }
        void TemplateTagAttr::render(std::string &buffer, expr::Scope &scope)const
        {
            std::vector<ObjectPtr> values;
            for (auto &expr : dynamic_values)
            {
                auto val = expr->eval(scope);
                if (auto arr = dynamic_cast<Array*>(val.get()))
                {
                    for (auto val2 : arr->get_value()) values.push_back(val2);
                }
                else values.push_back(val);
            }

            if (static_values.empty() && values.empty()) return;

            if (static_values.empty() && values.size() == 1)
            {
                if (values[0] == TRUE_VALUE)
                {
                    buffer += ' ' + attr;
                    return;
                }
                else if (values[0] == FALSE_VALUE || values[0] == NIL_VALUE) return;
            }

            std::vector<std::string> strings = static_values;
            for (auto &v : values) strings.push_back(html_escape(v));

            buffer += attr_str(attr, strings);
        }

        TemplateForExpr::TemplateForExpr(
            std::unique_ptr<Expression> &&expr,
            std::unique_ptr<TemplatePart> &&body,
            std::vector<std::shared_ptr<Symbol>> &&param_names)
            : expr(std::move(expr)), body(std::move(body)), param_names(std::move(param_names))
        {}
        TemplateForExpr::~TemplateForExpr()
        {}
        std::string TemplateForExpr::to_string() const
        {
            std::string out =  "<% " + expr->to_string() + " do |";
            if (!param_names.empty()) out += param_names[0]->str();
            for (size_t i = 1; i < param_names.size(); ++i) out += ", " + param_names[i]->str();
            out += "| %>";
            out += body->to_string();
            out += "<% end %>";
            return out;
        }
        void TemplateForExpr::render(std::string &buffer, expr::Scope &scope) const
        {
            struct CallNode : public expr::ExpressionNode
            {
                CallNode(const TemplatePart *body, std::string &buffer) : body(body), buffer(buffer) {}
                virtual std::string to_string()const override { std::terminate(); }
                virtual ObjectPtr eval(expr::Scope &scope)const override
                {
                    body->render(buffer, scope);
                    return NIL_VALUE;
                }
                const TemplatePart *body;
                std::string &buffer;
            };
            CallNode call(body.get(), buffer);
            auto result = expr->eval(scope);
            auto enumerator = coerce<Enumerator>(result);
            auto proc = std::make_shared<BlockProc>(call, param_names, scope);
            enumerator->each({ proc });
        }

        TemplateIfExpr::TemplateIfExpr(TemplateCondExpr &&if_expr, std::vector<TemplateCondExpr> &&elseif_exprs, std::unique_ptr<TemplatePart> &&else_body)
            : if_expr(std::move(if_expr)), elseif_exprs(std::move(elseif_exprs)), else_body(std::move(else_body))
        {
            assert(this->if_expr.expr != nullptr);
        }
        TemplateIfExpr::~TemplateIfExpr()
        {}
        std::string TemplateIfExpr::to_string() const
        {
            std::string out;
            out += "<% if " + if_expr.expr->to_string() + " %>" + if_expr.body->to_string();
            for (auto &elseif : elseif_exprs)
            {
                out += "<% elsif " + elseif.expr->to_string() + " %>" + elseif.body->to_string();
            }
            if (else_body) out += "<% else %>" + else_body->to_string();
            out += "<% end %>";
            return out;
        }
        void TemplateIfExpr::render(std::string & buffer, expr::Scope &scope) const
        {
            if (if_expr.expr->eval(scope)->is_true())
            {
                if_expr.body->render(buffer, scope);
                return;
            }
            for (auto &elseif : elseif_exprs)
            {
                if (elseif.expr->eval(scope)->is_true())
                {
                    elseif.body->render(buffer, scope);
                    return;
                }
            }
            if (else_body)
            {
                else_body->render(buffer, scope);
            }
        }

    }
}
