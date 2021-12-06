#include "SqlStatement.hpp"

using namespace rdb::parser;

std::ostream& rdb::parser::operator<<(std::ostream& os, const Value& value) {
    std::visit([&os](auto&& v) { os << v; }, value);
    return os;
}

std::ostream& rdb::parser::operator<<(std::ostream& os, const Operand& operand) {
    os << operand.val;
    return os;
}

std::ostream& rdb::parser::operator<<(std::ostream& os, const Expression& expression) {
    os << expression.loperand << " " << expression.operation << " " << expression.roperand;
    return os;
}

std::ostream& rdb::parser::operator<<(std::ostream& os, const SqlStatement& statement) {
    statement.print(os);
    return os;
}

Operand::Operand(Value val, bool is_id)
    : val{val}, is_id{is_id}
    {
    }

Operand::Operand(long val)
    : val{val}, is_id{false}
    {
    }

CreateTableStatement::CreateTableStatement(std::string table_name, std::vector<ColumnDef> column_def_seq)
    : _table_name{table_name}, _column_def_seq{column_def_seq}
{
}

void CreateTableStatement::print(std::ostream& os) const {
    os << "{ \n";
    os << "\"table_name\": " << _table_name << ",\n";
    os << "\"column_def_seq\": [\n";
    for (auto&& column_def : _column_def_seq) {
        os << "\t{ ";
        os << "\"column_name\": " << column_def.column_name << ", ";
        os << "\"type\": " << column_def.type_name;
        os << " } ";
        os << "\n";
    }
    os << "] }";
}

InsertStatement::InsertStatement(std::string table_name, std::vector<std::string> column_name_seq, std::vector<Value> value_seq)
    : _table_name{table_name}, _column_name_seq{column_name_seq}, _value_seq{value_seq}
{
}

void InsertStatement::print(std::ostream& os) const {
    os << "{ \n";
    os << "\"table_name\": " << _table_name << ",\n";
    os << "\"column_write_seq\": [\n";
    for (size_t index = 0; index < _column_name_seq.size(); index++) {
        os << "\t{ ";
        os << "\"column_name\": " << _column_name_seq.at(index) << ", ";
        os << "\"value\": " << _value_seq.at(index);
        os << " } ";
        os << "\n";
    }
    os << "] }";
}

SelectStatement::SelectStatement(std::string table_name, std::vector<std::string> column_name_seq, Expression expression)
    : _table_name{table_name}, _column_name_seq{column_name_seq}, _expression{expression}
{
    _has_expression_cond = (expression.operation == "N") ? false : true;
}

void SelectStatement::print(std::ostream& os) const {
    os << "{ \n";
    os << "\"table_name\": " << _table_name << ",\n";
    os << "\"column_name_seq\": [\n";
    for (auto&& column_name : _column_name_seq) {
        os << "\t{ ";
        os << "\"column_name\": " << column_name;
        os << " } ";
        os << "\n";
    }
    os << "] ";
    if (_has_expression_cond) {
        os << ",\n";
        os << "\"expression\": " << _expression << "\n";
    }
    os << "}";
}

DeleteFromStatement::DeleteFromStatement(std::string table_name, Expression expression)
    : _table_name{table_name}, _expression{expression}
{
    _has_expression_cond = (expression.operation == "N") ? false : true;
}

void DeleteFromStatement::print(std::ostream& os) const {
    os << "{ \n";
    os << "\"table_name\": " << _table_name;
    if (_has_expression_cond) {
        os << " ,\n";
        os << "\"expression\": " << _expression;
    }
    os << "\n}";
}

DropTableStatement::DropTableStatement(std::string table_name)
    : _table_name{table_name}
{
}

void DropTableStatement::print(std::ostream& os) const {
    os << "{ \n";
    os << "\"table_name\": " << _table_name << "\n";
    os << "}";
}