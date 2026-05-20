#include "domain/billing/PdfExporter.h"
#include "domain/persistence/FilePaths.h"
#include "util/Files.h"

#include <cstdio>
#include <sstream>

namespace pos::domain {

std::string PdfExporter::exportReceipt(const Receipt& r, const std::string& outPath) {
    std::string path = outPath;
    if (path.empty()) {
        pos::util::mkdirs(FilePaths::ReceiptsDir);
        char buf[32];
        std::snprintf(buf, sizeof(buf), "RCPT-%06u.pdf.txt", r.orderId().value());
        path = std::string(FilePaths::ReceiptsDir) + "/" + buf;
    }
    std::ostringstream oss;
    oss << "<<PDF-STYLE EXPORT (text)>>\n";
    oss << r;
    oss << "<<END>>\n";
    pos::util::writeAtomic(path, oss.str());
    return path;
}

} // namespace pos::domain
