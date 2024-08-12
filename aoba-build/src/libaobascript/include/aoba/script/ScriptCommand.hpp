#pragma once

namespace Aoba {

  enum class ScriptCommand {


    BRK                     = 0x00,
    EXIT                    = 0x01,
    RTS                     = 0x02,
    SWITCH                  = 0x03,
    TAC0                    = 0x04,
    TC0A                    = 0x05,
    PHC0                    = 0x06,
    PLC0                    = 0x07,
    TAX                     = 0x08,
    TXA                     = 0x09,
    FUNC_START              = 0x0A,
    FUNC_END                = 0x0B,


    LDA8_IMMEDIATE          = 0x10,
    LDA16_IMMEDIATE         = 0x11,
    LDA24_IMMEDIATE         = 0x12,
    LDA32_IMMEDIATE         = 0x13,
    // --
    LDA8_PARAM              = 0x18,
    LDA16_PARAM             = 0x19,
    LDA24_PARAM             = 0x1A,
    LDA32_PARAM             = 0x1B,
    LDA8_RAM_U              = 0x1C,
    LDA16_RAM_U             = 0x1D,
    // global
    // global

    LDA8_RAM                = 0x20, //ok
    LDA16_RAM               = 0x22, //ok
    LDA_VARIABLE            = 0x24,
    LDI_VARIABLE            = 0x25,
    LDX8_IMMEDIATE          = 0x26,
    LDX16_IMMEDIATE         = 0x27,
    LDA_BIT                 = 0x28, //+8
    ORA8_RAM_C0             = 0x30, //ok
    ORA16_RAM_C0            = 0x31, //ok
    AND8_RAM_C0             = 0x32, //ok
    AND16_RAM_C0            = 0x33, //ok
    LDA8_RAM_C0             = 0x34, //ok
    LDA16_RAM_C0            = 0x35, //ok
    STA8_RAM_C0             = 0x36, //ok
    STA16_RAM_C0            = 0x37, //ok
    //
    BBS                     = 0x40,
    BBC                     = 0x41,
    CZE                     = 0x42,
    CNZ                     = 0x43,
    BZE                     = 0x44,
    BNZ                     = 0x45,
    BLT                     = 0x46,
    BGE                     = 0x47,
    BVC                     = 0x48,
    BVS                     = 0x49,
    BMI                     = 0x4A,
    BPL                     = 0x4B,
    BLT_IMMEDIATE           = 0x4E,
    BGE_IMMEDIATE           = 0x4F,

    //

    DEC                     = 0x50,
    INC                     = 0x51,
    STA8_PARAM              = 0x58,
    STA16_PARAM             = 0x59,
    STA24_PARAM             = 0x5A,
    STA32_PARAM             = 0x5B,
    STA8_RAM_U              = 0x5C,
    STA16_RAM_U             = 0x5D,
    // global
    // global
    STA8_RAM                = 0x60, //ok
    STA16_RAM               = 0x62, //ok
    SET_VARIABLE            = 0x64,
    STA_VARIABLE            = 0x65,
    //
    //
    SET_BIT                 = 0x68, //+8
    CLR_BIT                 = 0x70, //+8
    CLR_VARIABLE            = 0x78,
    // 0x79...

    ADD8_STACK              = 0x80, //ok
    ADD16_STACK             = 0x81, //ok
    ADD24_STACK             = 0x82,
    ADD32_STACK             = 0x83,
    ADD8_STACK_SIGNED       = 0x84,
    ADD16_STACK_SIGNED      = 0x85,
    ADD24_STACK_SIGNED      = 0x86,
    ADD32_STACK_SIGNED      = 0x87,
    SUB8_STACK              = 0x88,
    SUB16_STACK             = 0x89,
    SUB24_STACK             = 0x8A,
    SUB32_STACK             = 0x8B,
    SUB8_STACK_SIGNED       = 0x8C,
    SUB16_STACK_SIGNED      = 0x8D,
    SUB24_STACK_SIGNED      = 0x8E,
    SUB32_STACK_SIGNED      = 0x8F,
    ADD8_IMMEDIATE          = 0x90, //ok
    ADD16_IMMEDIATE         = 0x91,
    SUB8_IMMEDIATE          = 0x92, //ok
    SUB16_IMMEDIATE         = 0x93,
    ASL_IMMEDIATE           = 0x94, //ok
    LSR_IMMEDIATE           = 0x95, //ok
    AND8_IMMEDIATE          = 0x96, //ok
    AND16_IMMEDIATE         = 0x97, //ok
    ORA8_IMMEDIATE          = 0x98,
    ORA16_IMMEDIATE         = 0x99,
  //
    ASL8_STACK              = 0x9E,
    LSR8_STACK              = 0x9F,
    AND8_STACK              = 0xA0,
    AND16_STACK             = 0xA1,
    AND24_STACK             = 0xA2,
    AND32_STACK             = 0xA3,
    ORA8_STACK              = 0xA4,
    ORA16_STACK             = 0xA5,
    ORA24_STACK             = 0xA6,
    ORA32_STACK             = 0xA7,
    ORA8_RAM                = 0xA8, //ok
    ORA16_RAM               = 0xAA, //ok
    AND8_RAM                = 0xAC,
    AND16_RAM               = 0xAE,
    PHA8                    = 0xB0,
    PHA16                   = 0xB1,
    PHA24                   = 0xB2,
    PHA32                   = 0xB3,
    PLA8                    = 0xB4,
    PLA16                   = 0xB5,
    PLA24                   = 0xB6,
    PLA32                   = 0xB7,
    PUSH8_IMMEDIATE8        = 0xB8,
    PUSH16_IMMEDIATE8       = 0xB9,
    PUSH16_IMMEDIATE16      = 0xBA,
    PUSH24_IMMEDIATE8       = 0xBC,
    PUSH24_IMMEDIATE16      = 0xBD,
    PUSH24_IMMEDIATE24      = 0xBE,
    PUSH32_IMMEDIATE32      = 0xBF,
    INVOKE                  = 0xC0,
    // includes all up to 0xCF
    JSR                     = 0xD0,
    // includes all up to 0xDF
    JMP                     = 0xE0,
    // includes all up to 0xEF
    RESERVED                = 0xF0,
    // includes all up to 0xFF

  // ===============================================
  // DEPRECATED OR NOT REALIGNED


    //PARAMS_END              = 0x0A,
    //DEC                     = 0x0B,
    //INC                     = 0x0C,
    //XBA                     = 0x0D,
    //PARAMS_START            = 0x0E,


    // ===================================================




  };

}