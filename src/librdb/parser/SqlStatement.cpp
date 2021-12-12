#include "SqlStatement.hpp"

std::ostream&
rdb::parser::operator<<(std::ostream& os, const rdb::parser::Value& value)
{
    std::visit([&os](auto&& v) { os << v; }, value);
    return os;
}

std::ostream&
rdb::parser::operator<<(std::ostream& os, const rdb::parser::Operand& operand)
{
    os << operand.val;
    return os;
}

std::ostream& rdb::parser::operator<<(
        std::ostream& os, const rdb::parser::Expression& expression)
{
    os << expression.loperand << " " << expression.operation << " "
       << expression.roperand;
    return os;
}

std::ostream& rdb::parser::operator<<(
        std::ostream& os, const rdb::parser::SqlStatement& statement)
{
    statement.print(os);
    return os;
}

rdb::parser::Operand::Operand(Value&& val, const bool is_id)
    : is_id{is_id}, val{val}
{
}

rdb::parser::Operand::Operand(long&& val) : is_id{false}, val{val}
{
}

rdb::parser::CreateTableStatement::CreateTableStatement(
        std::string&& table_name, std::vector<ColumnDef>&& column_def_seq)
    : _table_name{table_name}, _column_def_seq{column_def_seq}
{
}

const std::string& rdb::parser::CreateTableStatement::table_name() const
{
    return _table_name;
}

const rdb::parser::ColumnDef&
rdb::parser::CreateTableStatement::column_def(size_t index) const
{
    return _column_def_seq.at(index);
}

size_t rdb::parser::CreateTableStatement::columns_defined() const
{
    return _column_def_seq.size();
}

void rdb::parser::CreateTableStatement::print(std::ostream& os) const
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

rdb::parser::InsertStatement::InsertStatement(
        std::string&& table_name,
        std::vector<std::string>&& column_name_seq,
        std::vector<Value>&& value_seq)
    : _table_name{table_name},
      _column_name_seq{column_name_seq},
      _value_seq{value_seq}
{
}

const std::string& rdb::parser::InsertStatement::table_name() const
{
    return _table_name;
}

const std::string& rdb::parser::InsertStatement::column_name(size_t index) const
{
    return _column_name_seq.at(index);
}

size_t rdb::parser::InsertStatement::columns_defined() const
{
    return _column_name_seq.size();
}

const rdb::parser::Value&
rdb::parser::InsertStatement::value(size_t index) const
{
    return _value_seq.at(index);
}

void rdb::parser::InsertStatement::print(std::ostream& os) const
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

rdb::parser::SelectStatement::SelectStatement(
        std::string&& table_name,
        std::vector<std::string>&& column_name_seq,
        const Expression& expression)
    : _table_name{table_name},
      _column_name_seq{column_name_seq},
      _has_expression_cond{expression.operation != "N"},
      _expression{expression}
{
}

const std::string& rdb::parser::SelectStatement::table_name() const
{
    return _table_name;
}

const std::string& rdb::parser::SelectStatement::column_name(size_t index) const
{
    return _column_name_seq.at(index);
}

size_t rdb::parser::SelectStatement::columns_defined() const
{
    return _column_name_seq.size();
}

bool rdb::parser::SelectStatement::has_expression() const
{
    return _has_expression_cond;
}

const rdb::parser::Expression& rdb::parser::SelectStatement::expression() const
{
    if (_has_expression_cond) {
        return _expression;
    }
    throw std::runtime_error("SelectStatement: No expression defined");
}

void rdb::parser::SelectStatement::print(std::ostream& os) const
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

rdb::parser::DeleteFromStatement::DeleteFromStatement(
        std::string&& table_name, const Expression& expression)
    : _table_name{table_name},
      _has_expression_cond{expression.operation != "N"},
      _expression{expression}
{
}

const std::string& rdb::parser::DeleteFromStatement::table_name() const
{
    return _table_name;
}

bool rdb::parser::DeleteFromStatement::has_expression() const
{
    return _has_expression_cond;
}

const rdb::parser::Expression&
rdb::parser::DeleteFromStatement::expression() const
{
    if (_has_expression_cond) {
        return _expression;
    }
    throw std::runtime_error("DeleteFromStatement: No expression defined");
}

void rdb::parser::DeleteFromStatement::print(std::ostream& os) const
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

rdb::parser::DropTableStatement::DropTableStatement(std::string&& table_name)
    : _table_name{table_name}
{
}

const std::string& rdb::parser::DropTableStatement::table_name() const
{
    return _table_name;
}

void rdb::parser::DropTableStatement::print(std::ostream& os) const
{
    os << "\"drop_statement\":\n\t";
    os << "{ \n\t";
    os << "\"table_name\": " << _table_name << "\n\t";
    os << "}";
}