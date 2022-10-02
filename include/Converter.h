#pragma once

#include <string>
#include <vector>

#include <XCSP3CoreCallbacks.h>

#include "TreeConverter.h"

class ConverterCallbacks : public XCSP3Core::XCSP3CoreCallbacks {
public:
    virtual void buildVariableInteger(std::string id, int minValue, int maxValue) override;
    virtual void buildVariableInteger(std::string id, std::vector<int>& values) override;
    virtual void buildConstraintIntension(std::string id, XCSP3Core::Tree* tree) override;
    virtual void buildConstraintOrdered(std::string id, std::vector<XCSP3Core::XVariable *> &list, XCSP3Core::OrderType order) override;
    virtual void buildConstraintLex(std::string id, std::vector<std::vector<XCSP3Core::XVariable *>> &lists, XCSP3Core::OrderType order) override;
    virtual void buildConstraintAlldifferent(std::string id, std::vector<XCSP3Core::XVariable*> &list) override;
    virtual void buildConstraintAlldifferent(std::string id, std::vector<XCSP3Core::Tree*> &list) override;
    virtual void buildConstraintAlldifferentMatrix(std::string id, std::vector<std::vector<XCSP3Core::XVariable *>> &matrix) override;
    virtual void buildConstraintSum(std::string id, std::vector<XCSP3Core::XVariable *> &list, XCSP3Core::XCondition &cond) override;
    virtual void buildConstraintSum(std::string id, std::vector<XCSP3Core::XVariable *> &list, std::vector<int> &coeffs, XCSP3Core::XCondition &cond) override;
    virtual void buildConstraintExtension(std::string id, std::vector<XCSP3Core::XVariable *> list, std::vector<std::vector<int>> &tuples, bool support, bool hasStar) override;
    virtual void buildConstraintExtension(string id, XCSP3Core::XVariable *variable, std::vector<int> &tuples, bool support, bool hasStar) override;
    virtual void buildConstraintExtensionAs(std::string id, std::vector<XCSP3Core::XVariable *> list, bool support, bool hasStar) override;
    virtual void buildConstraintInstantiation(std::string id, std::vector<XCSP3Core::XVariable *> &list, vector<int> &values) override;
    virtual void buildConstraintElement(std::string id, std::vector<XCSP3Core::XVariable *> &list, int startIndex, XCSP3Core::XVariable *index, XCSP3Core::RankType rank, XCSP3Core::XVariable* value) override;
    virtual void buildConstraintElement(std::string id, std::vector<XCSP3Core::XVariable *> &list, int startIndex, XCSP3Core::XVariable *index, XCSP3Core::RankType rank, int value) override;
    void buildConstraintElement(std::string id, std::vector<XCSP3Core::XVariable *> &list, int startIndex, XCSP3Core::XVariable *index, XCSP3Core::RankType rank, const std::string& value_desc);
    virtual void buildConstraintElement(std::string id, std::vector<std::vector<XCSP3Core::XVariable*> > &matrix, int startRowIndex, XCSP3Core::XVariable *rowIndex, int startColIndex, XCSP3Core::XVariable* colIndex, XCSP3Core::XVariable* value) override;
    virtual void buildConstraintElement(std::string id, std::vector<std::vector<XCSP3Core::XVariable*> > &matrix, int startRowIndex, XCSP3Core::XVariable *rowIndex, int startColIndex, XCSP3Core::XVariable* colIndex, int value) override;
    void buildConstraintElement(std::string id, std::vector<std::vector<XCSP3Core::XVariable*> > &matrix, int startRowIndex, XCSP3Core::XVariable *rowIndex, int startColIndex, XCSP3Core::XVariable* colIndex, const std::string& value_desc);
    virtual void buildConstraintCardinality(std::string id, std::vector<XCSP3Core::XVariable *> &list, std::vector<int> values, std::vector<int> &occurs, bool closed) override;
    virtual void buildConstraintCardinality(std::string id, std::vector<XCSP3Core::XVariable *> &list, std::vector<int> values, std::vector<XCSP3Core::XVariable *> &occurs, bool closed) override;
    virtual void buildConstraintCardinality(std::string id, std::vector<XCSP3Core::XVariable *> &list, std::vector<int> values, std::vector<XCSP3Core::XInterval> &occurs, bool closed) override;
    void buildConstraintCardinality(std::string id, std::vector<XCSP3Core::XVariable *> &list, const std::vector<std::string>& values_desc, const std::vector<std::string> &occurs_desc, bool closed);
    virtual void buildConstraintExactlyK(std::string id, std::vector<XCSP3Core::XVariable *> &list, int value, int k) override;
    virtual void buildConstraintRegular(std::string id, std::vector<XCSP3Core::XVariable *> &list, std::string start, std::vector<std::string> &final, std::vector<XCSP3Core::XTransition> &transitions) override;

    const std::vector<std::string>& GetConvertedDescriptions() const { return converted_; }

private:
    int n_aux_var_ = 0;
    std::map<std::string, std::tuple<std::string, Type>> variables_;
    std::vector<std::string> converted_;
    std::vector<std::vector<int>> last_tuples_;

    std::string VarDescription(const std::string& name, Type type) const;
    std::string VarDescription(const XCSP3Core::XVariable* var, Type type) const;

    std::string NewAuxVarName();
};

std::string ConvertXCSP3Instance(const char* filename);
