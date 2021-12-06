#include "SqlStatement.hpp"

using namespace rdb::parser;

CreateTableStatement::CreateTableStatement(std::string table_name, std::vector<ColumnDef> column_def_seq)
    : _table_name{table_name}, _column_def_seq{column_def_seq}
{
}

std::ostream& rdb::parser::operator<<(std::ostream& os, const rdb::parser::CreateTableStatement& statement) {
    os << "{ \"table_name\": " << statement._table_name << ",\n";
    os << "\"column_def_seq\": [\n";
    for (auto &&column_def : statement._column_def_seq) {
        os << "\t{ ";
        os << "\"column_name\": " << column_def.column_name << ", ";
        os << "\"type\": " << static_cast<int>(column_def.type_name) << " ";
        os << "} ";
        os << "\n";
    }
    os << "] }";
    return os;
}

InsertStatement::InsertStatement(std::string table_name, std::vector<std::string> column_name_seq, std::vector<Value> value_seq)
    : _table_name{table_name}, _column_name_seq{column_name_seq}, _value_seq{value_seq}
{
}

SelectStatement::SelectStatement(std::string table_name, std::string column_name, Expression expression)
    : _table_name{table_name}, _column_name{column_name}, _expression{expression}
{
    _has_expression_cond = (expression.operation == "N") ? true : false;
}

DeleteFromStatement::DeleteFromStatement(std::string table_name, Expression expression)
    : _table_name{table_name}, _expression{expression}
{
    _has_expression_cond = (expression.operation == "N") ? true : false;
}

DropTableStatement::DropTableStatement(std::string table_name)
    : _table_name{table_name}
{
}