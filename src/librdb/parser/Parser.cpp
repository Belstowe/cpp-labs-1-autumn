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

void Parser::parse_token(const TokenType expected_token, std::string *value, bool *catched_error, const bool is_required, TokenType *got_token) {
    Token token = _lexer.get();
    if (got_token != NULL) {
        *got_token = token.type;
    }
    if (token.type != expected_token) {
        if (is_required) {
            _sql._errors.push_back(Error(token, ErrorType::SyntaxError, expected_token));
        }
        if (catched_error != NULL) {
            *catched_error = true;
        }
    }
    else if (value != NULL) {
        *value = token.lexeme;
    }
}

template<typename T>
void Parser::convert_lexeme_to_var(Token &token, Value &value, const TokenType &token_type, bool *catched_error) {
    T result{};
    auto [ptr, ec] { std::from_chars(token.lexeme.data(), token.lexeme.data() + token.lexeme.size(), result) };
    if (ec == std::errc()) {
        value = result;
    }
    else if (ec == std::errc::result_out_of_range) {
        _sql._errors.push_back(Error(token, ErrorType::VarOutOfRange, token_type));
         if (catched_error != NULL) {
            *catched_error = true;
        }
    }
    else {
        _sql._errors.push_back(Error(token, ErrorType::IncorrectVarType, token_type));
        if (catched_error != NULL) {
             *catched_error = true;
        }
    }
}

void Parser::parse_value(Value &value, TokenType &token_type, bool *catched_error) {
    Token token = _lexer.get();
    token_type = token.type;

    switch (token_type) {
        case TokenType::VarInt:
            convert_lexeme_to_var<long>(token, value, token_type, catched_error);
            break;

        case TokenType::VarReal:
            convert_lexeme_to_var<double>(token, value, token_type, catched_error);
            break;

        case TokenType::VarText:
            value = std::string(token.lexeme);
            break;

        default:
            if (catched_error != NULL) {
                _sql._errors.push_back(Error(token, ErrorType::VarSyntaxError));
                *catched_error = true;
            }
    }
}

void Parser::parse_var_type(TokenType &token_type, bool *catched_error) {
    Token token = _lexer.get();
    token_type = token.type;
    if ((token_type != TokenType::KwInt) && (token_type != TokenType::KwReal) && (token_type != TokenType::KwText)) {
        _sql._errors.push_back(Error(token, ErrorType::TypeSyntaxError));
        if (catched_error != NULL) {
            *catched_error = true;
        }
    }
}

int Parser::parse_column_def(std::vector<ColumnDef> &column_def_seq, bool& catched_error) {
    bool no_parenthesis_opening = false;
    parse_token(TokenType::ParenthesisOpening, NULL, &no_parenthesis_opening, true);
    if (no_parenthesis_opening) {
        catched_error = no_parenthesis_opening;
        return -1;
    }

    ColumnDef column_def;
    std::vector<Token> token_seq;
    Token token;
    int s = 0;
    do {
        do {
            token = _lexer.get();
            token_seq.push_back(token);
        } while ((token.type != TokenType::ParenthesisClosing) && (token.type != TokenType::Comma));

        if (token_seq.size() == 3) {
            if (token_seq[0].type == TokenType::VarId) {
                if ((token_seq[1].type == TokenType::KwInt) || (token_seq[1].type == TokenType::KwReal) || (token_seq[1].type == TokenType::KwText)) {
                    column_def.column_name = token_seq[0].lexeme;
                    column_def.type_name = token_seq[1].type;
                    column_def_seq.push_back(column_def);
                    s++;
                }
                else {
                    _sql._errors.push_back(Error(token_seq[1], ErrorType::VarSyntaxError));
                }
            }
            else {
                _sql._errors.push_back(Error(token_seq[0], ErrorType::SyntaxError, TokenType::VarId));
            }
        }
        else {
            _sql._errors.push_back(Error(token, ErrorType::WrongColumnDefinition));
        }

        token_seq.clear();
    } while (token.type != TokenType::ParenthesisClosing);

    return s;
}

void Parser::parse_statement_create() {
    bool catched_error = false;
    std::string table_name;
    std::vector<ColumnDef> column_def_seq;

    parse_token(TokenType::KwCreate, NULL, &catched_error, true);
    parse_token(TokenType::KwTable, NULL, &catched_error, true);
    parse_token(TokenType::VarId, &table_name, &catched_error, true);
    parse_column_def(column_def_seq, catched_error);
    parse_token(TokenType::Semicolon, NULL, &catched_error, true);

    if (!catched_error) {
        std::unique_ptr<CreateTableStatement> create_table_statement = std::make_unique<CreateTableStatement>(table_name, column_def_seq);
        _sql._sql_script._sql_statements.push_back(std::unique_ptr<SqlStatement>(create_table_statement.release()));
    }
}

void Parser::parse_statement_delete() {
    // TBD
}

void Parser::parse_statement_insert() {
    // TBD
}

void Parser::parse_statement_select() {
    // TBD
}

void Parser::parse_statement_drop() {
    // TBD
}

void Parser::parse_sql(ParseResult& sql) {
    Token token;

    token = _lexer.peek();
    do {
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
    } while (token.type != TokenType::EndOfFile);

    std::move(_sql._sql_script._sql_statements.begin(), _sql._sql_script._sql_statements.end(), std::back_inserter(sql._sql_script._sql_statements));
    _sql._sql_script._sql_statements.clear();
    sql._errors = _sql._errors;
    _sql._errors.clear();
}