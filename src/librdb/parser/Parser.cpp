#include "Parser.hpp"
#include <charconv>

using namespace rdb::parser;

Error::Error(Token token, ErrorType type, TokenType expected)
    : _token{token}, _type{type}, _expected{expected}
{
}

Parser::Parser(Lexer& lexer)
    : _lexer{lexer}
{
}

void Parser::parse_token(const TokenType expected_token, std::string *value, bool *catched_error, const bool is_required, TokenType *got_token) {
    Token token = _lexer.get();
    *got_token = token.type;
    if (token.type != expected_token) {
        if (is_required) {
            _sql._errors.push_back(Error(token, ErrorType::SyntaxError, expected_token));
        }
        if (catched_error != NULL) {
            *catched_error = true;
        }
    }
    else if (value != NULL) {
        *value = token.lexeme.data();
    }
}

void Parser::parse_value(Value &value, TokenType &token_type, bool *catched_error) {
    Token token = _lexer.get();
    token_type = token.type;

    switch (token_type) {
        case TokenType::VarInt:
            long result;
            auto [ptr, ec] { std::from_chars(token.lexeme.data(), token.lexeme.data() + token.lexeme.size(), result) };
            switch (ec) {
                case std::errc():
                    value = result;
                case std::errc::result_out_of_range:
                    _sql._errors.push_back(Error(token, ErrorType::VarOutOfRange, TokenType::VarInt));
                    if (catched_error != NULL) {
                        *catched_error = true;
                    }
                default:
                    _sql._errors.push_back(Error(token, ErrorType::IncorrectVarType, TokenType::VarInt));
                    if (catched_error != NULL) {
                        *catched_error = true;
                    }
            }
            break;

        case TokenType::VarReal:
            double result;
            auto [ptr, ec] { std::from_chars(token.lexeme.data(), token.lexeme.data() + token.lexeme.size(), result) };
            switch (ec) {
                case std::errc():
                    value = result;
                case std::errc::result_out_of_range:
                    _sql._errors.push_back(Error(token, ErrorType::VarOutOfRange, TokenType::VarReal));
                    if (catched_error != NULL) {
                        *catched_error = true;
                    }
                default:
                    _sql._errors.push_back(Error(token, ErrorType::IncorrectVarType, TokenType::VarReal));
                    if (catched_error != NULL) {
                        *catched_error = true;
                    }
            }
            break;

        case TokenType::VarText:
            value = token.lexeme.data();
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
    if ((token.type != TokenType::KwInt) || (token.type != TokenType::KwReal) || (token.type != TokenType::KwText)) {
        _sql._errors.push_back(Error(token, ErrorType::TypeSyntaxError));
        if (catched_error != NULL) {
            *catched_error = true;
        }
    }
}

int Parser::parse_column_def(std::vector<ColumnDef> &column_def_seq, bool& catched_error) {
    bool no_parenthesis_opening = false;
    TokenType token_type;
    parse_token(TokenType::ParenthesisOpening, NULL, &no_parenthesis_opening, true, &token_type);
    if (!no_parenthesis_opening) {
        catched_error = no_parenthesis_opening;
        return -1;
    }

    ColumnDef column_def;
    size_t s = 0;
    do {
        switch (token_type) {
            case TokenType::Comma:
                column_def_seq.push_back(column_def);
                column_def.type_name = TokenType::Unknown;
                column_def.column_name = "";
                s++;
            case TokenType::ParenthesisOpening:
                parse_token(TokenType::VarId, &column_def.column_name, &catched_error, true, &token_type);
                break;
            case TokenType::VarId:
                parse_var_type(token_type, &catched_error);
                break;
            default:
                parse_token(TokenType::Comma, NULL, NULL, false, &token_type);
        }
    } while ((token_type != TokenType::ParenthesisClosing) || (token_type != TokenType::EndOfFile));
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
}

void Parser::parse_statement_delete() {
    bool catched_error = false;
}

void Parser::parse_statement_insert() {
    bool catched_error = false;
}

void Parser::parse_statement_select() {
    bool catched_error = false;
}

void Parser::parse_statement_drop() {
    bool catched_error = false;
}

struct ParseResult Parser::parse_sql() {
    Token token;

    do {
        token = _lexer.peek();

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
    } while (token.type != TokenType::EndOfFile);
}