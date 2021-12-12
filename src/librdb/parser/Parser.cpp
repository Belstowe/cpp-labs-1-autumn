#include "Parser.hpp"
#include <charconv>
#include <stdexcept>

using rdb::parser::Error;
using rdb::parser::ErrorType;
using rdb::parser::Lexer;
using rdb::parser::ParseResult;
using rdb::parser::SqlStatement;
using rdb::parser::Token;
using rdb::parser::TokenType;

std::ostream&
rdb::parser::operator<<(std::ostream& os, const rdb::parser::SqlScript& sql)
{
    for (auto&& statement_ptr : sql._sql_statements) {
        auto* statement = statement_ptr.get();
        os << *statement << "\n";
    }
    return os;
}

std::string parse_token(Lexer& lexer, TokenType&& expected_token)
{
    Token token = lexer.get();
    if (token.type != expected_token) {
        if (token.type == TokenType::EndOfFile) {
            throw Error(token, ErrorType::UnexpectedEOF, expected_token);
        }
        throw Error(token, ErrorType::SyntaxError, expected_token);
    }

    return std::string(token.lexeme);
}

template <typename T>
rdb::parser::Value convert_lexeme_to_var(Token& token, TokenType&& token_type)
{
    T result{};
    auto [ptr, ec]{std::from_chars(
            token.lexeme.data(),
            token.lexeme.data() + token.lexeme.size(),
            result)};
    if (ec == std::errc::result_out_of_range) {
        throw Error(token, ErrorType::VarOutOfRange, token_type);
    }
    if (ec == std::errc::invalid_argument) {
        throw Error(token, ErrorType::IncorrectVarType, token_type);
    }

    return result;
}

rdb::parser::Value convert_lexeme_to_double(Token& token)
{
    try {
        std::string str(token.lexeme);
        return std::stod(str);
    } catch (const std::invalid_argument&) {
        throw Error(token, ErrorType::IncorrectVarType, TokenType::KwReal);
    } catch (const std::out_of_range&) {
        throw Error(token, ErrorType::VarOutOfRange, TokenType::KwReal);
    }
}

void parse_operand(Lexer& lexer, rdb::parser::Operand& operand)
{
    Token token = lexer.get();
    operand.is_id = false;

    switch (token.type) {
    case TokenType::VarInt:
        operand.val = convert_lexeme_to_var<long>(token, TokenType::VarInt);
        break;

    case TokenType::VarReal:
        operand.val = convert_lexeme_to_double(token);
        break;

    case TokenType::VarId:
        operand.is_id = true;
        operand.val = std::string(token.lexeme);
        break;

    case TokenType::VarText:
        operand.val = std::string(token.lexeme);
        break;

    case TokenType::EndOfFile:
        throw Error(token, ErrorType::UnexpectedEOF);

    default:
        throw Error(token, ErrorType::VarSyntaxError);
    }
}

void parse_column_def(
        Lexer& lexer, std::vector<rdb::parser::ColumnDef>& column_def_seq)
{
    parse_token(lexer, TokenType::ParenthesisOpening);
    rdb::parser::ColumnDef column_def;
    std::vector<Token> token_seq;
    Token token;

    do {
        do {
            token = lexer.get();
            token_seq.push_back(token);
        } while ((token.type != TokenType::ParenthesisClosing)
                 && (token.type != TokenType::Comma)
                 && (token.type != TokenType::EndOfFile));

        if (token_seq.size() == 3) {
            if (token_seq[0].type == TokenType::VarId) {
                if ((token_seq[1].type == TokenType::KwInt)
                    || (token_seq[1].type == TokenType::KwReal)
                    || (token_seq[1].type == TokenType::KwText)) {
                    column_def.column_name = token_seq[0].lexeme;
                    column_def.type_name = token_seq[1].type;
                    column_def_seq.push_back(column_def);
                } else {
                    throw Error(token_seq[1], ErrorType::TypeSyntaxError);
                }
            } else {
                throw Error(
                        token_seq[0], ErrorType::SyntaxError, TokenType::VarId);
            }
        } else {
            throw Error(token, ErrorType::WrongListDefinition);
        }

        token_seq.clear();
    } while ((token.type != TokenType::ParenthesisClosing)
             && (token.type != TokenType::EndOfFile));
    if (token.type == TokenType::EndOfFile) {
        throw Error(token, ErrorType::UnexpectedEOF);
    }
}

