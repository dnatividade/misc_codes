/* Frufru Library v2.0
   -------------- ----
   Implements terminal resources:
    color, clean, set position, etc
*/

#define clear() printf("\033[H\033[J")
#define gotoxy(x,y) printf("\033[%d;%dH", (x), (y))

#define ANSI_COLOR_BLACK	"\x1b[30m"
#define ANSI_COLOR_RED		"\x1b[31m"
#define ANSI_COLOR_GREEN	"\x1b[32m"
#define ANSI_COLOR_YELLOW	"\x1b[33m"
#define ANSI_COLOR_BLUE		"\x1b[34m"
#define ANSI_COLOR_MAGENTA	"\x1b[35m"
#define ANSI_COLOR_CYAN		"\x1b[36m"
#define ANSI_COLOR_WHITE	"\x1b[37m"

#define ANSI_BGCOLOR_WHITE 	"\x1b[47m"
#define ANSI_BGCOLOR_YELLOW_L	"\x1b[103m"
#define ANSI_BGCOLOR_MAGENTA_L	"\x1b[105m"
#define ANSI_BGCOLOR_CYAN_L	"\x1b[106m"

#define ANSI_COLOR_RESET	"\x1b[0m"


