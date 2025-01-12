#ifndef SCROLLER_H
#define SCROLLER_H

#include <ftxui/component/component.hpp>

#include "ftxui/component/component_base.hpp"  // for Component
#include <string>

namespace ftxui {
Component Scroller(Component child);
extern std::string selected_item_text;
}
#endif /* end of include guard: SCROLLER_H */

// Copyright 2021 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
