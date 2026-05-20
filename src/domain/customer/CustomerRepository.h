#pragma once

#include "domain/customer/Customer.h"
#include "domain/persistence/BinaryRepository.h"

#include <cstdint>
#include <optional>
#include <type_traits>
#include <vector>

namespace pos::domain {

#pragma pack(push, 1)
struct CustomerRecord {
    std::uint32_t id;                //  4
    char          name[40];          // 40
    char          phone[20];         // 20
    std::int64_t  joinedAtEpoch;     //  8
    std::int64_t  totalSpentCents;   //  8
    std::int32_t  loyaltyPoints;     //  4
    std::int32_t  orderCount;        //  4
    std::uint8_t  active;            //  1
    std::uint8_t  pad[7];            //  7  -> total 96
};
#pragma pack(pop)
static_assert(sizeof(CustomerRecord) == 96, "CustomerRecord layout drift");
static_assert(std::is_trivially_copyable_v<CustomerRecord>);

class CustomerRepository : public BinaryRepository<CustomerRecord> {
public:
    CustomerRepository();

    std::vector<Customer>     loadAll() const;
    void                      saveAll(const std::vector<Customer>& customers);
    std::optional<Customer>   findByPhone(const std::string& phone) const;
    std::optional<Customer>   findById(CustomerId id) const;
    std::uint32_t             nextId() const;

    static Customer       toCustomer(const CustomerRecord& r);
    static CustomerRecord toRecord(const Customer& c);
};

} // namespace pos::domain
