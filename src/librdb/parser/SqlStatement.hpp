#pragma once

#include "librdb/rdbtoken.hpp"
#include <initializer_list>
#include <string>
#include <variant>
#include <vector>

namespace rdb::parser {
    using Value = std::variant<long, double, std::string>;

    struct Operand {
        bool is_id;
        Value val;
    };

    typedef struct _column_def {
        std::string column_name;
        TokenType type_name;
    } ColumnDef;

    typedef struct _expression {
        Operand loperand;
        std::string operation;
        Operand roperand;
    } Expression;

    class SqlStatement {
        public:
            virtual ~SqlStatement() = default;
    };

    class CreateTableStatement : public SqlStatement {
        public:
            explicit CreateTableStatement(std::string, std::vector<ColumnDef>);

        private:
            std::string _table_name;
            std::vector<ColumnDef> _column_def_seq;
    };

    class InsertStatement : public SqlStatement {
        public:
            explicit InsertStatement(std::string, std::vector<std::string>, std::vector<Value>);

        private:
            std::string _table_name;
            std::vector<std::string> _column_name_seq;
            std::vector<Value> _value_seq;
    };

    class SelectStatement : public SqlStatement {
        public:
            explicit SelectStatement(std::string, std::string, Expression = Expression{0, "N", 0} );
        
        private:
            std::string _table_name;
            std::string _column_name;
            bool _has_expression_cond;
            Expression _expression;
    };

    class DeleteFromStatement : public SqlStatement {
        public:
            explicit DeleteFromStatement(std::string, Expression = Expression{0, "N", 0} );

        private:
            std::string _table_name;
            bool _has_expression_cond;
            Expression _expression;
    };

    class DropTableStatement : public SqlStatement {
        public:
            explicit DropTableStatement(std::string);
        
        private:
            std::string _table_name;
    };
} // namespace rdb::parser