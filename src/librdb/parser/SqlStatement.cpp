#include "SqlStatement.hpp"

using namespace rdb::parser;

std::ostream& rdb::parser::operator<<(std::ostream& os, const Value& value)
{
    std::visit([&os](auto&& v) { os << v; }, value);
    return os;
}

std::ostream& rdb::parser::operator<<(std::ostream& os, const Operand& operand)
{
    os << operand.val;
    return os;
}

std::ostream&
rdb::parser::operator<<(std::ostream& os, const Expression& expression)
{
    os << expression.loperand << " " << expression.operation << " "
       << expression.roperand;
    return os;
}

std::ostream&
rdb::parser::operator<<(std::ostream& os, const SqlStatement& statement)
{
    statement.print(os);
    return os;
}

Operand::Operand(Value val, bool is_id) : val{val}, is_id{is_id}
{
}

Operand::Operand(long val) : val{val}, is_id{false}
{
}

CreateTableStatement::CreateTableStatement(
        std::string table_name, std::vector<ColumnDef> column_def_seq)
    : _table_name{table_name}, _column_def_seq{column_def_seq}
{
}

std::string CreateTableStatement::table_name() const
{
    return _table_name;
}

ColumnDef CreateTableStatement::column_def(size_t index) const
{
    return _column_def_seq.at(index);
}

size_t CreateTableStatement::columns_defined() const
{
    return _column_def_seq.size();
}

void CreateTableStatement::print(std::ostream& os) const
{
    os << "\"create_statement\":\n\t";
    os << "{ \n\t";
    os << "\"table_name\": " << _table_name << ",\n\t";
    os << "\"column_def_seq\": [\n\t";
    for (auto&& column_def : _column_def_seq) {
        os << "\t{ ";
        os << "\"column_name\": " << column_def.column_name << ", ";
        os << "\"type\": " << column_def.type_name;
        os << " } ";
        os << "\n\t";
    }
    os << "] }";
}

InsertStatement::InsertStatement(
        std::string table_name,
        std::vector<std::string> column_name_seq,
        std::vector<Value> value_seq)
    : _table_name{table_name},
      _column_name_seq{column_name_seq},
      _value_seq{value_seq}
{
}

std::string InsertStatement::table_name() const
{
    return _table_name;
}

std::string InsertStatement::column_name(size_t index) const
{
    return _column_name_seq.at(index);
}

size_t InsertStatement::columns_defined() const
{
    return _column_name_seq.size();
}

Value InsertStatement::value(size_t index) const
{
    return _value_seq.at(index);
}

void InsertStatement::print(std::ostream& os) const
{
    os << "\"insert_statement\":\n\t";
    os << "{ \n\t";
    os << "\"table_name\": " << _table_name << ",\n\t";
    os << "\"column_write_seq\": [\n\t";
    for (size_t index = 0; index < _column_name_seq.size(); index++) {
        os << "\t{ ";
        os << "\"column_name\": " << _column_name_seq.at(index) << ", ";
        os << "\"value\": " << _value_seq.at(index);
        os << " } ";
        os << "\n\t";
    }
    os << "] }";
}

SelectStatement::SelectStatement(
        std::string table_name,
        std::vector<std::string> column_name_seq,
        Expression expression)
    : _table_name{table_name},
      _column_name_seq{column_name_seq},
      _expression{expression}
{
    _has_expression_cond = (expression.operation == "N") ? false : true;
}

std::string SelectStatement::table_name() const
{
    return _table_name;
}

std::string SelectStatement::column_name(size_t index) const
{
    return _column_name_seq.at(index);
}

size_t SelectStatement::columns_defined() const
{
    return _column_name_seq.size();
}

bool SelectStatement::has_expression() const
{
    return _has_expression_cond;
}

Expression SelectStatement::expression() const
{
    if (_has_expression_cond) {
        return _expression;
    } else {
        throw std::runtime_error("SelectStatement: No expression defined");
    }
}

void SelectStatement::print(std::ostream& os) const
{
    os << "\"select_statement\":\n\t";
    os << "{ \n\t";
    os << "\"table_name\": " << _table_name << ",\n\t";
    os << "\"column_name_seq\": [\n\t";
    for (auto&& column_name : _column_name_seq) {
        os << "\t{ ";
        os << "\"column_name\": " << column_name;
        os << " } ";
        os << "\n\t";
    }
    os << "] ";
    if (_has_expression_cond) {
        os << ",\n\t";
        os << "\"expression\": " << _expression << "\n\t";
    }
    os << "}";
}

DeleteFromStatement::DeleteFromStatement(
        std::string table_name, Expression expression)
    : _table_name{table_name}, _expression{expression}
{
    _has_expression_cond = (expression.operation == "N") ? false : true;
}

std::string DeleteFromStatement::table_name() const
{
    return _table_name;
}

bool DeleteFromStatement::has_expression() const
{
    return _has_expression_cond;
}

Expression DeleteFromStatement::expression() const
{
    if (_has_expression_cond) {
        return _expression;
    } else {
        throw std::runtime_error("DeleteFromStatement: No expression defined");
    }
}

void DeleteFromStatement::print(std::ostream& os) const
{
    os << "\"delete_statement\":\n\t";
    os << "{ \n\t";
    os << "\"table_name\": " << _table_name;
    if (_has_expression_cond) {
        os << " ,\n\t";
        os << "\"expression\": " << _expression;
    }
    os << "\n\t}";
}

DropTableStatement::DropTableStatement(std::string table_name)
    : _table_name{table_name}
{
}

std::string DropTableStatement::table_name() const
{
    return _table_name;
}

void DropTableStatement::print(std::ostream& os) const
{
    os << "\"drop_statement\":\n\t";
    os << "{ \n\t";
    os << "\"table_name\": " << _table_name << "\n\t";
    os << "}";
}