void parse_column_list(Lexer& lexer, std::vector<std::string>& column_name_seq)
{
    do {
        std::string column_name;
        column_name = parse_token(lexer, TokenType::VarId);
        column_name_seq.push_back(column_name);
    } while (lexer.peek().type == TokenType::VarId);
}

void parse_argument_table(Lexer& lexer, std::string& table_name)
{
    parse_token(lexer, TokenType::KwTable);
    table_name = parse_token(lexer, TokenType::VarId);
}

void parse_argument_into(
        Lexer& lexer,
        std::string& table_name,
        std::vector<std::string>& column_name_seq)
{
    parse_token(lexer, TokenType::KwInto);
    table_name = parse_token(lexer, TokenType::VarId);

    parse_token(lexer, TokenType::ParenthesisOpening);
    std::vector<Token> token_seq;
    Token token;

    do {
        do {
            token = lexer.get();
            token_seq.push_back(token);
        } while ((token.type != TokenType::ParenthesisClosing)
                 && (token.type != TokenType::Comma)
                 && (token.type != TokenType::EndOfFile));

        if (token_seq.size() == 2) {
            if (token_seq[0].type == TokenType::VarId) {
                column_name_seq.emplace_back(std::string(token_seq[0].lexeme));
            } else {
                throw Error(token, ErrorType::SyntaxError, TokenType::VarId);
            }
        } else {
            throw Error(token, ErrorType::WrongListDefinition);
        }

        token_seq.clear();
    } while ((token.type != TokenType::ParenthesisClosing)
             && (token.type != TokenType::EndOfFile));
    if (token.type == TokenType::EndOfFile) {
        throw Error(token, ErrorType::UnexpectedEOF);
    }
}

void parse_argument_values(
        Lexer& lexer, std::vector<rdb::parser::Value>& value_seq)
{
    parse_token(lexer, TokenType::KwValues);
    parse_token(lexer, TokenType::ParenthesisOpening);
    std::vector<Token> token_seq;
    Token token;

    do {
        do {
            token = lexer.get();
            token_seq.push_back(token);
        } while ((token.type != TokenType::ParenthesisClosing)
                 && (token.type != TokenType::Comma)
                 && (token.type != TokenType::EndOfFile));

        if (token_seq.size() == 2) {
            rdb::parser::Value value;
            switch (token_seq[0].type) {
            case TokenType::VarInt:
                value = convert_lexeme_to_var<long>(
                        token_seq[0], TokenType::VarInt);
                value_seq.push_back(value);
                break;

            case TokenType::VarReal:
                value = convert_lexeme_to_double(token_seq[0]);
                value_seq.push_back(value);
                break;

            case TokenType::VarText:
                value = std::string(token_seq[0].lexeme);
                value_seq.push_back(value);
                break;

            default:
                throw Error(token, ErrorType::VarSyntaxError);
            }
        } else {
            throw Error(token, ErrorType::WrongListDefinition);
        }

        token_seq.clear();
    } while ((token.type != TokenType::ParenthesisClosing)
             && (token.type != TokenType::EndOfFile));
    if (token.type == TokenType::EndOfFile) {
        throw Error(token, ErrorType::UnexpectedEOF);
    }
}

void parse_argument_where(Lexer& lexer, rdb::parser::Expression& expression)
{
    parse_token(lexer, TokenType::KwWhere);
    parse_operand(lexer, expression.loperand);
    expression.operation = parse_token(lexer, TokenType::Operation);
    parse_operand(lexer, expression.roperand);
}

void parse_argument_from(
        Lexer& lexer,
        std::string& table_name,
        rdb::parser::Expression& expression)
{
    parse_token(lexer, TokenType::KwFrom);
    table_name = parse_token(lexer, TokenType::VarId);

    if (lexer.peek().type == TokenType::KwWhere) {
        parse_argument_where(lexer, expression);
    }
}

std::unique_ptr<SqlStatement> parse_statement_create(Lexer& lexer)
{
    std::string table_name;
    std::vector<rdb::parser::ColumnDef> column_def_seq;

    parse_token(lexer, TokenType::KwCreate);
    parse_argument_table(lexer, table_name);
    parse_column_def(lexer, column_def_seq);
    parse_token(lexer, TokenType::Semicolon);

    std::unique_ptr<rdb::parser::CreateTableStatement> create_table_statement
            = std::make_unique<rdb::parser::CreateTableStatement>(
                    std::move(table_name), std::move(column_def_seq));
    return std::unique_ptr<SqlStatement>(create_table_statement.release());
}

