#pragma once

#include <utility>

class CRUD {
 public:
  template <class... Args>
  void create(this auto& self, Args&&... args) {
    self.create_impl(std::forward<Args>(args)...);
  }

  template <class... Args>
  void read(this auto& self, Args&&... args) {
    self.read_impl(std::forward<Args>(args)...);
  }

  template <class... Args>
  void update(this auto& self, Args&&... args) {
    self.update_impl(std::forward<Args>(args)...);
  }

  template <class... Args>
  void delete_(this auto& self, Args&&... args) {
    self.delete_impl(std::forward<Args>(args)...);
  }
};