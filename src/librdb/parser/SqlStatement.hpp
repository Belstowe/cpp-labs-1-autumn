#pragma once

#include "librdb/Token.hpp"
#include <initializer_list>
#include <string>
#include <variant>
#include <vector>

namespace rdb::parser {
using Value = std::variant<long, double, std::string>;

std::ostream& operator<<(std::ostream& os, const Value& value);

struct Operand {
    bool is_id;
    Value val;
    Operand(Value&& val, const bool is_id = false);
    Operand(long&& val);
};

std::ostream& operator<<(std::ostream& os, const Operand& operand);

typedef struct t_column_def {
    std::string column_name;
    TokenType type_name;
} ColumnDef;

typedef struct t_expression {
    Operand loperand;
    std::string operation;
    Operand roperand;
} Expression;

std::ostream& operator<<(std::ostream& os, const Expression& expression);

class SqlStatement {
public:
    virtual ~SqlStatement() = default;
    virtual void print(std::ostream& os) const = 0;
};

std::ostream& operator<<(std::ostream& os, const SqlStatement& statement);

class CreateTableStatement : public SqlStatement {
private:
    std::string table_name_;
    std::vector<ColumnDef> column_def_seq_;

public:
    ~CreateTableStatement() = default;
    CreateTableStatement(std::string&&, std::vector<ColumnDef>&&);
    void print(std::ostream& os) const;
    const std::string& table_name() const;
    const ColumnDef& column_def(size_t index) const;
    size_t columns_defined() const;
};

class InsertStatement : public SqlStatement {
private:
    std::string table_name_;
    std::vector<std::string> column_name_seq_;
    std::vector<Value> value_seq_;

public:
    ~InsertStatement() = default;
    InsertStatement(
            std::string&&, std::vector<std::string>&&, std::vector<Value>&&);
    void print(std::ostream& os) const;
    const std::string& table_name() const;
    const std::string& column_name(size_t index) const;
    size_t columns_defined() const;
    const Value& value(size_t index) const;
};

class SelectStatement : public SqlStatement {
private:
    std::string table_name_;
    std::vector<std::string> column_name_seq_;
    bool has_expression_cond_;
    Expression expression_;

public:
    ~SelectStatement() = default;
    SelectStatement(
            std::string&&,
            std::vector<std::string>&&,
            const Expression& = Expression{0, "N", 0});
    void print(std::ostream& os) const;
    const std::string& table_name() const;
    const std::string& column_name(size_t index) const;
    size_t columns_defined() const;
    bool has_expression() const;
    const Expression& expression() const;
};

class DeleteFromStatement : public SqlStatement {
private:
    std::string table_name_;
    bool has_expression_cond_;
    Expression expression_;

public:
    ~DeleteFromStatement() = default;
    DeleteFromStatement(
            std::string&&, const Expression& = Expression{0, "N", 0});
    void print(std::ostream& os) const;
    const std::string& table_name() const;
    bool has_expression() const;
    const Expression& expression() const;
};

class DropTableStatement : public SqlStatement {
private:
    std::string table_name_;

public:
    ~DropTableStatement() = default;
    DropTableStatement(std::string&&);
    void print(std::ostream& os) const;
    const std::string& table_name() const;
};
} // namespace rdb::parser