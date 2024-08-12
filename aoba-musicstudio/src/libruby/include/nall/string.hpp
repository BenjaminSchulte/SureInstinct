#ifndef NALL_STRING_HPP
#define NALL_STRING_HPP

#include <initializer_list>
#include <nall/platform.hpp>
#include <nall/utility.hpp>

#include <nall/string/base.hpp>
#include <nall/string/bsv.hpp>
#include <nall/string/core.hpp>
#include <nall/string/cast.hpp>
#include <nall/string/compare.hpp>
#include <nall/string/convert.hpp>
#include <nall/string/filename.hpp>
#include <nall/string/math.hpp>
#include <nall/string/platform.hpp>
#include <nall/string/strl.hpp>
#include <nall/string/strpos.hpp>
#include <nall/string/trim.hpp>
#include <nall/string/replace.hpp>
#include <nall/string/split.hpp>
#include <nall/string/utility.hpp>
#include <nall/string/variadic.hpp>
#include <nall/string/wrapper.hpp>
#include <nall/string/xml.hpp>

namespace nall {
  template<> struct has_length<string> { enum { value = true }; };
  template<> struct has_size<lstring> { enum { value = true }; };
}

#endif
