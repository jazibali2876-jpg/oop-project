#pragma once

#include "domain/billing/Receipt.h"

#include <string>

namespace pos::domain {

// Writes a "PDF-style" text file (text-based, not a true PDF — out of scope per plan).
class PdfExporter {
public:
    // Returns the output path written. If outPath is empty, defaults to
    // data/receipts/RCPT-NNNNNN.pdf.txt.
    static std::string exportReceipt(const Receipt& r, const std::string& outPath = "");
};

} // namespace pos::domain
