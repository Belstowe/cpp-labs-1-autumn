#include "Parser.hpp"
#include <charconv>
#include <stdexcept>

using rdb::parser::Parser;

std::ostream&
rdb::parser::operator<<(std::ostream& os, const rdb::parser::SqlScript& sql)
{
    for (auto&& statement_ptr : sql._sql_statements) {
        auto* statement = statement_ptr.get();
        os << *statement << "\n";
    }
    return os;
}

Parser::Parser(Lexer& lexer) : _lexer{lexer}
{
}

std::string Parser::parse_token(TokenType&& expected_token)
{
    Token token = _lexer.get();
    if (token.type != expected_token) {
        if (token.type == TokenType::EndOfFile) {
            throw Error(token, ErrorType::UnexpectedEOF, expected_token);
        }
        throw Error(token, ErrorType::SyntaxError, expected_token);
    }

    return std::string(token.lexeme);
}

template <typename T>
rdb::parser::Value Parser::convert_lexeme_to_var(
        Token& token, TokenType&& token_type)
{
    T result{};
    auto [ptr, ec]{std::from_chars(
            token.lexeme.data(),
            token.lexeme.data() + token.lexeme.size(),
            result)};
    if (ec == std::errc::result_out_of_range) {
        throw Error(token, ErrorType::VarOutOfRange, token_type);
    } else if (ec == std::errc::invalid_argument) {
        throw Error(token, ErrorType::IncorrectVarType, token_type);
    }

    return result;
}

rdb::parser::Value Parser::convert_lexeme_to_double(Token& token)
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

