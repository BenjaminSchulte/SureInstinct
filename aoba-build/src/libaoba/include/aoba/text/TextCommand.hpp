#pragma once

namespace Aoba {

enum class TextCommand {
  WRITE_TEXT = 0,
  WAIT_FOR_CONFIRM,
  SET_COLOR,
  SET_ALIGN,
  PRINT_VARIABLE,
  NEW_LINE,
  NEW_PAGE,
  OPTION,
  WAIT
};

}