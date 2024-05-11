#include "Symbol.h"

#include <utility>

Symbol::Symbol(std::string identifier) : identifier(std::move(identifier)) {}
