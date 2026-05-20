#pragma once

#include "domain/common/DateTime.h"
#include "domain/common/Id.h"
#include "domain/tables/TableStatus.h"

#include <optional>

namespace pos::domain {

class Table {
public:
    Table() = default;
    Table(TableId id, int seats, TableStatus status = TableStatus::Free,
          std::optional<CustomerId> reservedFor = std::nullopt,
          DateTime reservedAt = {});

    TableId                          id() const           { return id_; }
    int                              seats() const        { return seats_; }
    TableStatus                      status() const       { return status_; }
    const std::optional<CustomerId>& reservedFor() const  { return reservedFor_; }
    DateTime                         reservedAt() const   { return reservedAt_; }

    void setSeats(int s)                         { seats_ = s; }
    void setStatus(TableStatus s)                { status_ = s; }
    void setReservedFor(std::optional<CustomerId> c) { reservedFor_ = c; }
    void setReservedAt(DateTime t)               { reservedAt_ = t; }

private:
    TableId                   id_{};
    int                       seats_ = 2;
    TableStatus               status_ = TableStatus::Free;
    std::optional<CustomerId> reservedFor_;
    DateTime                  reservedAt_;
};

} // namespace pos::domain
