#include "Converter.h"

#include <algorithm>
#include <map>
#include <sstream>

#include <XCSP3CoreParser.h>

void ConverterCallbacks::buildVariableInteger(std::string id, int minValue, int maxValue) {
    if (0 <= minValue && maxValue <= 1) {
        // boolean variable
        std::string var_name = id;
        variables_.insert({id, {var_name, Type::kBool}});
        converted_.push_back("(bool " + var_name + ")");
        if (minValue == 1) {
            converted_.push_back(var_name);
        }
        if (maxValue == 0) {
            converted_.push_back("(! " + var_name + ")");
        }
    } else {
        // int variable
        std::string var_name = id;
        variables_.insert({id, {var_name, Type::kInt}});
        converted_.push_back("(int " + var_name + " " + std::to_string(minValue) + " " + std::to_string(maxValue) + ")");
    }
}

void ConverterCallbacks::buildVariableInteger(std::string id, std::vector<int>& values) {
    if (values[0] == 0 && values[1] == 1 && values.size() == 2) {
        buildVariableInteger(id, 0, 1);
        return;
    }
    if (values.size() == 0) {
        std::cerr << "empty domain: value " << id << std::endl;
        abort();
    }
    variables_.insert({id, {id, Type::kInt}});
    std::string desc = "(int " + id + " (";
    for (int i = 0; i < values.size(); ++i) {
        if (i != 0) {
            desc.push_back(' ');
        }
        desc += std::to_string(values[i]);
    }
    desc += "))";
    converted_.push_back(desc);
}

void ConverterCallbacks::buildConstraintIntension(std::string id, XCSP3Core::Tree* tree) {
    converted_.push_back(std::get<0>(AsBool(ConvertTree(tree, variables_))));
}

void ConverterCallbacks::buildConstraintOrdered(std::string id, std::vector<XCSP3Core::XVariable *> &list, XCSP3Core::OrderType order) {
    std::string op;
    switch (order) {
        case XCSP3Core::OrderType::LT:
            op = "<";
            break;
        case XCSP3Core::OrderType::LE:
            op = "<=";
            break;
        case XCSP3Core::OrderType::GT:
            op = ">";
            break;
        case XCSP3Core::OrderType::GE:
            op = ">=";
            break;
        default:
            std::cerr << "error: unsupported order type for Ordered: " << order << std::endl;
            abort();
    }
    for (int i = 1; i < list.size(); ++i) {
        converted_.push_back("(" + op + " " + VarDescription(list[i - 1], Type::kInt) + " " + VarDescription(list[i], Type::kInt) + ")");
    }
}

void ConverterCallbacks::buildConstraintAlldifferent(std::string id, std::vector<XCSP3Core::XVariable*> &list) {
    std::string stmt = "(alldifferent";
    for (auto& var : list) {
        stmt.push_back(' ');
        stmt += VarDescription(var, Type::kInt);
    }
    stmt.push_back(')');
    converted_.push_back(stmt);
}

void ConverterCallbacks::buildConstraintAlldifferent(string id, std::vector<XCSP3Core::Tree*> &list) {
    std::string stmt = "(alldifferent";
    for (auto& t : list) {
        stmt.push_back(' ');
        stmt += std::get<0>(AsInt(ConvertTree(t, variables_)));
    }
    stmt.push_back(')');
    converted_.push_back(stmt);
}

void ConverterCallbacks::buildConstraintAlldifferentMatrix(std::string id, std::vector<std::vector<XCSP3Core::XVariable *>> &matrix) {
    int n_row = matrix.size();
    if (n_row == 0) return;
    int n_col = matrix[0].size();
    for (int i = 1; i < n_row; ++i) {
        if (matrix[i].size() != n_col) {
            std::cerr << "jagged matrix not supported for Alldifferent" << std::endl;
            abort();
        }
    }
    for (int y = 0; y < n_row; ++y) {
        std::vector<XCSP3Core::XVariable*> row = matrix[y];
        buildConstraintAlldifferent(id, row);
    }
    for (int x = 0; x < n_col; ++x) {
        std::vector<XCSP3Core::XVariable*> col;
        for (int y = 0; y < n_row; ++y) col.push_back(matrix[y][x]);
        buildConstraintAlldifferent(id, col);
    }
}

void ConverterCallbacks::buildConstraintSum(std::string id, std::vector<XCSP3Core::XVariable *> &list, XCSP3Core::XCondition &cond) {
    std::vector<int> coeffs(list.size(), 1);
    buildConstraintSum(id, list, coeffs, cond);
}

