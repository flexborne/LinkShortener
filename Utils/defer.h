#pragma once

#include <utility>

template <class F>
class Deferrer
{
    F f;
public:
    Deferrer(F&& f) : f{std::forward<F>(f)} {}
    ~Deferrer() { f(); }
};

#define defer(function) auto def = Deferrer{function}
