#pragma once

#include <initializer_list>
#include <string>
#include <variant>
#include <vector>

namespace rdb::parser {
    using Value = std::variant<int, double, std::string>;

    struct Operand {
        bool is_var;
        Value val;
    };

    typedef struct _column_def {
        std::string column_name;
        std::string type_name;
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
            std::vector<ColumnDef> _column_def;
    };

    class InsertStatement : public SqlStatement {
        public:
            explicit InsertStatement(std::string, std::vector<std::string>, std::vector<Value>);

        private:
            std::string _table_name;
            std::vector<std::string> _column_name;
            std::vector<Value> _value;
    };

    class SelectStatement : public SqlStatement {
        public:
            explicit SelectStatement(std::string, std::string, Expression = Expression{0, "None", 0} );
        
        private:
            std::string _table_name;
            std::string _column_name;
            bool _has_expression_cond;
            Expression _expression;
    };

    class DeleteFromStatement : public SqlStatement {
        public:
            explicit DeleteFromStatement(std::string, Expression = Expression{0, "None", 0} );

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