void ConverterCallbacks::buildConstraintSum(std::string id, std::vector<XCSP3Core::XVariable *> &list, std::vector<int> &coeffs, XCSP3Core::XCondition &cond) {
    if (!(cond.operandType == XCSP3Core::OperandType::INTEGER || cond.operandType == XCSP3Core::OperandType::VARIABLE)) {
        std::cerr << "buildConstraintSum supported only for integer operand" << std::endl;
        abort();
    }
    std::string desc = "(";
    switch (cond.op) {
        case XCSP3Core::OrderType::EQ:
            desc += "==";
            break;
        case XCSP3Core::OrderType::NE:
            desc += "!=";
            break;
        case XCSP3Core::OrderType::GE:
            desc += ">=";
            break;
        case XCSP3Core::OrderType::GT:
            desc += ">";
            break;
        case XCSP3Core::OrderType::LE:
            desc += "<=";
            break;
        case XCSP3Core::OrderType::LT:
            desc += "<";
            break;
        default:
            std::cerr << "unsupported order type: " << cond.op << std::endl;
            abort();
    }
    desc += " (+";
    for (int i = 0; i < list.size(); ++i) {
        std::string var_desc = VarDescription(list[i], Type::kInt);

        if (coeffs[i] == 1) {
            desc.push_back(' ');
            desc += var_desc;
        } else if (coeffs[i] == -1) {
            desc += " (- ";
            desc += var_desc;
            desc.push_back(')');
        } else {
            desc += " (* ";
            desc += var_desc;
            desc.push_back(' ');
            desc += std::to_string(coeffs[i]);
            desc.push_back(')');
        }
    }
    desc += ") ";
    if (cond.operandType == XCSP3Core::OperandType::INTEGER) {
        desc += std::to_string(cond.val);
    } else {
        desc += VarDescription(cond.var, Type::kInt);
    }
    desc.push_back(')');
    converted_.push_back(desc);
}

void ConverterCallbacks::buildConstraintExtension(std::string id, std::vector<XCSP3Core::XVariable *> list, std::vector<std::vector<int>> &tuples, bool support, bool hasStar) {
    last_tuples_ = tuples;
    buildConstraintExtensionAs(id, list, support, hasStar);
}

void ConverterCallbacks::buildConstraintExtension(string id, XCSP3Core::XVariable *variable, std::vector<int> &tuples, bool support, bool hasStar) {
    last_tuples_.clear();
    for (int v : tuples) last_tuples_.push_back({v});
    buildConstraintExtensionAs(id, {variable}, support, hasStar);
}

void ConverterCallbacks::buildConstraintExtensionAs(std::string id, std::vector<XCSP3Core::XVariable *> list, bool support, bool hasStar) {
    for (auto& tuple : last_tuples_) {
        bool flg = false;
        for (int i = 0; i < tuple.size(); ++i) {
            if (tuple[i] != STAR) {
                flg = true;
                break;
            }
        }
        if (!flg) {
            // tuple (*,*, ...)
            if (!support) {
                converted_.push_back("false");
            }
            return;
        }
    }
    std::ostringstream oss;
    oss << "(" << (support ? "||" : "&&");
    for (auto& tuple : last_tuples_) {
        oss << " (" << (support ? "&&" : "||");
        for (int i = 0; i < tuple.size(); ++i) {
            if (tuple[i] != STAR) {
                oss << " (" << (support ? "==" : "!=") << " " << VarDescription(list[i], Type::kInt) << " " << tuple[i] << ")";
            }
        }
        oss << ")";
    }
    oss << ")";
    converted_.push_back(oss.str());
}

void ConverterCallbacks::buildConstraintInstantiation(std::string id, std::vector<XCSP3Core::XVariable *> &list, vector<int> &values) {
    if (list.size() != values.size()) {
        if (values.size() == 1) {
            // TODO: is this inference valid?
            values = std::vector<int>(list.size(), values[0]);
        } else {
            std::cerr << "size mismatch" << std::endl;
            abort();
        }
    }
    for (int i = 0; i < list.size(); ++i) {
        int value = values[i];
        if (auto found = variables_.find(list[i]->id); found != variables_.end()) {
            auto [name, ty] = found->second;
            if (ty == Type::kBool) {
                if (value == 0) {
                    converted_.push_back("(! " + name + ")");
                } else if (value == 1) {
                    converted_.push_back(name);
                } else {
                    converted_.push_back("false");
                }
            } else {
                converted_.push_back("(== " + name + " " + std::to_string(value) + ")");
            }
        } else {
            std::cerr << "error: unknown variable: " << list[i]->id << std::endl;
            abort();
        }
    }
}

