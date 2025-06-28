#include "sema/SymbolTable.hpp"
SymbolEntry &SymbolTable::addSymbol(const Location location, const char *const name, SymbolEntry::SymbolKind kind,
                                    int level, PTypeSharedPtr type, const Constant *const constant)
{

    SymbolEntry entry(location, name, kind, level, type, constant);
    entries.push_back(entry);
    return entries.back();
}
SymbolEntry &SymbolTable::addSymbol(const Location location, const char *const name, SymbolEntry::SymbolKind kind,
                                    int level, PTypeSharedPtr type, const std::string &attribute)
{
    SymbolEntry entry(location, name, kind, level, type, attribute);
    entries.push_back(entry);
    return entries.back();
}
SymbolEntry *SymbolTable::lookupDecl(const char *const name)
{
    for (auto &entry : entries)
    {
        // printf("lookupDecl: %s\n", entry.getName().c_str());
        if (entry.getName() == name)
        {
            return &entry;
        }
    }
    if (parent_table != nullptr)
    {
        return parent_table->lookupDecl(name);
    }
    return nullptr;
}
SymbolEntry *SymbolTable::lookupLocal(const char *const name)
{
    for (auto &entry : entries)
    {
        // printf("lookupDecl: %s\n", entry.getName().c_str());
        if (entry.getName() == name)
        {
            return &entry;
        }
    }
    return nullptr;
}