void Parser::parse_operand(Operand& operand)
{
    Token token = _lexer.get();
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

void Parser::parse_column_def(std::vector<ColumnDef>& column_def_seq)
{
    parse_token(TokenType::ParenthesisOpening);
    ColumnDef column_def;
    std::vector<Token> token_seq;
    Token token;
    bool catched_error = false;
    do {
        do {
            token = _lexer.get();
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
                    _sql._errors.emplace_back(
                            Error(token_seq[1], ErrorType::TypeSyntaxError));
                    catched_error = true;
                }
            } else {
                _sql._errors.emplace_back(
                        Error(token_seq[0],
                              ErrorType::SyntaxError,
                              TokenType::VarId));
                catched_error = true;
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
    if (catched_error) {
        throw Error(token, ErrorType::WrongListDefinition);
    }
}

void Parser::parse_column_list(std::vector<std::string>& column_name_seq)
{
    do {
        std::string column_name;
        column_name = parse_token(TokenType::VarId);
        column_name_seq.push_back(column_name);
    } while (_lexer.peek().type == TokenType::VarId);
}

void Parser::parse_argument_table(std::string& table_name)
{
    parse_token(TokenType::KwTable);
    table_name = parse_token(TokenType::VarId);
}

void Parser::parse_argument_into(
        std::string& table_name, std::vector<std::string>& column_name_seq)
{
    parse_token(TokenType::KwInto);
    table_name = parse_token(TokenType::VarId);

    parse_token(TokenType::ParenthesisOpening);
    std::vector<Token> token_seq;
    Token token;
    bool catched_error = false;
    do {
        do {
            token = _lexer.get();
            token_seq.push_back(token);
        } while ((token.type != TokenType::ParenthesisClosing)
                 && (token.type != TokenType::Comma)
                 && (token.type != TokenType::EndOfFile));

        if (token_seq.size() == 2) {
            if (token_seq[0].type == TokenType::VarId) {
                column_name_seq.emplace_back(std::string(token_seq[0].lexeme));
            } else {
                _sql._errors.emplace_back(
                        Error(token, ErrorType::SyntaxError, TokenType::VarId));
                catched_error = true;
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
    if (catched_error) {
        throw Error(token, ErrorType::WrongListDefinition);
    }
}

void Parser::parse_argument_values(std::vector<Value>& value_seq)
{
    parse_token(TokenType::KwValues);
    parse_token(TokenType::ParenthesisOpening);
    std::vector<Token> token_seq;
    Token token;
    bool catched_error = false;
    do {
        do {
            token = _lexer.get();
            token_seq.push_back(token);
        } while ((token.type != TokenType::ParenthesisClosing)
                 && (token.type != TokenType::Comma)
                 && (token.type != TokenType::EndOfFile));

        if (token_seq.size() == 2) {
            Value value;
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
                _sql._errors.emplace_back(
                        Error(token, ErrorType::VarSyntaxError));
                catched_error = true;
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
    if (catched_error) {
        throw Error(token, ErrorType::WrongListDefinition);
    }
}

void Parser::parse_argument_where(Expression& expression)
{
    parse_token(TokenType::KwWhere);
    parse_operand(expression.loperand);
    expression.operation = parse_token(TokenType::Operation);
    parse_operand(expression.roperand);
}

void Parser::parse_argument_from(
        std::string& table_name, Expression& expression)
{
    parse_token(TokenType::KwFrom);
    table_name = parse_token(TokenType::VarId);

    if (_lexer.peek().type == TokenType::KwWhere) {
        parse_argument_where(expression);
    }
}

void Parser::parse_statement_create()
{
    std::string table_name;
    std::vector<ColumnDef> column_def_seq;

    parse_token(TokenType::KwCreate);
    parse_argument_table(table_name);
    parse_column_def(column_def_seq);
    parse_token(TokenType::Semicolon);

    std::unique_ptr<CreateTableStatement> create_table_statement
            = std::make_unique<CreateTableStatement>(
                    std::move(table_name), std::move(column_def_seq));
    _sql._sql_script._sql_statements.push_back(
            std::unique_ptr<SqlStatement>(create_table_statement.release()));
}

void Parser::parse_statement_insert()
{
    std::string table_name;
    std::vector<std::string> column_name_seq;
    std::vector<Value> value_seq;

    parse_token(TokenType::KwInsert);
    parse_argument_into(table_name, column_name_seq);
    parse_argument_values(value_seq);
    parse_token(TokenType::Semicolon);

    std::unique_ptr<InsertStatement> insert_statement
            = std::make_unique<InsertStatement>(
                    std::move(table_name),
                    std::move(column_name_seq),
                    std::move(value_seq));
    _sql._sql_script._sql_statements.push_back(
            std::unique_ptr<SqlStatement>(insert_statement.release()));
}

void Parser::parse_statement_select()
{
    std::vector<std::string> column_name_seq;
    std::string table_name;
    Expression expression{0, "N", 0};

    parse_token(TokenType::KwSelect);
    parse_column_list(column_name_seq);
    parse_argument_from(table_name, expression);
    parse_token(TokenType::Semicolon);

    std::unique_ptr<SelectStatement> select_statement
            = std::make_unique<SelectStatement>(
                    std::move(table_name),
                    std::move(column_name_seq),
                    expression);
    _sql._sql_script._sql_statements.push_back(
            std::unique_ptr<SqlStatement>(select_statement.release()));
}

void Parser::parse_statement_delete()
{
    std::string table_name;
    Expression expression{0, "N", 0};

    parse_token(TokenType::KwDelete);
    parse_argument_from(table_name, expression);
    parse_token(TokenType::Semicolon);

    std::unique_ptr<DeleteFromStatement> delete_statement
            = std::make_unique<DeleteFromStatement>(
                    std::move(table_name), expression);
    _sql._sql_script._sql_statements.push_back(
            std::unique_ptr<SqlStatement>(delete_statement.release()));
}

void Parser::parse_statement_drop()
{
    std::string table_name;

    parse_token(TokenType::KwDrop);
    parse_argument_table(table_name);
    parse_token(TokenType::Semicolon);

    std::unique_ptr<DropTableStatement> drop_statement
            = std::make_unique<DropTableStatement>(std::move(table_name));
    _sql._sql_script._sql_statements.push_back(
            std::unique_ptr<SqlStatement>(drop_statement.release()));
}

void Parser::parse_sql(ParseResult& sql)
{
    Token token;

    token = _lexer.peek();
    try {
        while (token.type != TokenType::EndOfFile) {
            try {
                switch (token.type) {
                case TokenType::KwCreate:
                    parse_statement_create();
                    break;

                case TokenType::KwDelete:
                    parse_statement_delete();
                    break;

                case TokenType::KwInsert:
                    parse_statement_insert();
                    break;

                case TokenType::KwSelect:
                    parse_statement_select();
                    break;

                case TokenType::KwDrop:
                    parse_statement_drop();
                    break;

                default:
                    throw Error(token, ErrorType::NotStatement);
                }
            } catch (const Error& error) {
                switch (error.type()) {
                case ErrorType::UnexpectedEOF:
                    throw error;

                default:
                    _sql._errors.push_back(error);
                    if (error.token_type() != TokenType::Semicolon) {
                        do {
                            token = _lexer.get();
                        } while ((token.type != TokenType::Semicolon)
                                 && (token.type != TokenType::EndOfFile));
                        if (token.type == TokenType::EndOfFile) {
                            throw Error(token, ErrorType::UnexpectedEOF);
                        }
                    }
                }
            }

            token = _lexer.peek();
        }
    } catch (const Error& error) {
        _sql._errors.push_back(error);
    }

    std::move(
            _sql._sql_script._sql_statements.begin(),
            _sql._sql_script._sql_statements.end(),
            std::back_inserter(sql._sql_script._sql_statements));
    _sql._sql_script._sql_statements.clear();
    sql._errors = _sql._errors;
    _sql._errors.clear();
}