void ConverterCallbacks::buildConstraintElement(std::string id, std::vector<XCSP3Core::XVariable *> &list, int startIndex, XCSP3Core::XVariable *index, XCSP3Core::RankType rank, XCSP3Core::XVariable* value) {
    buildConstraintElement(id, list, startIndex, index, rank, VarDescription(value, Type::kInt));
}

void ConverterCallbacks::buildConstraintElement(std::string id, std::vector<XCSP3Core::XVariable *> &list, int startIndex, XCSP3Core::XVariable *index, XCSP3Core::RankType rank, int value) {
    buildConstraintElement(id, list, startIndex, index, rank, std::to_string(value));
}

void ConverterCallbacks::buildConstraintElement(std::string id, std::vector<XCSP3Core::XVariable *> &list, int startIndex, XCSP3Core::XVariable *index, XCSP3Core::RankType rank, const std::string& value_desc) {
    if (rank != XCSP3Core::RankType::ANY) {
        std::cerr << "error: RankType other than ANY is not supported" << std::endl;
        abort();
    }
    std::ostringstream oss;
    oss << "(||";
    for (int i = 0; i < list.size(); ++i) {
        oss << " (&& (== " << VarDescription(list[i], Type::kInt) << " " << value_desc
            << ") (== " << VarDescription(index, Type::kInt) << " " << std::to_string(i + startIndex)
            << "))";
    }
    oss << ")";
    converted_.push_back(oss.str());
}

void ConverterCallbacks::buildConstraintElement(std::string id, std::vector<std::vector<XCSP3Core::XVariable*> > &matrix, int startRowIndex, XCSP3Core::XVariable *rowIndex, int startColIndex, XCSP3Core::XVariable* colIndex, XCSP3Core::XVariable* value) {
    buildConstraintElement(id, matrix, startRowIndex, rowIndex, startColIndex, colIndex, VarDescription(value, Type::kInt));
}

void ConverterCallbacks::buildConstraintElement(std::string id, std::vector<std::vector<XCSP3Core::XVariable*> > &matrix, int startRowIndex, XCSP3Core::XVariable *rowIndex, int startColIndex, XCSP3Core::XVariable* colIndex, int value) {
    buildConstraintElement(id, matrix, startRowIndex, rowIndex, startColIndex, colIndex, std::to_string(value));
}

void ConverterCallbacks::buildConstraintElement(std::string id, std::vector<std::vector<XCSP3Core::XVariable*> > &matrix, int startRowIndex, XCSP3Core::XVariable *rowIndex, int startColIndex, XCSP3Core::XVariable* colIndex, const std::string& value_desc) {
    std::ostringstream oss;
    oss << "(||";
    for (int y = 0; y < matrix.size(); ++y) {
        for (int x = 0; x < matrix[y].size(); ++x) {
            oss << " (&& (== " << VarDescription(matrix[y][x], Type::kInt) << " " << value_desc
                << ") (== " << VarDescription(rowIndex, Type::kInt) << " " << std::to_string(y + startRowIndex)
                << ") (== " << VarDescription(colIndex, Type::kInt) << " " << std::to_string(x + startColIndex)
                << "))";
        }
    }
    oss << ")";
    converted_.push_back(oss.str());
}

void ConverterCallbacks::buildConstraintCardinality(std::string id, std::vector<XCSP3Core::XVariable *> &list, std::vector<int> values, std::vector<int> &occurs, bool closed) {
    std::vector<std::string> values_desc;
    for (int i = 0; i < values.size(); ++i) values_desc.push_back(std::to_string(values[i]));
    std::vector<std::string> occurs_desc;
    for (int i = 0; i < occurs.size(); ++i) occurs_desc.push_back(std::to_string(occurs[i]));
    buildConstraintCardinality(id, list, values_desc, occurs_desc, closed);
}

void ConverterCallbacks::buildConstraintCardinality(std::string id, std::vector<XCSP3Core::XVariable *> &list, std::vector<int> values, std::vector<XCSP3Core::XVariable *> &occurs, bool closed) {
    std::vector<std::string> values_desc;
    for (int i = 0; i < values.size(); ++i) values_desc.push_back(std::to_string(values[i]));
    std::vector<std::string> occurs_desc;
    for (int i = 0; i < occurs.size(); ++i) occurs_desc.push_back(VarDescription(occurs[i], Type::kInt));
    buildConstraintCardinality(id, list, values_desc, occurs_desc, closed);
}

