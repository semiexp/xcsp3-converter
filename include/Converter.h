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
    virtual void buildConstraintAlldifferent(std::string id, std::vector<XCSP3Core::XVariable*> &list) override;
    virtual void buildConstraintAlldifferent(string id, std::vector<XCSP3Core::Tree*> &list) override;
    virtual void buildConstraintSum(std::string id, std::vector<XCSP3Core::XVariable *> &list, XCSP3Core::XCondition &cond) override;
    virtual void buildConstraintSum(std::string id, std::vector<XCSP3Core::XVariable *> &list, std::vector<int> &coeffs, XCSP3Core::XCondition &cond) override;
    virtual void buildConstraintExtension(std::string id, std::vector<XCSP3Core::XVariable *> list, std::vector<std::vector<int>> &tuples, bool support, bool hasStar) override;
    virtual void buildConstraintExtension(string id, XCSP3Core::XVariable *variable, std::vector<int> &tuples, bool support, bool hasStar) override;
    virtual void buildConstraintExtensionAs(std::string id, std::vector<XCSP3Core::XVariable *> list, bool support, bool hasStar) override;

    const std::vector<std::string>& GetConvertedDescriptions() const { return converted_; }

private:
    std::map<std::string, std::tuple<std::string, Type>> variables_;
    std::vector<std::string> converted_;
    std::vector<std::vector<int>> last_tuples_;

    std::string VarDescription(const std::string& name, Type type) const;
    std::string VarDescription(const XCSP3Core::XVariable* var, Type type) const;
};

std::string ConvertXCSP3Instance(const char* filename);
