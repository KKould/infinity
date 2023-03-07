//
// Created by JinHai on 2022/8/11.
//

#pragma once

#include "planner/expression_binder.h"
#include "bind_alias_proxy.h"

namespace infinity {

class GroupBinder : public ExpressionBinder {
public:
    i64 group_by_expr_index {-1};

    HashSet<i64> bound_select_index_;

public:
    explicit GroupBinder(SharedPtr<QueryContext>& query_context,
                         const SharedPtr<BindAliasProxy>& bind_alias_proxy)
                         : ExpressionBinder(query_context),
                         bind_alias_proxy_(bind_alias_proxy) {}

    // Bind expression entry
    SharedPtr<BaseExpression>
    BuildExpression(const ParsedExpr& expr,
                    const SharedPtr<BindContext>& bind_context_ptr,
                    i64 depth,
                    bool root) override;

    SharedPtr<BaseExpression>
    BindColumnReference(const ColumnExpr& expr,
                         const SharedPtr<BindContext>& bind_context_ptr);

    SharedPtr<BaseExpression>
    BindConstantExpression(const ConstantExpr& expr,
                           const SharedPtr<BindContext>& bind_context_ptr);

    SharedPtr<BaseExpression>
    BuildColExpr(const ColumnExpr& expr,
                 const SharedPtr<BindContext>& bind_context_ptr,
                 i64 depth,
                 bool root) override;

    SharedPtr<BaseExpression>
    BuildFuncExpr(const FunctionExpr& expr,
                  const SharedPtr<BindContext>& bind_context_ptr,
                  i64 depth,
                  bool root) override;

    void
    CheckFuncType(FunctionType func_type) const override;

    SharedPtr<SubqueryExpression>
    BuildSubquery(const SubqueryExpr& select,
                  const SharedPtr<BindContext>& bind_context_ptr,
                  SubqueryType subquery_type,
                  i64 depth,
                  bool root) final;
private:

    const SharedPtr<BindAliasProxy>& bind_alias_proxy_;
};

}

