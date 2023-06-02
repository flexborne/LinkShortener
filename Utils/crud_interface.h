#pragma once

#include <utility>

template <class Derived>
class CRUD {
 public:
  template <class... Args>
  auto create(Args&&... args) {
    return derived().createImpl(std::forward<Args>(args)...);
  }

  template <class... Args>
  const auto read(Args&&... args) const {
    return derived().readImpl(std::forward<Args>(args)...);
  }

  template <class... Args>
  auto update(Args&&... args) {
    return derived().updateImpl(std::forward<Args>(args)...);
  }

  template <class... Args>
  auto delete_(Args&&... args) {
    return derived().deleteImpl(std::forward<Args>(args)...);
  }

 private:
  Derived& derived() { return static_cast<Derived&>(*this); }
  const Derived& derived() const { return static_cast<const Derived&>(*this); }
};