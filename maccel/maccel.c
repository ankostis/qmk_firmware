// Copyright 2024 burkfers (@burkfers)
// Copyright 2024 Wimads (@wimads)
// SPDX-License-Identifier: GPL-2.0-or-later

#include "quantum.h" // IWYU pragma: keep
#include "maccel.h"
#include "math.h"

/**
 * MAccel Curve Parameters
 * https://www.desmos.com/calculator/rlh6o2kx2w
 *
 * MACCEL_CPI (C)
 *
 * The CPI desired from the maccel algo, independent of device's CPI setting
 * (the latter is free to control mouse's hardware accuracy).
 *
 * --/++ value --> pointer speedier/slower
 *
 * It may prove to be a device specific parameter, if sensors report
 * physical drift differently.
 *
 * It is NOT fed into the curve, not to interfere with the tuning of it's parameters.
 */
#ifndef MACCEL_CPI
#    define MACCEL_CPI 120.0
#endif
#ifndef MACCEL_TAKEOFF
#    define MACCEL_TAKEOFF 2.0 // (K) lower/higher value = curve starts more smoothly/abruptly
#endif
#ifndef MACCEL_GROWTH_RATE
#    define MACCEL_GROWTH_RATE 0.25 // (G) lower/higher value = curve reaches its upper limit slower/faster
#endif
#ifndef MACCEL_OFFSET
#    define MACCEL_OFFSET 2.2 // (S) lower/higher value = acceleration kicks in earlier/later
#endif
#ifndef MACCEL_LIMIT
#    define MACCEL_LIMIT 6.0 // (M) upper limit of accel curve (maximum acceleration factor)
#endif
/**
 * Scale acceleration curve's v-input so that its params are not uncannily small
 * and floats do not overflow in the accel formula (eg. `exp(709.8)` for doubles).
 * Eventually the accel formula is calculated as if the pointer reports @ 1000 CPI,
 * but "counts" are expressed as float (vs integer for the hw "counts").
 */
#define MACCEL_MAGNIFICATION_DPI 1000.0

#ifndef MACCEL_CPI_THROTTLE_MS
#    define MACCEL_CPI_THROTTLE_MS 200 // milliseconds to wait between requesting the device's current DPI
#endif
#ifndef MACCEL_ROUNDING_CURRY_TIMEOUT_MS
#    define MACCEL_ROUNDING_CURRY_TIMEOUT_MS 300 // Elapsed time after which quantization error gets reset.
#endif

maccel_config_t g_maccel_config = {
    // clang-format off
    .cpi =          MACCEL_CPI,
    .growth_rate =  MACCEL_GROWTH_RATE,
    .offset =       MACCEL_OFFSET,
    .limit =        MACCEL_LIMIT,
    .takeoff =      MACCEL_TAKEOFF,
    .enabled =      true
    // clang-format on
};

#ifdef MACCEL_USE_KEYCODES
#    ifndef MACCEL_CPI_STEP
#        define MACCEL_CPI_STEP 10
#    endif
#    ifndef MACCEL_TAKEOFF_STEP
#        define MACCEL_TAKEOFF_STEP 0.01f
#    endif
#    ifndef MACCEL_GROWTH_RATE_STEP
#        define MACCEL_GROWTH_RATE_STEP 0.01f
#    endif
#    ifndef MACCEL_OFFSET_STEP
#        define MACCEL_OFFSET_STEP 0.1f
#    endif
#    ifndef MACCEL_LIMIT_STEP
#        define MACCEL_LIMIT_STEP 0.1f
#    endif
#endif

float maccel_get_cpi(void) {
    return g_maccel_config.cpi;
}

float maccel_get_takeoff(void) {
    return g_maccel_config.takeoff;
}
float maccel_get_growth_rate(void) {
    return g_maccel_config.growth_rate;
}
float maccel_get_offset(void) {
    return g_maccel_config.offset;
}
float maccel_get_limit(void) {
    return g_maccel_config.limit;
}
void maccel_set_cpi(float val) {
    if (val >= 1) { // 0 zeroes all
        g_maccel_config.cpi = val;
    }
}
void maccel_set_takeoff(float val) {
    if (val >= 0.5) { // value less than 0.5 leads to nonsensical results
        g_maccel_config.takeoff = val;
    }
}
void maccel_set_growth_rate(float val) {
    if (val >= 0) { // value less 0 leads to nonsensical results
        g_maccel_config.growth_rate = val;
    }
}
void maccel_set_offset(float val) {
    g_maccel_config.offset = val;
}
void maccel_set_limit(float val) {
    if (val >= 1) { // limit less than 1 leads to nonsensical results
        g_maccel_config.limit = val;
    }
}

