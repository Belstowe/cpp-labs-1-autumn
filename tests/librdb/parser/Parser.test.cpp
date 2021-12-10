#include "librdb/parser/Parser.hpp"
#include "librdb/lexer/Lexer.hpp"
#include "gtest/gtest.h"
#include <string>
#include <string_view>
#include <typeinfo>

using rdb::parser::ColumnDef;
using rdb::parser::ErrorType;
using rdb::parser::Expression;
using rdb::parser::Lexer;
using rdb::parser::Parser;
using rdb::parser::ParseResult;
using rdb::parser::SqlStatement;
using rdb::parser::TokenType;
using rdb::parser::Value;

TEST(ParserTest, CreateStatementExtraction)
{
    std::string instring(
            "CREATE TABLE users (name TEXT, age INT, meters REAL);");
    Lexer lexer(instring);
    Parser parser(lexer);
    ParseResult sql;
    parser.parse_sql(sql);

    ASSERT_EQ(sql._errors.size(), 0);
    ASSERT_EQ(sql._sql_script._sql_statements.size(), 1);
    ASSERT_EQ(
            typeid(sql._sql_script._sql_statements[0].get()),
            typeid(rdb::parser::CreateTableStatement*));

    rdb::parser::CreateTableStatement statement
            = dynamic_cast<rdb::parser::CreateTableStatement&>(
                    *sql._sql_script._sql_statements[0]);
    ASSERT_EQ(statement.table_name(), "users");
    ASSERT_EQ(statement.columns_defined(), 3);

    std::vector<std::string_view> column_name_expected_seq(
            {"name", "age", "meters"});
    std::vector<TokenType> type_name_expected_seq(
            {TokenType::KwText, TokenType::KwInt, TokenType::KwReal});
    for (size_t i = 0; i < 3; i++) {
        ColumnDef column_def = statement.column_def(i);
        ASSERT_EQ(column_def.column_name, column_name_expected_seq[i]);
        ASSERT_EQ(column_def.type_name, type_name_expected_seq[i]);
    }
}

TEST(ParserTest, InsertStatementExtraction)
{
    std::string instring(
            "INSERT INTO users (name, age, meters) VALUES (\"James\", 29, "
            "1.8);");
    Lexer lexer(instring);
    Parser parser(lexer);
    ParseResult sql;
    parser.parse_sql(sql);

    ASSERT_EQ(sql._errors.size(), 0);
    ASSERT_EQ(sql._sql_script._sql_statements.size(), 1);
    ASSERT_EQ(
            typeid(sql._sql_script._sql_statements[0].get()),
            typeid(rdb::parser::InsertStatement*));

    rdb::parser::InsertStatement statement
            = dynamic_cast<rdb::parser::InsertStatement&>(
                    *sql._sql_script._sql_statements[0]);
    ASSERT_EQ(statement.table_name(), "users");
    ASSERT_EQ(statement.columns_defined(), 3);

    std::vector<std::string_view> column_name_expected_seq(
            {"name", "age", "meters"});
    for (size_t i = 0; i < 3; i++) {
        ASSERT_EQ(statement.column_name(i), column_name_expected_seq[i]);
    }
    ASSERT_EQ(std::get<std::string>(statement.value(0)), "\"James\"");
    ASSERT_EQ(std::get<long>(statement.value(1)), 29);
    ASSERT_DOUBLE_EQ(std::get<double>(statement.value(2)), 1.8);
}

TEST(ParserTest, SelectStatementExtraction)
{
    std::string instring(
            "SELECT name age FROM users WHERE age >= 22; SELECT meters FROM "
            "users;");
    Lexer lexer(instring);
    Parser parser(lexer);
    ParseResult sql;
    parser.parse_sql(sql);

    ASSERT_EQ(sql._errors.size(), 0);
    ASSERT_EQ(sql._sql_script._sql_statements.size(), 2);
    ASSERT_EQ(
            typeid(sql._sql_script._sql_statements[0].get()),
            typeid(rdb::parser::SelectStatement*));
    ASSERT_EQ(
            typeid(sql._sql_script._sql_statements[1].get()),
            typeid(rdb::parser::SelectStatement*));

    rdb::parser::SelectStatement statement_expr
            = dynamic_cast<rdb::parser::SelectStatement&>(
                    *sql._sql_script._sql_statements[0]);
    rdb::parser::SelectStatement statement_no_expr
            = dynamic_cast<rdb::parser::SelectStatement&>(
                    *sql._sql_script._sql_statements[1]);
    ASSERT_EQ(statement_expr.table_name(), "users");
    ASSERT_EQ(statement_no_expr.table_name(), "users");
    ASSERT_EQ(statement_expr.columns_defined(), 2);
    ASSERT_EQ(statement_no_expr.columns_defined(), 1);

    std::vector<std::string_view> column_name_expected_seq({"name", "age"});
    for (size_t i = 0; i < 2; i++) {
        ASSERT_EQ(statement_expr.column_name(i), column_name_expected_seq[i]);
    }
    ASSERT_EQ(statement_no_expr.column_name(0), "meters");

    ASSERT_EQ(statement_expr.has_expression(), true);
    ASSERT_EQ(statement_no_expr.has_expression(), false);
    Expression expression = statement_expr.expression();
    ASSERT_EQ(std::get<std::string>(expression.loperand.val), "age");
    ASSERT_EQ(expression.loperand.is_id, true);
    ASSERT_EQ(expression.operation, ">=");
    ASSERT_EQ(std::get<long>(expression.roperand.val), 22);
    ASSERT_EQ(expression.roperand.is_id, false);
}