std::unique_ptr<SqlStatement> parse_statement_insert(Lexer& lexer)
{
    std::string table_name;
    std::vector<std::string> column_name_seq;
    std::vector<rdb::parser::Value> value_seq;

    parse_token(lexer, TokenType::KwInsert);
    parse_argument_into(lexer, table_name, column_name_seq);
    parse_argument_values(lexer, value_seq);
    parse_token(lexer, TokenType::Semicolon);

    std::unique_ptr<rdb::parser::InsertStatement> insert_statement
            = std::make_unique<rdb::parser::InsertStatement>(
                    std::move(table_name),
                    std::move(column_name_seq),
                    std::move(value_seq));
    return std::unique_ptr<SqlStatement>(insert_statement.release());
}

std::unique_ptr<SqlStatement> parse_statement_select(Lexer& lexer)
{
    std::vector<std::string> column_name_seq;
    std::string table_name;
    rdb::parser::Expression expression{0, "N", 0};

    parse_token(lexer, TokenType::KwSelect);
    parse_column_list(lexer, column_name_seq);
    parse_argument_from(lexer, table_name, expression);
    parse_token(lexer, TokenType::Semicolon);

    std::unique_ptr<rdb::parser::SelectStatement> select_statement
            = std::make_unique<rdb::parser::SelectStatement>(
                    std::move(table_name),
                    std::move(column_name_seq),
                    expression);
    return std::unique_ptr<SqlStatement>(select_statement.release());
}

std::unique_ptr<SqlStatement> parse_statement_delete(Lexer& lexer)
{
    std::string table_name;
    rdb::parser::Expression expression{0, "N", 0};

    parse_token(lexer, TokenType::KwDelete);
    parse_argument_from(lexer, table_name, expression);
    parse_token(lexer, TokenType::Semicolon);

    std::unique_ptr<rdb::parser::DeleteFromStatement> delete_statement
            = std::make_unique<rdb::parser::DeleteFromStatement>(
                    std::move(table_name), expression);
    return std::unique_ptr<SqlStatement>(delete_statement.release());
}

std::unique_ptr<SqlStatement> parse_statement_drop(Lexer& lexer)
{
    std::string table_name;

    parse_token(lexer, TokenType::KwDrop);
    parse_argument_table(lexer, table_name);
    parse_token(lexer, TokenType::Semicolon);

    std::unique_ptr<rdb::parser::DropTableStatement> drop_statement
            = std::make_unique<rdb::parser::DropTableStatement>(
                    std::move(table_name));
    return std::unique_ptr<SqlStatement>(drop_statement.release());
}

ParseResult rdb::parser::parse_sql(std::string_view sql_inquiry)
{
    Lexer lexer(sql_inquiry);
    ParseResult sql;
    Token token;

    token = lexer.peek();
    try {
        while (token.type != TokenType::EndOfFile) {
            try {
                switch (token.type) {
                case TokenType::KwCreate:
                    sql._sql_script._sql_statements.emplace_back(
                            parse_statement_create(lexer));
                    break;

                case TokenType::KwDelete:
                    sql._sql_script._sql_statements.emplace_back(
                            parse_statement_delete(lexer));
                    break;

                case TokenType::KwInsert:
                    sql._sql_script._sql_statements.emplace_back(
                            parse_statement_insert(lexer));
                    break;

                case TokenType::KwSelect:
                    sql._sql_script._sql_statements.emplace_back(
                            parse_statement_select(lexer));
                    break;

                case TokenType::KwDrop:
                    sql._sql_script._sql_statements.emplace_back(
                            parse_statement_drop(lexer));
                    break;

                default:
                    throw Error(token, ErrorType::NotStatement);
                }
            } catch (const Error& error) {
                switch (error.type()) {
                case ErrorType::UnexpectedEOF:
                    throw error;

                default:
                    sql._errors.push_back(error);
                    if (error.token_type() != TokenType::Semicolon) {
                        do {
                            token = lexer.get();
                        } while ((token.type != TokenType::Semicolon)
                                 && (token.type != TokenType::EndOfFile));
                        if (token.type == TokenType::EndOfFile) {
                            throw Error(token, ErrorType::UnexpectedEOF);
                        }
                    }
                }
            }

            token = lexer.peek();
        }
    } catch (const Error& error) {
        sql._errors.push_back(error);
    }

    return sql;
}