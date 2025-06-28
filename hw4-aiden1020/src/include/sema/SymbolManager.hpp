#ifndef SEMA_SYMBOL_MANAGER_H
#define SEMA_SYMBOL_MANAGER_H

#include <stack>
#include "sema/SymbolTable.hpp"
class SymbolManager
{
public:
    SymbolManager() : current_level(0)
    {
    }
    ~SymbolManager() = default;
    void pushScope(SymbolTable *current_table = nullptr)
    {
        tables.push(new SymbolTable(current_level, current_table));
        current_level++;
    }
    void popScope(bool dump_table)
    {
        if (dump_table && !tables.empty())
        {
            tables.top()->dumpTable();
        }
        delete tables.top();
        tables.pop();
        current_level--;
    }
    SymbolTable *getCurrentTable()
    {
        return tables.top();
    }
    int getCurrentLevel()
    {
        return current_level;
    }
    // other methods
private:
    std::stack<SymbolTable *> tables;
    int current_level;
};
#endif