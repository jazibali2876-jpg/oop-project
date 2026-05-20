#pragma once

#include "domain/common/Id.h"
#include "domain/common/Money.h"

#include <string>

namespace pos::domain {

class Ingredient {
public:
    Ingredient() = default;
    Ingredient(IngredientId id, std::string name, std::string unit,
               double stock, double reorderThreshold, Money costPerUnit);

    IngredientId       id() const                { return id_; }
    const std::string& name() const              { return name_; }
    const std::string& unit() const              { return unit_; }
    double             stock() const             { return stock_; }
    double             reorderThreshold() const  { return reorderThreshold_; }
    Money              costPerUnit() const       { return costPerUnit_; }

    void setName(std::string n)              { name_ = std::move(n); }
    void setUnit(std::string u)              { unit_ = std::move(u); }
    void setStock(double s)                  { stock_ = s; }
    void setReorderThreshold(double t)       { reorderThreshold_ = t; }
    void setCostPerUnit(Money c)             { costPerUnit_ = c; }

    void addStock(double delta)              { stock_ += delta; }
    bool isLow() const                       { return stock_ <= reorderThreshold_; }

private:
    IngredientId id_{};
    std::string  name_;
    std::string  unit_;
    double       stock_ = 0.0;
    double       reorderThreshold_ = 0.0;
    Money        costPerUnit_;
};

} // namespace pos::domain
