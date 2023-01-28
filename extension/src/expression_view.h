#ifndef MORPHY_EXPRESSION_VIEW_CLASS_H
#define MORPHY_EXPRESSION_VIEW_CLASS_H

#include "table_view.h"
#include "table_data.h"

#include <godot_cpp/classes/expression.hpp>
#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/classes/a_star2d.hpp>

namespace morphy
{

struct ExpressionHeader
{
    godot::String name;
    godot::Ref<godot::Expression> expression;

    godot::Variant execute(const godot::Array& inputs = godot::Array(), godot::Object* base_instance = nullptr);
};

class ExpressionColumn : public godot::RefCounted
{
    GDCLASS(ExpressionColumn, godot::RefCounted);

public:
    godot::Error set_name(const godot::String& p_name);
    const godot::String& get_name() const;

    godot::Error set_expression(const godot::String& p_expression);
    const godot::Ref<godot::Expression>& get_expression() const;

protected:
    static void _bind_methods();

private:
    godot::String name;
    godot::Ref<godot::Expression> expression;
};

class ExpressionView : public TableView
{
    GDCLASS(ExpressionView, TableView);

public:
    ExpressionView();
    virtual ~ExpressionView();

    godot::Error add_expressions(const godot::TypedArray<ExpressionColumn>& p_columns);

    void set_view(const godot::Ref<TableView>& p_view);

    virtual const godot::Variant& get_cell(int64_t p_column, int64_t p_row) const override;

protected:
    static void _bind_methods();

private:
    godot::Ref<TableView>          view;
    TableData<ExpressionHeader, 4> data;
};

} // namespace morphy

#endif // MORPHY_EXPRESSION_VIEW_CLASS_H