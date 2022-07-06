#ifndef ASCII_ESC_H
#define ASCII_ESC_H

// TOKEN_BASED

#define A_ESC "\033"
#define A_ESC_CSI "\033["

#define A_RETURN "\r"
#define A_BACK "\b"
#define A_ENDL "\n"

#define A_CURSOR_HOME "\033[H"
#define A_COORDS "f"

#define A_UP_ONE "\033[M"
#define A_UP_HERE "A"
#define A_UP_FRONT "F"
#define A_DOWN_HERE "B"
#define A_DOWN_FRONT "E"
#define A_RIGHT "C"
#define A_LEFT "D"

#define A_GOTO_C "G"
#define A_SAVE_DEC "\033 7"
#define A_SAVE_SCO "\033[s"
#define A_LOAD_DEC "\033 8"
#define A_LOAD_SCO "\033[u"


#define A_ERASE_TO_EOF "\033[0J"
#define A_ERASE_TO_SOF "\033[1J"
#define A_ERASE_SCREEN "\033[2J"
#define A_ERASE_SAVED "\033[3J"

#define A_ERASE_TO_EOL "\033[0K"
#define A_ERASE_TO_SOL "\033[1K"
#define A_ERASE_LINE "\033[2K"

#define A_BOLD "\033[1m"
#define A_BOLD_OFF "\033[22m"
#define A_UNDERLINE "\033[4m"
#define A_UNDERLINE_OFF "\033[24m"
#define A_ITALICS "\033[3m"
#define A_ITALICS_OFF "\033[23m"
#define A_BLINK "\033[5m"
#define A_BLINK_OFF "\033[25m"
#define A_DIM "\033[2m"
#define A_DIM_OFF "\033[22m"
#define A_INVERT "\033[7m"
#define A_INVERT_OFF "\033[27m"
#define A_HIDE "\033[8m"
#define A_HIDE_OFF "\033[28m"
#define A_STRIKETHROUGH "\033[9m"
#define A_STRIKETHROUGH_OFF "\033[29m"
#define A_DOUBLE_UNDERLINE "\033[21m"
#define A_DOUBLE_UNDERLINE_OFF "\033[24m"

#define A_BLACK_F "\033[30m"
#define A_RED_F "\033[31m"
#define A_GREEN_F "\033[32m"
#define A_YELLOW_F "\033[33m"
#define A_BLUE_F "\033[34m"
#define A_MAGENTA_F "\033[35m"
#define A_CYAN_F "\033[36m"
#define A_WHITE_F "\033[37m"
#define A_DEFAULT_F "\033[39m"

#define A_BLACK_B "\033[40m"
#define A_RED_B "\033[41m"
#define A_GREEN_B "\033[42m"
#define A_YELLOW_B "\033[43m"
#define A_BLUE_B "\033[44m"
#define A_MAGENTA_B "\033[45m"
#define A_CYAN_B "\033[46m"
#define A_WHITE_B "\033[47m"
#define A_DEFAULT_B "\033[49m"

#define A_RESET "\033[0m" 

#endif
