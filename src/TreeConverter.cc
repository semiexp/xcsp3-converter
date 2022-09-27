#include "TreeConverter.h"

#include <cstdlib>
#include <iostream>

#include <XCSP3Tree.h>
#include <XCSP3TreeNode.h>

namespace {

using XCSP3Core::ExpressionType;

std::tuple<std::string, std::vector<Type>, Type> OperatorInfo(ExpressionType type, int n_arity) {
    switch (type) {
        case ExpressionType::ONEG:
            if (n_arity != 1) {
                std::cerr << "error: n_arity must be 1 for neg" << std::endl;
                abort();
            }
            return {"-", {Type::kInt}, Type::kInt};
        case ExpressionType::OADD:
            return {"+", std::vector<Type>(n_arity, Type::kInt), Type::kInt};
        case ExpressionType::OMUL:
            return {"*", std::vector<Type>(n_arity, Type::kInt), Type::kInt};
        case ExpressionType::OSUB:
            if (n_arity != 2) {
                std::cerr << "error: n_arity must be 2 for sub" << std::endl;
                abort();
            }
            return {"-", std::vector<Type>(n_arity, Type::kInt), Type::kInt};
        case ExpressionType::OIF:
            if (n_arity != 3) {
                std::cerr << "error: n_arity must be 3 for if" << std::endl;
                abort();
            }
            return {"if", {Type::kBool, Type::kInt, Type::kInt}, Type::kInt};
        case ExpressionType::OAND:
            return {"and", std::vector<Type>(n_arity, Type::kBool), Type::kBool};
        case ExpressionType::OOR:
            return {"or", std::vector<Type>(n_arity, Type::kBool), Type::kBool};
        case ExpressionType::OXOR:
            return {"xor", std::vector<Type>(n_arity, Type::kBool), Type::kBool};
        case ExpressionType::OEQ:
            if (n_arity < 2) {
                std::cerr << "error: n_arity must be at least 2 for eq" << std::endl;
                abort();
            }
            return {"==", std::vector<Type>(n_arity, Type::kInt), Type::kBool};
        case ExpressionType::ONE:
            if (n_arity != 2) {
                std::cerr << "error: n_arity must be 2 for ne" << std::endl;
                abort();
            }
            return {"!=", std::vector<Type>(n_arity, Type::kInt), Type::kBool};
        case ExpressionType::OLE:
            if (n_arity != 2) {
                std::cerr << "error: n_arity must be 2 for le" << std::endl;
                abort();
            }
            return {"<=", std::vector<Type>(n_arity, Type::kInt), Type::kBool};
        case ExpressionType::OLT:
            if (n_arity != 2) {
                std::cerr << "error: n_arity must be 2 for lt" << std::endl;
                abort();
            }
            return {"<", std::vector<Type>(n_arity, Type::kInt), Type::kBool};
        case ExpressionType::OGE:
            if (n_arity != 2) {
                std::cerr << "error: n_arity must be 2 for ge" << std::endl;
                abort();
            }
            return {">=", std::vector<Type>(n_arity, Type::kInt), Type::kBool};
        case ExpressionType::OGT:
            if (n_arity != 2) {
                std::cerr << "error: n_arity must be 2 for gt" << std::endl;
                abort();
            }
            return {">", std::vector<Type>(n_arity, Type::kInt), Type::kBool};
        default:
            std::cerr << "error: unknown operator type: " << type << " (" << XCSP3Core::operatorToString(type) << ")" << std::endl;
            abort();
    }
}

TypedConvertedExpr ConvertTreeImpl(const XCSP3Core::Node* node, const VariableMap& var_map) {
    if (auto n = dynamic_cast<const XCSP3Core::NodeConstant*>(node)) {
        int val = n->val;
        return {std::to_string(val), Type::kInt};
    }
    if (auto n = dynamic_cast<const XCSP3Core::NodeVariable*>(node)) {
        auto var_name = n->var;
        if (auto found = var_map.find(var_name); found != var_map.end()) {
            auto [converted_var_name, ty] = found->second;
            return {converted_var_name, ty};
        } else {
            std::cerr << "error: unknown variable: " << var_name << std::endl;
            abort();
        }
    }
    if (auto n = dynamic_cast<const XCSP3Core::NodeOperator*>(node)) {
        int n_arity = n->parameters.size();
        auto [op_name, input_types, output_type] = OperatorInfo(n->type, n_arity);

        std::string ret_expr = "(" + op_name;
        for (int i = 0; i < n_arity; ++i) {
            ret_expr.push_back(' ');
            ret_expr += std::get<0>(AsType(ConvertTreeImpl(n->parameters[i], var_map), input_types[i]));
        }
        ret_expr.push_back(')');
        return {ret_expr, output_type};
    }
    std::cerr << "error: unknown node" << std::endl;
    abort();
}

}

TypedConvertedExpr AsInt(const TypedConvertedExpr& ty_expr) {
    auto [desc, ty] = ty_expr;
    if (ty == Type::kBool) {
        return {"(if " + desc + " 1 0)", Type::kInt};
    } else {
        return ty_expr;
    }
}

TypedConvertedExpr AsBool(const TypedConvertedExpr& ty_expr) {
    auto [desc, ty] = ty_expr;
    if (ty == Type::kInt) {
        return {"(> " + desc + " 0)", Type::kBool};
    } else {
        return ty_expr;
    }
}

TypedConvertedExpr AsType(const TypedConvertedExpr& ty_expr, Type expected_type) {
    if (expected_type == Type::kBool) {
        return AsBool(ty_expr);
    } else {
        return AsInt(ty_expr);
    }
}

TypedConvertedExpr ConvertTree(const XCSP3Core::Tree* tree, const VariableMap& var_map) {
    return ConvertTreeImpl(tree->root, var_map);
}
