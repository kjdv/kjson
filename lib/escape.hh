#pragma once

#include <string>
#include <string_view>

namespace kjson {

void        iescape(std::string& str);
std::string escape(std::string_view input);

} // namespace kjson
