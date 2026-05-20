#include "domain/auth/Session.h"
#include "domain/common/Exceptions.h"

namespace pos::domain {

const User& Session::user() const {
    if (!currentUser_) throw DomainException("Session not authenticated.");
    return *currentUser_;
}

User& Session::user() {
    if (!currentUser_) throw DomainException("Session not authenticated.");
    return *currentUser_;
}

void Session::set(std::unique_ptr<User> u) {
    currentUser_ = std::move(u);
    loginAt_     = DateTime::now();
}

void Session::clear() {
    currentUser_.reset();
    loginAt_ = DateTime{};
}

} // namespace pos::domain