TEST(ParserTest, DeleteStatementExtraction)
{
    std::string instring(
            "DELETE FROM companies; DELETE FROM users WHERE name = \"James\";");
    Lexer lexer(instring);
    Parser parser(lexer);
    ParseResult sql;
    parser.parse_sql(sql);

    ASSERT_EQ(sql._errors.size(), 0);
    ASSERT_EQ(sql._sql_script._sql_statements.size(), 2);
    ASSERT_EQ(
            typeid(sql._sql_script._sql_statements[0].get()),
            typeid(rdb::parser::DeleteFromStatement*));
    ASSERT_EQ(
            typeid(sql._sql_script._sql_statements[1].get()),
            typeid(rdb::parser::DeleteFromStatement*));

    rdb::parser::DeleteFromStatement statement_no_expr
            = dynamic_cast<rdb::parser::DeleteFromStatement&>(
                    *sql._sql_script._sql_statements[0]);
    rdb::parser::DeleteFromStatement statement_expr
            = dynamic_cast<rdb::parser::DeleteFromStatement&>(
                    *sql._sql_script._sql_statements[1]);
    ASSERT_EQ(statement_no_expr.table_name(), "companies");
    ASSERT_EQ(statement_expr.table_name(), "users");

    ASSERT_EQ(statement_expr.has_expression(), true);
    ASSERT_EQ(statement_no_expr.has_expression(), false);
    Expression expression = statement_expr.expression();
    ASSERT_EQ(std::get<std::string>(expression.loperand.val), "name");
    ASSERT_EQ(expression.loperand.is_id, true);
    ASSERT_EQ(expression.operation, "=");
    ASSERT_EQ(std::get<std::string>(expression.roperand.val), "\"James\"");
    ASSERT_EQ(expression.roperand.is_id, false);
}

TEST(ParserTest, DropStatementExtraction)
{
    std::string instring("DROP TABLE users;");
    Lexer lexer(instring);
    Parser parser(lexer);
    ParseResult sql;
    parser.parse_sql(sql);

    ASSERT_EQ(sql._errors.size(), 0);
    ASSERT_EQ(sql._sql_script._sql_statements.size(), 1);
    ASSERT_EQ(
            typeid(sql._sql_script._sql_statements[0].get()),
            typeid(rdb::parser::DropTableStatement*));

    rdb::parser::DropTableStatement statement
            = dynamic_cast<rdb::parser::DropTableStatement&>(
                    *sql._sql_script._sql_statements[0]);
    ASSERT_EQ(statement.table_name(), "users");
}

TEST(ParserTest, LawfulInput)
{
    std::string instring(
            "CREATE TABLE users (name TEXT, age INT, meters REAL);\nINsert "
            "into users (name, age, meters) values (\"Johnny\", 22, "
            "1.68);\nDROP TABLE users;");
    Lexer lexer(instring);
    Parser parser(lexer);
    ParseResult sql;
    parser.parse_sql(sql);

    ASSERT_EQ(sql._errors.size(), 0);
    ASSERT_EQ(sql._sql_script._sql_statements.size(), 3);

    ASSERT_EQ(
            typeid(sql._sql_script._sql_statements[0].get()),
            typeid(rdb::parser::CreateTableStatement*));
    ASSERT_EQ(
            typeid(sql._sql_script._sql_statements[1].get()),
            typeid(rdb::parser::InsertStatement*));
    ASSERT_EQ(
            typeid(sql._sql_script._sql_statements[2].get()),
            typeid(rdb::parser::DropTableStatement*));
}

TEST(ParserTest, PartlyChaoticInput)
{
    std::string instring(
            "CREATE table animals;\ninsert into animals (kind, family) VALUES "
            "(\"Rabbit\",\"Mammals\"); insert values "
            "(\"Snake\",\"Reptiles\");\ndrop animals;");
    Lexer lexer(instring);
    Parser parser(lexer);
    ParseResult sql;
    parser.parse_sql(sql);

    ASSERT_EQ(sql._errors.size(), 3);
    ASSERT_EQ(sql._sql_script._sql_statements.size(), 1);

    ASSERT_EQ(
            typeid(sql._sql_script._sql_statements[0].get()),
            typeid(rdb::parser::InsertStatement*));
}

TEST(ParserTest, Misprints)
{
    std::string instring(
            "CREAET TABLE users (name TEXT, age INT, meters REAL); DROP TABL "
            "users");
    Lexer lexer(instring);
    Parser parser(lexer);
    ParseResult sql;
    parser.parse_sql(sql);

    ASSERT_EQ(sql._errors.size(), 3);
    ASSERT_EQ(sql._sql_script._sql_statements.size(), 0);

    ASSERT_EQ(sql._errors.at(0).type(), ErrorType::NotStatement);
    ASSERT_EQ(sql._errors.at(1).type(), ErrorType::SyntaxError);
    ASSERT_EQ(sql._errors.at(2).type(), ErrorType::UnexpectedEOF);
}