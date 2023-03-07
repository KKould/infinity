//
// Created by JinHai on 2022/9/23.
//

#include "column_identifier.h"

namespace infinity {

ColumnIdentifier
ColumnIdentifier::MakeColumnIdentifier(SharedPtr<QueryContext>& query_context,
                                       const ColumnExpr& expr) {
    PlannerAssert(expr.star_ == false, "Star expression should be unfolded before.")

    SharedPtr<String> db_name_ptr = nullptr;
    SharedPtr<String> schema_name_ptr = nullptr;
    SharedPtr<String> table_name_ptr = nullptr;
    SharedPtr<String> column_name_ptr = nullptr;

    i64 name_count = expr.names_.size();
    PlannerAssert(name_count <= 4 && name_count > 0, "Invalid field count of the column name.");
    -- name_count;
    column_name_ptr = MakeShared<String>(expr.names_[name_count]);
    -- name_count;
    if(name_count >= 0) {
        table_name_ptr = MakeShared<String>(expr.names_[name_count]);
        -- name_count;
        if(name_count >= 0) {
            schema_name_ptr = MakeShared<String>(expr.names_[name_count]);
            -- name_count;
            if(name_count >= 0) {
                db_name_ptr = MakeShared<String>(expr.names_[name_count]);
            }
        }
    }

    SharedPtr<String> alias_name_ptr = nullptr;
    if(!expr.alias_.empty()) {
        alias_name_ptr = MakeShared<String>(expr.alias_);
    }
    return ColumnIdentifier(db_name_ptr, schema_name_ptr, table_name_ptr, column_name_ptr, alias_name_ptr);
}


ColumnIdentifier::ColumnIdentifier(SharedPtr<String> db_name,
                                   SharedPtr<String> schema_name,
                                   SharedPtr<String> table_name,
                                   SharedPtr<String> column_name,
                                   SharedPtr<String> alias_name)
                                   : db_name_ptr_(std::move(db_name)),
                                   schema_name_ptr_(std::move(schema_name)),
                                   column_name_ptr_(std::move(column_name)),
                                   table_name_ptr_(std::move(table_name)),
                                   alias_name_ptr_(std::move(alias_name))
{}

String
ColumnIdentifier::ToString() const {
    if(table_name_ptr_ != nullptr) return *table_name_ptr_ + "." + *column_name_ptr_;
    else return *column_name_ptr_;
}

bool
ColumnIdentifier::operator==(const ColumnIdentifier& other) const {
    if(this == &other) return true;
    if(*column_name_ptr_ != *other.column_name_ptr_) {
        return false;
    }
    if(table_name_ptr_ != nullptr && other.table_name_ptr_ != nullptr) {
        return *table_name_ptr_ == *other.table_name_ptr_;
    }

    if(table_name_ptr_ == nullptr && other.table_name_ptr_ == nullptr) {
        return true;
    }

    return false;
}

}