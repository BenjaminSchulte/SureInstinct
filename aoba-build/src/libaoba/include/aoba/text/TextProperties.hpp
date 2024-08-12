#pragma once

namespace Aoba {
  // Minimum length of a dictionary record
  static const int TEXT_MIN_DICT_LENGTH = 4;

  // Maximum length of a dictionary record
  static const int TEXT_MAX_DICT_LENGTH = 256;

  // Minimum occurrences for a text to be added to the dictionary
  static const int TEXT_MIN_OCCURRENCE = 3;

  // Maximum ID for letters which are only 1 byte of size
  static const int TEXT_NUM_ONE_BYTE_LETTERS = 192;

  // Maximum ID for letters
  static const int TEXT_MAX_NUM_LETTERS = 0x1000;

  // The maximum dictionary (num records)
  static const int TEXT_DICT_SIZE = 512;

  // The maximum length of a text message
  static const int TEXT_MAX_LENGTH = 256;



  // Set text color
  // Commands $E0..$EF are for decompression
  static const unsigned char TEXT_CMD_PRINT_VARIABLE = 0xE0;
  
  // Text termination
  // Commands $F0..$FF are for rendering
  static const unsigned char TEXT_CMD_END_OF_TEXT = 0xF0;
  static const unsigned char TEXT_CMD_SET_COLOR0 = 0xF1;
  static const unsigned char TEXT_CMD_SET_COLOR1 = 0xF2;
  static const unsigned char TEXT_CMD_SET_COLOR2 = 0xF3;
  static const unsigned char TEXT_CMD_ALIGN_LEFT = 0xF4;
  static const unsigned char TEXT_CMD_ALIGN_CENTER = 0xF5;
  static const unsigned char TEXT_CMD_ALIGN_RIGHT = 0xF6;
  static const unsigned char TEXT_CMD_NEW_LINE = 0xF7;
  static const unsigned char TEXT_CMD_NEW_PAGE = 0xF8;
  static const unsigned char TEXT_CMD_OPTION = 0xF9;
  static const unsigned char TEXT_CMD_CHOOSE_OPTION = 0xFA;
  static const unsigned char TEXT_CMD_DELAY = 0xFB;
}