void ConverterCallbacks::buildConstraintCardinality(std::string id, std::vector<XCSP3Core::XVariable *> &list, std::vector<int> values, std::vector<XCSP3Core::XInterval> &occurs, bool closed) {
    std::vector<std::string> values_desc;
    for (int i = 0; i < values.size(); ++i) values_desc.push_back(std::to_string(values[i]));
    std::vector<std::string> occurs_desc;
    for (int i = 0; i < occurs.size(); ++i) {
        int lo = occurs[i].min;
        int hi = occurs[i].max;
        std::string var_name = NewAuxVarName();
        converted_.push_back("(int " + var_name + " " + std::to_string(lo) + " " + std::to_string(hi) + ")");
        occurs_desc.push_back(var_name);
    }
    buildConstraintCardinality(id, list, values_desc, occurs_desc, closed);
}

void ConverterCallbacks::buildConstraintCardinality(std::string id, std::vector<XCSP3Core::XVariable *> &list, const std::vector<std::string>& values_desc, const std::vector<std::string> &occurs_desc, bool closed) {
    if (closed) {
        std::cerr << "error: cardinality of closed form not supported" << std::endl;
        abort();
    }

    for (int i = 0; i < values_desc.size(); ++i) {
        std::ostringstream oss;
        oss << "(== (+";
        for (int j = 0; j < list.size(); ++j) {
            oss << " " << "(if (== " << VarDescription(list[j], Type::kInt) << " " << values_desc[i] << ") 1 0)";
        }
        oss << ") " << occurs_desc[i] << ")";
        converted_.push_back(oss.str());
    }
}

void ConverterCallbacks::buildConstraintExactlyK(std::string id, std::vector<XCSP3Core::XVariable *> &list, int value, int k) {
    std::ostringstream oss;
    oss << "(== (+";

    for (int i = 0; i < list.size(); ++i) {
        oss << " " << "(if (== " << VarDescription(list[i], Type::kInt) << " " << value << ") 1 0)";
    }
    oss << ") " << k << ")";
    converted_.push_back(oss.str());
}

void ConverterCallbacks::buildConstraintRegular(std::string id, std::vector<XCSP3Core::XVariable *> &list, std::string start, std::vector<std::string> &final, std::vector<XCSP3Core::XTransition> &transitions) {
    std::map<std::string, int> state_id_map;
    auto state_id = [&](const std::string& name) {
        if (auto found = state_id_map.find(name); found != state_id_map.end()) {
            return found->second;
        } else {
            int ret = state_id_map.size();
            return state_id_map[name] = ret;
        }
    };

    int start_id = state_id(start);
    std::vector<int> final_id;
    for (auto& s : final) final_id.push_back(state_id(s));
    std::vector<std::tuple<int, int, int>> trans;  // (src, dest, value)
    for (auto& t : transitions) {
        int s_id = state_id(t.from);
        int d_id = state_id(t.to);
        trans.push_back({s_id, d_id, t.val});
    }

    std::vector<std::string> states;
    states.push_back(std::to_string(start_id));
    for (int i = 0; i < list.size(); ++i) {
        std::string aux_var = NewAuxVarName();
        if (i + 1 == list.size()) {
            std::ostringstream oss;
            oss << "(int " << aux_var << " (";
            for (int j = 0; j < final_id.size(); ++j) {
                if (j > 0) oss << " ";
                oss << final_id[j];
            }
            oss << "))";
            converted_.push_back(oss.str());
        } else {
            converted_.push_back("(int " + aux_var + " 0 " + std::to_string(state_id_map.size() - 1) + ")");
        }
        states.push_back(aux_var);
    }

    for (int i = 0; i < list.size(); ++i) {
        std::ostringstream oss;
        oss << "(||";
        for (auto& [src, dest, value] : trans) {
            oss << " (&& (== " << states[i] << " " << src << ") (== " << states[i + 1] << " " << dest << ") (== " << VarDescription(list[i], Type::kInt) << " " << value << "))";
        }
        oss << ")";
        converted_.push_back(oss.str());
    }
}

std::string ConverterCallbacks::VarDescription(const std::string& name, Type type) const {
    if (auto found = variables_.find(name); found != variables_.end()) {
        return std::get<0>(AsType(found->second, type));
    } else {
        std::cerr << "error: unknown variable: " << name << std::endl;
        abort();
    }
}

std::string ConverterCallbacks::VarDescription(const XCSP3Core::XVariable* var, Type type) const {
    return VarDescription(var->id, type);
}

std::string ConverterCallbacks::NewAuxVarName() {
    std::string ret("converter_aux_var_");
    ret += std::to_string(n_aux_var_++);
    return ret;
}

std::string ConvertXCSP3Instance(const char* filename) {
    ConverterCallbacks cb;
    cb.recognizeSpecialIntensionCases = false;

    XCSP3Core::XCSP3CoreParser parser(&cb);
    parser.parse(filename);

    std::ostringstream oss;
    for (auto& line : cb.GetConvertedDescriptions()) {
        oss << line << '\n';
    }
    return oss.str();
}
