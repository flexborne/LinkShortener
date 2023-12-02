#pragma once

#include <utility>

class CRUD {
 public:
  template <class... Args>
  auto create(this auto& self, Args&&... args) {
    return self.create(std::forward<Args>(args)...);
  }

  template <class... Args>
  auto read(this auto& self, Args&&... args) {
    return self.readImpl(std::forward<Args>(args)...);
  }

  template <class... Args>
  auto update(this auto& self, Args&&... args) {
    return self.updateImpl(std::forward<Args>(args)...);
  }

  template <class... Args>
  auto delete_(this auto& self, Args&&... args) {
    return self.deleteImpl(std::forward<Args>(args)...);
  }
};