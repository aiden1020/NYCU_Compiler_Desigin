#ifndef SEMA_SYMBOL_TABLE_H
#define SEMA_SYMBOL_TABLE_H

#include <vector>
#include "sema/SymbolEntry.hpp"
class SymbolTable
{
public:
    ~SymbolTable() = default;
    SymbolTable(int m_level, SymbolTable *parent = nullptr) : level(m_level), parent_table(parent) {}
    SymbolEntry &addSymbol(const Location location, const char *const name, SymbolEntry::SymbolKind kind,
                           int level, PTypeSharedPtr type, const Constant *const constant);
    SymbolEntry &addSymbol(const Location location, const char *const name, SymbolEntry::SymbolKind kind,
                           int level, PTypeSharedPtr type, const std::string &attribute);
    SymbolEntry *lookupDecl(const char *const name);
    SymbolEntry *lookupLocal(const char *const name);
    // other methods
    void dumpDemarcation(const char chr)
    {
        for (size_t i = 0; i < 110; ++i)
        {
            printf("%c", chr);
        }
        puts("");
    }

    void dumpTable(void)
    {
        dumpDemarcation('=');
        printf("%-33s%-11s%-11s%-17s%-11s\n", "Name", "Kind", "Level", "Type",
               "Attribute");
        dumpDemarcation('-');
        for (const auto &entry : entries)
        {
            entry.dumpEntry();
        }
        dumpDemarcation('-');
    }
    int getLevel() const
    {
        return level;
    }
    SymbolTable *getParentTable() const
    {
        return parent_table;
    }

private:
    // keep the order by simply using vector
    std::vector<SymbolEntry> entries;
    int level;
    SymbolTable *parent_table;
};

#endif