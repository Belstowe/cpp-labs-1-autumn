#include "Parser.hpp"
#include <charconv>

using namespace rdb::parser;

std::ostream& rdb::parser::operator<<(std::ostream& os, const rdb::parser::SqlScript& sql) {
    for (auto&& statement_ptr : sql._sql_statements) {
        auto statement = statement_ptr.get();
        os << *statement << "\n";
    }
    return os;
}

std::ostream& rdb::parser::operator<<(std::ostream& os, const rdb::parser::ParseResult& parse_result) {
    os << parse_result._sql_script << "\n";
    for (auto&& error: parse_result._errors) {
        os << "\n" << error;
    }
    return os;
}

Parser::Parser(Lexer& lexer)
    : _lexer{lexer}
{
}

void Parser::parse_token(const TokenType expected_token, std::string *value, bool& catched_error) {
    Token token = _lexer.get();
    if (token.type != expected_token) {
        if (token.type == TokenType::EndOfFile) {
            throw Error(token, ErrorType::UnexpectedEOF, expected_token);
        }
        _sql._errors.push_back(Error(token, ErrorType::SyntaxError, expected_token));
        catched_error = true;
    }
    else if (value != NULL) {
        *value = token.lexeme;
    }
}

template<typename T>
void Parser::convert_lexeme_to_var(Token &token, Value &value, const TokenType &token_type, bool& catched_error) {
    T result{};
    auto [ptr, ec] { std::from_chars(token.lexeme.data(), token.lexeme.data() + token.lexeme.size(), result) };
    if (ec == std::errc()) {
        value = result;
    }
    else if (ec == std::errc::result_out_of_range) {
        _sql._errors.push_back(Error(token, ErrorType::VarOutOfRange, token_type));
        catched_error = true;
    }
    else {
        _sql._errors.push_back(Error(token, ErrorType::IncorrectVarType, token_type));
        catched_error = true;
    }
}

void Parser::parse_operand(Operand& operand, bool& catched_error) {
    Token token = _lexer.get();
    operand.is_id = false;

    switch (token.type) {
        case TokenType::VarInt:
            convert_lexeme_to_var<long>(token, operand.val, token.type, catched_error);
            break;

        case TokenType::VarReal:
            convert_lexeme_to_var<double>(token, operand.val, token.type, catched_error);
            break;

        case TokenType::VarId:
            operand.is_id = true;
        case TokenType::VarText:
            operand.val = std::string(token.lexeme);
            break;

        case TokenType::EndOfFile:
            throw Error(token, ErrorType::UnexpectedEOF);

        default:
            _sql._errors.push_back(Error(token, ErrorType::VarSyntaxError));
            catched_error = true;
    }
}

void Parser::parse_column_def(std::vector<ColumnDef> &column_def_seq, bool& catched_error) {
    bool no_parenthesis_opening = false;
    parse_token(TokenType::ParenthesisOpening, NULL, no_parenthesis_opening);
    if (no_parenthesis_opening) {
        catched_error = no_parenthesis_opening;
        return;
    }

    ColumnDef column_def;
    std::vector<Token> token_seq;
    Token token;
    do {
        do {
            token = _lexer.get();
            token_seq.push_back(token);
        } while ((token.type != TokenType::ParenthesisClosing) && (token.type != TokenType::Comma) && (token.type != TokenType::EndOfFile));

        if (token_seq.size() == 3) {
            if (token_seq[0].type == TokenType::VarId) {
                if ((token_seq[1].type == TokenType::KwInt) || (token_seq[1].type == TokenType::KwReal) || (token_seq[1].type == TokenType::KwText)) {
                    column_def.column_name = token_seq[0].lexeme;
                    column_def.type_name = token_seq[1].type;
                    column_def_seq.push_back(column_def);
                }
                else {
                    _sql._errors.push_back(Error(token_seq[1], ErrorType::TypeSyntaxError));
                    catched_error = true;
                }
            }
            else {
                _sql._errors.push_back(Error(token_seq[0], ErrorType::SyntaxError, TokenType::VarId));
                catched_error = true;
            }
        }
        else {
            _sql._errors.push_back(Error(token, ErrorType::WrongListDefinition));
            catched_error = true;
        }

        token_seq.clear();
    } while ((token.type != TokenType::ParenthesisClosing) && (token.type != TokenType::EndOfFile));
    if (token.type == TokenType::EndOfFile) {
        throw Error(token, ErrorType::UnexpectedEOF);
    }
}

