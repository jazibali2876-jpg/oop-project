#pragma once

#include "domain/billing/Receipt.h"

#include <string>

namespace pos::domain {

class ReceiptRepository {
public:
    ReceiptRepository();

    // Writes the receipt text into data/receipts/RCPT-NNNNNN.txt and returns the path.
    std::string write(const Receipt& r);

    // Convention: data/receipts/RCPT-NNNNNN.txt
    static std::string pathFor(OrderId id);
};

} // namespace pos::domain