void maccel_enabled(bool enable) {
    g_maccel_config.enabled = enable;
#ifdef MACCEL_DEBUG
    printf("maccel: enabled: %d\n", g_maccel_config.enabled);
#endif
}
bool maccel_get_enabled(void) {
    return g_maccel_config.enabled;
}
void maccel_toggle_enabled(void) {
    maccel_enabled(!maccel_get_enabled());
}

#define _CONSTRAIN(amt, low, high) ((amt) < (low) ? (low) : ((amt) > (high) ? (high) : (amt)))
#define CONSTRAIN_REPORT(val) (mouse_xy_report_t) _CONSTRAIN(val, XY_REPORT_MIN, XY_REPORT_MAX)

report_mouse_t pointing_device_task_maccel(report_mouse_t mouse_report) {
    static uint32_t maccel_timer = 0;
    // rounding carry to recycle dropped floats from int mouse reports, to smoothen low speed movements (credit @ankostis)
    static float rounding_carry_x = 0;
    static float rounding_carry_y = 0;

    // time since last non-zero mouse report
    const uint16_t delta_time = timer_elapsed32(maccel_timer);

    if ((mouse_report.x == 0 && mouse_report.y == 0) || !g_maccel_config.enabled) {
        if (delta_time > MACCEL_ROUNDING_CURRY_TIMEOUT_MS) {
            rounding_carry_x = rounding_carry_y = 0;
        }
        return mouse_report;
    }

    // Reset timer on non-zero reports.
    maccel_timer = timer_read32();

    // Limit expensive calls to get device cpi settings only when mouse stationary for some time.
    static uint16_t device_cpi = 300;
    if (delta_time > MACCEL_CPI_THROTTLE_MS) {
        device_cpi = pointing_device_get_cpi();
    }
    // calculate euclidean distance moved (sqrt(x^2 + y^2))
    const float distance_counts = sqrtf(mouse_report.x * mouse_report.x + mouse_report.y * mouse_report.y);
    // calculate delta velocity: dv = distance/dt
    const float velocity_dots = distance_counts / delta_time;
    // correct raw velocity for dpi
    const float velocity_inches = MACCEL_MAGNIFICATION_DPI * velocity_dots / device_cpi;
    // calculate mouse acceleration factor: f(dv) = c - ((c-1) / ((1 + e^(x(x - b)) * a/z)))
    const float k = g_maccel_config.takeoff;
    const float g = g_maccel_config.growth_rate;
    const float s = g_maccel_config.offset;
    const float m = g_maccel_config.limit;
    // acceleration factor: y(x) = M - (M - 1) / {1 + e^[K(x - S)]}^(G/K)
    // Generalised Sigmoid Function, see https://www.desmos.com/calculator/rlh6o2kx2w
    const float maccel_factor = m - (m - 1) / powf(1 + expf(k * (velocity_inches - s)), g / k);

    // Reset carry when pointer swaps direction, to follow promptly user's hand.
    if (mouse_report.x * rounding_carry_x < 0) rounding_carry_x = 0;
    if (mouse_report.y * rounding_carry_y < 0) rounding_carry_y = 0;
    // Apply acceleration, convert hw-DPI-->sw-DPI and account previous quantization carry.
    const float scale = g_maccel_config.cpi * maccel_factor / device_cpi;
    const float new_x = rounding_carry_x + scale * mouse_report.x;
    const float new_y = rounding_carry_y + scale * mouse_report.y;
    // Accumulate carry from difference with next integers (quantization).
    rounding_carry_x = new_x - (int)new_x;
    rounding_carry_y = new_y - (int)new_y;
    // clamp values
    const mouse_xy_report_t x = CONSTRAIN_REPORT(new_x);
    const mouse_xy_report_t y = CONSTRAIN_REPORT(new_y);

// console output for debugging (enable/disable in config.h)
#ifdef MACCEL_DEBUG
    const float distance_out = sqrtf(new_x * new_x + new_y * new_y);
    const float velocity_out = velocity_inches * maccel_factor;
    printf("MACCEL: DPI:%5i Macpi:%7.1f Tko:%6.3f Grw:%6.3f Ofs:%6.3f Lmt:%6.3f | Acc:%7.3f Vin:%7.3f Vout:%+8.3f Din:%3i Dout:%+8.3f\n", device_cpi, g_maccel_config.cpi, g_maccel_config.takeoff, g_maccel_config.growth_rate, g_maccel_config.offset, g_maccel_config.limit, maccel_factor, velocity_inches, velocity_out - velocity_inches, CONSTRAIN_REPORT(distance_counts), distance_out - CONSTRAIN_REPORT(distance_counts));
#endif // MACCEL_DEBUG

    // report back accelerated values
    mouse_report.x = x;
    mouse_report.y = y;

    return mouse_report;
}