void Parser::parse_column_list(std::vector<std::string>& column_name_seq, bool& catched_error) {
    do {
        std::string column_name;
        parse_token(TokenType::VarId, &column_name, catched_error);
        column_name_seq.push_back(column_name);
    } while (_lexer.peek().type == TokenType::VarId);
}

void Parser::parse_argument_table(std::string& table_name, bool& catched_error) {
    parse_token(TokenType::KwTable, NULL, catched_error);
    parse_token(TokenType::VarId, &table_name, catched_error);
}

void Parser::parse_argument_into(std::string& table_name, std::vector<std::string>& column_name_seq, bool& catched_error) {
    parse_token(TokenType::KwInto, NULL, catched_error);
    parse_token(TokenType::VarId, &table_name, catched_error);

    parse_token(TokenType::ParenthesisOpening, NULL, catched_error);
    std::vector<Token> token_seq;
    Token token;
    do {
        do {
            token = _lexer.get();
            token_seq.push_back(token);
        } while ((token.type != TokenType::ParenthesisClosing) && (token.type != TokenType::Comma) && (token.type != TokenType::EndOfFile));

        if (token_seq.size() == 2) {
            if (token_seq[0].type == TokenType::VarId) {
                column_name_seq.push_back(std::string(token_seq[0].lexeme));
            }
            else {
                _sql._errors.push_back(Error(token, ErrorType::SyntaxError, TokenType::VarId));
                catched_error = true;
            }
        }
        else {
            _sql._errors.push_back(Error(token, ErrorType::WrongListDefinition));
            catched_error = true;
        }

        token_seq.clear();
    } while ((token.type != TokenType::ParenthesisClosing) && (token.type != TokenType::EndOfFile));
    if (token.type == TokenType::EndOfFile) {
        throw Error(token, ErrorType::UnexpectedEOF);
    }
}

void Parser::parse_argument_values(std::vector<Value>& value_seq, bool& catched_error) {
    parse_token(TokenType::KwValues, NULL, catched_error);
    parse_token(TokenType::ParenthesisOpening, NULL, catched_error);
    std::vector<Token> token_seq;
    Token token;
    do {
        do {
            token = _lexer.get();
            token_seq.push_back(token);
        } while ((token.type != TokenType::ParenthesisClosing) && (token.type != TokenType::Comma) && (token.type != TokenType::EndOfFile));

        if (token_seq.size() == 2) {
            Value value;
            switch (token_seq[0].type) {
                case TokenType::VarInt:
                    convert_lexeme_to_var<long>(token_seq[0], value, token_seq[0].type, catched_error);
                    value_seq.push_back(value);
                    break;

                case TokenType::VarReal:
                    convert_lexeme_to_var<double>(token_seq[0], value, token_seq[0].type, catched_error);
                    value_seq.push_back(value);
                    break;

                case TokenType::VarText:
                    value = std::string(token_seq[0].lexeme);
                    value_seq.push_back(value);
                    break;

                default:
                    _sql._errors.push_back(Error(token, ErrorType::VarSyntaxError));
                    catched_error = true;
            }
        }
        else {
            _sql._errors.push_back(Error(token, ErrorType::WrongListDefinition));
            catched_error = true;
        }

        token_seq.clear();
    } while ((token.type != TokenType::ParenthesisClosing) && (token.type != TokenType::EndOfFile));
    if (token.type == TokenType::EndOfFile) {
        throw Error(token, ErrorType::UnexpectedEOF);
    }
}

void Parser::parse_argument_where(Expression& expression, bool& catched_error) {
    parse_token(TokenType::KwWhere, NULL, catched_error);
    parse_operand(expression.loperand, catched_error);
    parse_token(TokenType::Operation, &expression.operation, catched_error);
    parse_operand(expression.roperand, catched_error);
}

