#ifdef MACCEL_ENABLE
#   include "features/maccel/maccel.h"
enum my_keycodes {
    MA_TOGGLE = QK_USER, // mouse acceleration curve scale step key
    MA_CPI,              // software CPI
    MA_TAKEOFF,          // mouse acceleration curve takeoff (initial acceleration) step key
    MA_GROWTH_RATE,      // mouse acceleration curve growth rate step key
    MA_OFFSET,           // mouse acceleration curve offset step key
    MA_LIMIT,            // mouse acceleration curve limit step key
};
#define MA_TOGL MA_TOGGLE
#define MA_TKFF MA_TAKEOFF
#define MA_GRWT MA_GROWTH_RATE
#define MA_OFST MA_OFFSET
#define MA_LIMT MA_LIMIT
#endif
