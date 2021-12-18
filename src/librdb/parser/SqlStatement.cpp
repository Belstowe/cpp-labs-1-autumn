#include "SqlStatement.hpp"

namespace rdb::parser {
std::ostream& operator<<(std::ostream& os, const Value& value)
{
    std::visit([&os](auto&& v) { os << v; }, value);
    return os;
}

std::ostream& operator<<(std::ostream& os, const Operand& operand)
{
    os << operand.val;
    return os;
}

std::ostream& operator<<(std::ostream& os, const Expression& expression)
{
    os << expression.loperand << " " << expression.operation << " "
       << expression.roperand;
    return os;
}

std::ostream& operator<<(std::ostream& os, const SqlStatement& statement)
{
    statement.print(os);
    return os;
}

Operand::Operand(Value&& val, const bool is_id) : is_id{is_id}, val{val}
{
}

Operand::Operand(long&& val) : is_id{false}, val{val}
{
}

CreateTableStatement::CreateTableStatement(
        std::string&& table_name, std::vector<ColumnDef>&& column_def_seq)
    : table_name_{table_name}, column_def_seq_{column_def_seq}
{
}

const std::string& CreateTableStatement::table_name() const
{
    return table_name_;
}

const ColumnDef& CreateTableStatement::column_def(size_t index) const
{
    return column_def_seq_.at(index);
}

size_t CreateTableStatement::columns_defined() const
{
    return column_def_seq_.size();
}

void CreateTableStatement::print(std::ostream& os) const
{
    os << "\"create_statement\":\n\t";
    os << "{ \n\t";
    os << "\"table_name\": " << table_name_ << ",\n\t";
    os << "\"column_def_seq\": [\n\t";
    for (auto&& column_def : column_def_seq_) {
        os << "\t{ ";
        os << "\"column_name\": " << column_def.column_name << ", ";
        os << "\"type\": " << column_def.type_name;
        os << " } ";
        os << "\n\t";
    }
    os << "] }";
}

InsertStatement::InsertStatement(
        std::string&& table_name,
        std::vector<std::string>&& column_name_seq,
        std::vector<Value>&& value_seq)
    : table_name_{table_name},
      column_name_seq_{column_name_seq},
      value_seq_{value_seq}
{
}

const std::string& InsertStatement::table_name() const
{
    return table_name_;
}

const std::string& InsertStatement::column_name(size_t index) const
{
    return column_name_seq_.at(index);
}

size_t InsertStatement::columns_defined() const
{
    return column_name_seq_.size();
}

const Value& InsertStatement::value(size_t index) const
{
    return value_seq_.at(index);
}

void InsertStatement::print(std::ostream& os) const
{
    os << "\"insert_statement\":\n\t";
    os << "{ \n\t";
    os << "\"table_name\": " << table_name_ << ",\n\t";
    os << "\"column_write_seq\": [\n\t";
    for (size_t index = 0; index < column_name_seq_.size(); index++) {
        os << "\t{ ";
        os << "\"column_name\": " << column_name_seq_.at(index) << ", ";
        os << "\"value\": " << value_seq_.at(index);
        os << " } ";
        os << "\n\t";
    }
    os << "] }";
}

SelectStatement::SelectStatement(
        std::string&& table_name,
        std::vector<std::string>&& column_name_seq,
        const Expression& expression)
    : table_name_{table_name},
      column_name_seq_{column_name_seq},
      has_expression_cond_{expression.operation != "N"},
      expression_{expression}
{
}

const std::string& SelectStatement::table_name() const
{
    return table_name_;
}

const std::string& SelectStatement::column_name(size_t index) const
{
    return column_name_seq_.at(index);
}

size_t SelectStatement::columns_defined() const
{
    return column_name_seq_.size();
}

bool SelectStatement::has_expression() const
{
    return has_expression_cond_;
}

const Expression& SelectStatement::expression() const
{
    if (has_expression_cond_) {
        return expression_;
    }
    throw std::runtime_error("SelectStatement: No expression defined");
}

void SelectStatement::print(std::ostream& os) const
{
    os << "\"select_statement\":\n\t";
    os << "{ \n\t";
    os << "\"table_name\": " << table_name_ << ",\n\t";
    os << "\"column_name_seq\": [\n\t";
    for (auto&& column_name : column_name_seq_) {
        os << "\t{ ";
        os << "\"column_name\": " << column_name;
        os << " } ";
        os << "\n\t";
    }
    os << "] ";
    if (has_expression_cond_) {
        os << ",\n\t";
        os << "\"expression\": " << expression_ << "\n\t";
    }
    os << "}";
}

DeleteFromStatement::DeleteFromStatement(
        std::string&& table_name, const Expression& expression)
    : table_name_{table_name},
      has_expression_cond_{expression.operation != "N"},
      expression_{expression}
{
}

const std::string& DeleteFromStatement::table_name() const
{
    return table_name_;
}

bool DeleteFromStatement::has_expression() const
{
    return has_expression_cond_;
}

const Expression& DeleteFromStatement::expression() const
{
    if (has_expression_cond_) {
        return expression_;
    }
    throw std::runtime_error("DeleteFromStatement: No expression defined");
}

void DeleteFromStatement::print(std::ostream& os) const
{
    os << "\"delete_statement\":\n\t";
    os << "{ \n\t";
    os << "\"table_name\": " << table_name_;
    if (has_expression_cond_) {
        os << " ,\n\t";
        os << "\"expression\": " << expression_;
    }
    os << "\n\t}";
}

DropTableStatement::DropTableStatement(std::string&& table_name)
    : table_name_{table_name}
{
}

const std::string& DropTableStatement::table_name() const
{
    return table_name_;
}

void DropTableStatement::print(std::ostream& os) const
{
    os << "\"drop_statement\":\n\t";
    os << "{ \n\t";
    os << "\"table_name\": " << table_name_ << "\n\t";
    os << "}";
}
} // namespace rdb::parser