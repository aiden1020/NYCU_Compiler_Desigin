#include "sema/SymbolEntry.hpp"
#include <iostream>
SymbolEntry::SymbolEntry(const Location location, const char *const name, SymbolKind kind,
                         int level, PTypeSharedPtr type, const Constant *const constant)
    : m_id_info(location.line, location.col, name),
      m_kind(kind),
      m_level(level),
      m_type(type),
      m_constant(constant) {}
SymbolEntry::SymbolEntry(const Location location, const char *const name, SymbolKind kind,
                         int level, PTypeSharedPtr type, const std::string &attribute)
    : m_id_info(location.line, location.col, name),
      m_kind(kind),
      m_level(level),
      m_type(type),
      m_attribute(attribute) {}