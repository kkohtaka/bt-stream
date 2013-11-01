// Copyright (c) 2013 Kazumasa Kohtaka. All rights reserved.
// This file is available under the MIT license.

#ifndef SRC_UTIL_H_
#define SRC_UTIL_H_

#include <bluetooth/bluetooth.h>
#include <string>

namespace Util {

extern std::string get_bdaddr_str(const ::bdaddr_t &bdaddr);

}  // namespace Util

#endif  // SRC_UTIL_H_

