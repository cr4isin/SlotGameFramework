#pragma once

#include <string>
#include <map>

using ColorStyle = uint32_t;

enum ColorStyles : ColorStyle {
    // Text Colors (0x000000FF)
    tLAVENDER = 1,
    tPURPLE,
    tPINK,
    tRED,
    tORANGE,
    tBROWN,
    tGOLD,
    tYELLOW,
    tLIME,
    tGREEN,
    tTEAL,
    tCYAN,
    tBLUE,
    tWHITE,
    tGRAY,
    tBLACK,

    // Background Colors (0x0000FF00) — shifted left 8 bits
    bLAVENDER = 1 << 8,
    bPURPLE = 2 << 8,
    bPINK = 3 << 8,
    bRED = 4 << 8,
    bORANGE = 5 << 8,
    bBROWN = 6 << 8,
    bGOLD = 7 << 8,
    bYELLOW = 8 << 8,
    bLIME = 9 << 8,
    bGREEN = 10 << 8,
    bTEAL = 11 << 8,
    bCYAN = 12 << 8,
    bBLUE = 13 << 8,
    bWHITE = 14 << 8,
    bGRAY = 15 << 8,
    bBLACK = 16 << 8,

    // Text Effects (0x00FF0000) — shifted left 16 bits
    eBOLD = 0x01 << 16,
    eDIM = 0x02 << 16,
    eITALIC = 0x04 << 16,
    eUNDERLINE = 0x08 << 16,
    eBLINK = 0x10 << 16,
    eINVERSE = 0x20 << 16,
    eHIDDEN = 0x40 << 16,
    eSTRIKETHROUGH = 0x80 << 16
};

class ANSI {
public:
    static inline const std::map<ColorStyle, std::string> ColorANSIMap = {
        // Text
        {tLAVENDER,"\033[38;5;141m"},
        {tPURPLE,  "\033[38;5;5m"},
        {tPINK,    "\033[38;5;198m"},
        {tRED,     "\033[38;5;1m"},
        {tORANGE,  "\033[38;5;202m"},
        {tBROWN,   "\033[38;5;94m"},
        {tGOLD,    "\033[38;5;3m"},
        {tYELLOW,  "\033[38;5;226m"},
        {tLIME,    "\033[38;5;118m"},
        {tGREEN,   "\033[38;5;2m"},
        {tTEAL,    "\033[38;5;36m"},
        {tCYAN,    "\033[38;5;6m"},
        {tBLUE,    "\033[38;5;4m"},
        {tWHITE,   "\033[38;5;7m"},
        {tGRAY,    "\033[38;5;8m"},
        {tBLACK,   "\033[38;5;0m"},
        // Background
        {bLAVENDER,"\033[48;5;141m"},
        {bPURPLE,  "\033[48;5;5m"},
        {bPINK,    "\033[48;5;198m"},
        {bRED,     "\033[48;5;1m"},
        {bORANGE,  "\033[48;5;202m"},
        {bBROWN,   "\033[48;5;94m"},
        {bGOLD,    "\033[48;5;3m"},
        {bYELLOW,  "\033[48;5;226m"},
        {bLIME,    "\033[48;5;118m"},
        {bGREEN,   "\033[48;5;2m"},
        {bTEAL,    "\033[48;5;36m"},
        {bCYAN,    "\033[48;5;6m"},
        {bBLUE,    "\033[48;5;4m"},
        {bWHITE,   "\033[48;5;7m"},
        {bGRAY,    "\033[48;5;8m"},
        {bBLACK,   "\033[48;5;0m"},
    };

    // Reset & Formatting
    static std::string Reset() { return "\033[0m"; }
    static std::string Bold() { return "\033[1m"; }
    static std::string Dim() { return "\033[2m"; }
    static std::string Italic() { return "\033[3m"; }
    static std::string Underline() { return "\033[4m"; }
    static std::string Blink() { return "\033[5m"; }
    static std::string Inverse() { return "\033[7m"; }
    static std::string Hidden() { return "\033[8m"; }
    static std::string Strikethrough() { return "\033[9m"; }
    static std::string Color(ColorStyle styleCode)
    {
        std::string ansiCode;

        uint32_t fg = styleCode & 0xFF;
        uint32_t bg = styleCode & 0xFF00;
        uint32_t fx = styleCode & 0xFF0000;

        if (ColorANSIMap.contains(fg)) ansiCode += ColorANSIMap.at(fg);
        if (ColorANSIMap.contains(bg)) ansiCode += ColorANSIMap.at(bg);

        if (fx & eBOLD) ansiCode += ANSI::Bold();
        if (fx & eDIM) ansiCode += ANSI::Dim();
        if (fx & eITALIC) ansiCode += ANSI::Italic();
        if (fx & eUNDERLINE) ansiCode += ANSI::Underline();
        if (fx & eBLINK) ansiCode += ANSI::Blink();
        if (fx & eINVERSE) ansiCode += ANSI::Inverse();
        if (fx & eHIDDEN) ansiCode += ANSI::Hidden();
        if (fx & eSTRIKETHROUGH) ansiCode += ANSI::Strikethrough();

        return ansiCode;
    }

    // Cursor
    static std::string MoveCursorUp(int n) { return "\033[" + std::to_string(n) + "A"; }
    static std::string MoveCursorDown(int n) { return "\033[" + std::to_string(n) + "B"; }
    static std::string MoveCursorRight(int n) { return "\033[" + std::to_string(n) + "C"; }
    static std::string MoveCursorLeft(int n) { return "\033[" + std::to_string(n) + "D"; }
    static std::string NextLine(int n) { return "\033[" + std::to_string(n) + "E"; }
    static std::string PreviousLine(int n) { return "\033[" + std::to_string(n) + "F"; }

    static std::string SetCursor(int row, int col) { return "\033[" + std::to_string(row) + ";" + std::to_string(col) + "H"; }
    static std::string CursorHome() { return "\033[H"; }
    static std::string CursorEnd() { return "\033[F"; }

    static std::string SaveCursor() { return "\033[s"; }
    static std::string RestoreCursor() { return "\033[u"; }

    static std::string HideCursor() { return "\033[?25l"; }
    static std::string ShowCursor() { return "\033[?25h"; }

    // Clearing
    static std::string ClearScreen() { return "\033[2J"; }
    static std::string ClearScreenBeforeCursor() { return "\033[1J"; }
    static std::string ClearScreenAfterCursor() { return "\033[0J"; }
    static std::string ClearScrollBack () { return "\033[3J"; }
    static std::string ClearLine() { return "\033[2K"; }
    static std::string ClearLineBeforeCursor() { return "\033[1K"; }
    static std::string ClearLineAfterCursor() { return "\033[0K"; }
    static std::string FullCLear() { return "\033[H\033[2J\033[3J"; }

    // Bell
    static std::string RingBell() { return "\007"; }

    // 256-color Text
    static std::string TCOLOR256(int styleCode) {
        return "\033[38;5;" + std::to_string(styleCode) + "m";
    }

    // 256-color Background
    static std::string BCOLOR256(int styleCode) {
        return "\033[48;5;" + std::to_string(styleCode) + "m";
    }

    // RGB Truecolor (optional, if needed)
    static std::string TCOLORRGB(int r, int g, int b) {
        return "\033[38;2;" + std::to_string(r) + ";" +
            std::to_string(g) + ";" + std::to_string(b) + "m";
    }

    static std::string BCOLORRGB(int r, int g, int b) {
        return "\033[48;2;" + std::to_string(r) + ";" +
            std::to_string(g) + ";" + std::to_string(b) + "m";
    }
};