void Parser::parse_argument_from(std::string& table_name, Expression& expression, bool& catched_error) {
    parse_token(TokenType::KwFrom, NULL, catched_error);
    parse_token(TokenType::VarId, &table_name, catched_error);

    if (_lexer.peek().type == TokenType::KwWhere) {
        parse_argument_where(expression, catched_error);
    }
}

void Parser::parse_statement_create() {
    bool catched_error = false;
    std::string table_name;
    std::vector<ColumnDef> column_def_seq;

    parse_token(TokenType::KwCreate, NULL, catched_error);
    parse_argument_table(table_name, catched_error);
    parse_column_def(column_def_seq, catched_error);
    parse_token(TokenType::Semicolon, NULL, catched_error);

    if (!catched_error) {
        std::unique_ptr<CreateTableStatement> create_table_statement = std::make_unique<CreateTableStatement>(table_name, column_def_seq);
        _sql._sql_script._sql_statements.push_back(std::unique_ptr<SqlStatement>(create_table_statement.release()));
    }
}

void Parser::parse_statement_insert() {
    bool catched_error = false;
    std::string table_name;
    std::vector<std::string> column_name_seq;
    std::vector<Value> value_seq;

    parse_token(TokenType::KwInsert, NULL, catched_error);
    parse_argument_into(table_name, column_name_seq, catched_error);
    parse_argument_values(value_seq, catched_error);
    parse_token(TokenType::Semicolon, NULL, catched_error);

    if (!catched_error) {
        std::unique_ptr<InsertStatement> insert_statement = std::make_unique<InsertStatement>(table_name, column_name_seq, value_seq);
        _sql._sql_script._sql_statements.push_back(std::unique_ptr<SqlStatement>(insert_statement.release()));
    }
}

void Parser::parse_statement_select() {
    bool catched_error = false;
    std::vector<std::string> column_name_seq;
    std::string table_name;
    Expression expression{0, "N", 0};

    parse_token(TokenType::KwSelect, NULL, catched_error);
    parse_column_list(column_name_seq, catched_error);
    parse_argument_from(table_name, expression, catched_error);
    parse_token(TokenType::Semicolon, NULL, catched_error);

    if (!catched_error) {
        std::unique_ptr<SelectStatement> select_statement = std::make_unique<SelectStatement>(table_name, column_name_seq, expression);
        _sql._sql_script._sql_statements.push_back(std::unique_ptr<SqlStatement>(select_statement.release()));
    }
}

void Parser::parse_statement_delete() {
    bool catched_error = false;
    std::string table_name;
    Expression expression{0, "N", 0};

    parse_token(TokenType::KwDelete, NULL, catched_error);
    parse_argument_from(table_name, expression, catched_error);
    parse_token(TokenType::Semicolon, NULL, catched_error);

    if (!catched_error) {
        std::unique_ptr<DeleteFromStatement> delete_statement = std::make_unique<DeleteFromStatement>(table_name, expression);
        _sql._sql_script._sql_statements.push_back(std::unique_ptr<SqlStatement>(delete_statement.release()));
    }
}

void Parser::parse_statement_drop() {
    bool catched_error = false;
    std::string table_name;

    parse_token(TokenType::KwDrop, NULL, catched_error);
    parse_argument_table(table_name, catched_error);
    parse_token(TokenType::Semicolon, NULL, catched_error);

    if (!catched_error) {
        std::unique_ptr<DropTableStatement> drop_statement = std::make_unique<DropTableStatement>(table_name);
        _sql._sql_script._sql_statements.push_back(std::unique_ptr<SqlStatement>(drop_statement.release()));
    }
}

void Parser::parse_sql(ParseResult& sql) {
    Token token;

    token = _lexer.peek();
    try {
        while (token.type != TokenType::EndOfFile) {
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
                    token = _lexer.get();
                    _sql._errors.push_back(Error(token, ErrorType::NotStatement));
            }

            token = _lexer.peek();
        }
    }
    catch (const Error& error) {
        _sql._errors.push_back(error);
    }

    std::move(_sql._sql_script._sql_statements.begin(), _sql._sql_script._sql_statements.end(), std::back_inserter(sql._sql_script._sql_statements));
    _sql._sql_script._sql_statements.clear();
    sql._errors = _sql._errors;
    _sql._errors.clear();
}