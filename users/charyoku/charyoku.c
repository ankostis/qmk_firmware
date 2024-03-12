#include "charyoku.h"

#ifdef MACCEL_ENABLE

report_mouse_t pointing_device_task_user(report_mouse_t mouse_report) {
    return pointing_device_task_maccel(mouse_report);
}

void keyboard_post_init_user(void) {
    keyboard_post_init_maccel();
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (!process_record_maccel(keycode, record, MA_TOGGLE, MA_CPI, MA_TAKEOFF, MA_GROWTH_RATE, MA_OFFSET, MA_LIMIT)) {
        return false;
    }
    /* insert your own macros here */
    return true;
}
#endif // MACCEL_ENABLE
