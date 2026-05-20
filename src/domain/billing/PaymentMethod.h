#pragma once

#include <cstdint>
#include <string>

namespace pos::domain {

enum class PaymentMethod : std::uint8_t {
    Cash      = 0,
    Card      = 1,
    OnlineSim = 2,
    Unpaid    = 255
};

std::string paymentMethodName(PaymentMethod m);

} // namespace pos::domain
