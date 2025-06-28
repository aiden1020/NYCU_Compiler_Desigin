#ifndef SEMA_SYMBOL_ENTRY_H
#define SEMA_SYMBOL_ENTRY_H

#include "AST/PType.hpp"
#include "AST/PType.hpp"
#include "AST/utils.hpp"
#include "AST/constant.hpp"
#include <string>

class SymbolEntry
{
public:
    enum class SymbolKind
    {
        kProgram,
        kFunction,
        kParameter,
        kVariable,
        kLoopVar,
        kConstant
    };
    static const char *kindToString(SymbolKind kind)
    {
        switch (kind)
        {
        case SymbolKind::kProgram:
            return "program";
        case SymbolKind::kFunction:
            return "function";
        case SymbolKind::kParameter:
            return "parameter";
        case SymbolKind::kVariable:
            return "variable";
        case SymbolKind::kLoopVar:
            return "loop_var";
        case SymbolKind::kConstant:
            return "constant";
        default:
            return "unknown";
        }
    }
    // constant
    SymbolEntry(const Location location, const char *const name, SymbolKind kind,
                int level, PTypeSharedPtr type, const Constant *const constant);
    // function
    SymbolEntry(const Location location, const char *const name, SymbolKind kind,
                int level, PTypeSharedPtr type, const std::string &attribute);
    ~SymbolEntry() = default;
    const std::string getName() const
    {
        return m_id_info.id;
    }
    const Location &getLocation() const
    {
        return m_id_info.location;
    }
    const int getLevel() const
    {
        return m_level;
    }
    const SymbolKind getKind() const
    {
        return m_kind;
    }
    const PTypeSharedPtr &getType() const
    {
        return m_type;
    }
    const Constant *getConstantAttribute() const
    {
        return m_constant;
    }
    const std::string &getParameterAttribute() const
    {
        return m_attribute;
    }

    void dumpEntry() const
    {
        std::string level_str = (m_level == 0)
                                    ? "0(global)"
                                    : std::to_string(m_level) + "(local)";
        const char *attr_cstr = "";
        if (m_kind == SymbolKind::kConstant)
        {
            attr_cstr = m_constant ? m_constant->getConstantValueCString() : "";
        }
        else if (m_kind == SymbolKind::kFunction)
        {
            attr_cstr = m_attribute.c_str();
        }
        else
        {
            attr_cstr = "";
        }

        printf("%-33s%-11s%-11s%-17s%-11s\n",
               m_id_info.id.c_str(),
               kindToString(m_kind),
               level_str.c_str(),
               m_type->getPTypeCString(),
               attr_cstr);
    }
    void setInvalid(bool is_invalid)
    {
        m_is_invalid = is_invalid;
    }

    bool isInvalid() const
    {
        return m_is_invalid;
    }

private:
    // Variable names
    IdInfo m_id_info;
    // Kind
    SymbolKind m_kind;
    // Level
    int m_level;
    // Type
    PTypeSharedPtr m_type;
    // Attribute
    //      Constant
    const Constant *m_constant;
    //      Attribute
    std::string m_attribute;
    bool m_is_invalid = false;
};

#endif