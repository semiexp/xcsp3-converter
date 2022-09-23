#pragma once

#include <algorithm>
#include <string>
#include <map>

namespace XCSP3Core {
    class Tree;
}

enum Type {
    kBool, kInt,
};

using TypedConvertedExpr = std::tuple<std::string, Type>;
using VariableMap = std::map<std::string, std::tuple<std::string, Type>>;

TypedConvertedExpr AsInt(const TypedConvertedExpr& ty_expr);
TypedConvertedExpr AsBool(const TypedConvertedExpr& ty_expr);
TypedConvertedExpr AsType(const TypedConvertedExpr& ty_expr, Type expected_type);

TypedConvertedExpr ConvertTree(const XCSP3Core::Tree* tree, const VariableMap& var_map);
