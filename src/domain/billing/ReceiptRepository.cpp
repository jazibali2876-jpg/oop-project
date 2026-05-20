#include "domain/billing/ReceiptRepository.h"
#include "domain/persistence/FilePaths.h"
#include "util/Files.h"

#include <cstdio>
#include <sstream>

namespace pos::domain {

ReceiptRepository::ReceiptRepository() {
    pos::util::mkdirs(FilePaths::ReceiptsDir);
}

std::string ReceiptRepository::pathFor(OrderId id) {
    char buf[32];
    std::snprintf(buf, sizeof(buf), "RCPT-%06u.txt", id.value());
    return std::string(FilePaths::ReceiptsDir) + "/" + buf;
}

std::string ReceiptRepository::write(const Receipt& r) {
    pos::util::mkdirs(FilePaths::ReceiptsDir);
    std::string p = pathFor(r.orderId());
    std::ostringstream oss;
    oss << r;
    pos::util::writeAtomic(p, oss.str());
    return p;
}

} // namespace pos::domain