#ifdef MACCEL_USE_KEYCODES
static inline float get_mod_step(float step) {
    const uint8_t mod_mask = get_mods();
    if (mod_mask & MOD_MASK_CTRL) {
        step *= 10; // control increases by factor 10
    }
    if (mod_mask & MOD_MASK_SHIFT) {
        step *= -1; // shift inverts
    }
    return step;
}

bool process_record_maccel(uint16_t keycode, keyrecord_t *record, uint16_t toggle, uint16_t cpi, uint16_t takeoff, uint16_t growth_rate, uint16_t offset, uint16_t limit) {
    if (record->event.pressed) {
        if (keycode == toggle) {
            maccel_toggle_enabled();
            return false;
        }
        if (keycode == cpi) {
            maccel_set_cpi(maccel_get_cpi() + get_mod_step(MACCEL_CPI_STEP));
#    ifdef MACCEL_DEBUG
            printf("MACCEL:keycode: MACPI: %.1f tko: %.3f gro: %.3f ofs: %.3f lmt: %.3f\n", g_maccel_config.cpi, g_maccel_config.takeoff, g_maccel_config.growth_rate, g_maccel_config.offset, g_maccel_config.limit);
#    endif // MACCEL_DEBUG
            return false;
        }
        if (keycode == takeoff) {
            maccel_set_takeoff(maccel_get_takeoff() + get_mod_step(MACCEL_TAKEOFF_STEP));
#    ifdef MACCEL_DEBUG
            printf("MACCEL:keycode: MaCpi: %.1f TKO: %.3f gro: %.3f ofs: %.3f lmt: %.3f\n", g_maccel_config.cpi, g_maccel_config.takeoff, g_maccel_config.growth_rate, g_maccel_config.offset, g_maccel_config.limit);
#    endif // MACCEL_DEBUG
            return false;
        }
        if (keycode == growth_rate) {
            maccel_set_growth_rate(maccel_get_growth_rate() + get_mod_step(MACCEL_GROWTH_RATE_STEP));
#    ifdef MACCEL_DEBUG
            printf("MACCEL:keycode: MaCpi: %.1f tko: %.3f GRO: %.3f ofs: %.3f lmt: %.3f\n", g_maccel_config.cpi, g_maccel_config.takeoff, g_maccel_config.growth_rate, g_maccel_config.offset, g_maccel_config.limit);
#    endif // MACCEL_DEBUG
            return false;
        }
        if (keycode == offset) {
            maccel_set_offset(maccel_get_offset() + get_mod_step(MACCEL_OFFSET_STEP));
#    ifdef MACCEL_DEBUG
            printf("MACCEL:keycode: MaCpi: %.1f tko: %.3f gro: %.3f OFS: %.3f lmt: %.3f\n", g_maccel_config.cpi, g_maccel_config.takeoff, g_maccel_config.growth_rate, g_maccel_config.offset, g_maccel_config.limit);
#    endif // MACCEL_DEBUG
            return false;
        }
        if (keycode == limit) {
            maccel_set_limit(maccel_get_limit() + get_mod_step(MACCEL_LIMIT_STEP));
#    ifdef MACCEL_DEBUG
            printf("MACCEL:keycode: MaCpi: %.1f tko: %.3f gro: %.3f ofs: %.3f LMT: %.3f\n", g_maccel_config.cpi, g_maccel_config.takeoff, g_maccel_config.growth_rate, g_maccel_config.offset, g_maccel_config.limit);
#    endif // MACCEL_DEBUG
            return false;
        }
    }
    return true;
}
#else
bool process_record_maccel(uint16_t keycode, keyrecord_t *record, uint16_t toggle, uint16_t cpi, uint16_t takeoff, uint16_t growth_rate, uint16_t offset, uint16_t limit) {
    // provide a do-nothing keyrecord function so a user doesn't need to un-shim when disabling the keycodes
    return true;
}
#endif

// provide weak do-nothing shims so users do not need to un-shim when disabling via
__attribute__((weak)) void keyboard_post_init_maccel(void) {
    return;
}
