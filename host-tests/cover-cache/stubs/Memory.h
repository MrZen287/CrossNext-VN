#pragma once
#include <memory>
#include <utility>
template<typename T,typename...A>auto makeUniqueNoThrow(A&&...a){return std::make_unique<T>(std::forward<A>(a)...);}
