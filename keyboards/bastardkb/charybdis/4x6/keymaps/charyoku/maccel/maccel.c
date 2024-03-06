// Copyright 2024 burkfers (@burkfers)
// Copyright 2024 Wimads (@wimads)
// SPDX-License-Identifier: GPL-2.0-or-later

#include "quantum.h" // IWYU pragma: keep
#include "maccel.h"
#include "math.h"

/**
 * MACCEL_CPI (C)
 *
 * The DPI desired from the maccel algo, independent of device's CPI setting
 * (the latter is free to control mouse's hardware accuracy).
 *
 * --/++ value --> pointer speedier/slower
 *
 * It can be considered a device specific parameter
 * (apart from a user-comfort control).
 * It is NOT used by the curve, not to burden tuning it's parameters.
 */
#ifndef MACCEL_CPI
#    ifdef POINTING_DEVICE_DRIVER_pmw3360
#        define MACCEL_CPI 120.0
#    elif POINTING_DEVICE_DRIVER_cirque_pinnacle_spi
#        define MACCEL_CPI 120.0
#    else
#        warning "Unsupported pointing device driver! Please manually set the scaling parameter MACCEL_CPI to achieve a consistent acceleration curve!"
#        define MACCEL_CPI 120.0
#    endif
#endif
#ifdef MACCEL_SCALE
#  error "You must rename MACCEL_SCALE as MACCEL_CPI in your `config.h`!"
#endif // MACCEL_SCALE
#ifndef MACCEL_TAKEOFF
// https://www.desmos.com/calculator/vtfkbxwj8s
#    define MACCEL_TAKEOFF     6.8     // (K) --/++ value --> curve starts smoothlier/abruptlier
#endif
#ifndef MACCEL_GROWTH_RATE
#    define MACCEL_GROWTH_RATE 0.6     // (M) --/++ value --> max limit reached slower/faster
#endif
#ifndef MACCEL_OFFSET
#    define MACCEL_OFFSET      1.0     // (S) --/++ value --> growth kicks in earlier/later
#endif
#ifndef MACCEL_LIMIT
#    define MACCEL_LIMIT       9.6     // (M) maximum acceleration factor
#endif
#ifndef MACCEL_CPI_THROTTLE_MS
#    define MACCEL_CPI_THROTTLE_MS 200 // milliseconds to wait between requesting the device's current DPI
#endif
#ifndef MACCEL_ROUNDING_CURRY_TIMEOUT_MS
#    define MACCEL_ROUNDING_CURRY_TIMEOUT_MS 300 // mouse report delta time after which quantization carry gets reset
#endif
/**
 * Scale acceleration curve's v-input so that its params are not uncannily small
 * and floats do not overflow in the accel formula (eg. `exp(709.8)` for doubles).
 * Eventually the accel formula is calculated as if mouse reports @ 1000 DPI,
 * but that being a float.
 */
#define MACCEL_MAGNIFICATION_DPI 1000

maccel_config_t g_maccel_config = {
    // clang-format off
    .scaling =      MACCEL_CPI,
    .growth_rate =  MACCEL_GROWTH_RATE,
    .offset =       MACCEL_OFFSET,
    .limit =        MACCEL_LIMIT,
    .takeoff =      MACCEL_TAKEOFF,
    .enabled =      true
    // clang-format on
};

#ifdef MACCEL_USE_KEYCODES
#    ifndef MACCEL_TAKEOFF_STEP
#        define MACCEL_CPI_STEP 20
#    endif
#    ifndef MACCEL_TAKEOFF_STEP
#        define MACCEL_TAKEOFF_STEP 0.2f
#    endif
#    ifndef MACCEL_GROWTH_RATE_STEP
#        define MACCEL_GROWTH_RATE_STEP 0.01f
#    endif
#    ifndef MACCEL_OFFSET_STEP
#        define MACCEL_OFFSET_STEP 0.1f
#    endif
#    ifndef MACCEL_LIMIT_STEP
#        define MACCEL_LIMIT_STEP 0.2f
#    endif
#endif

float maccel_get_scaling(void) {
    return g_maccel_config.scaling;
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
void maccel_set_scaling(float val) {
    if (val >= 1) { // 0 zeroes all
        g_maccel_config.scaling = val;
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
    static uint32_t last_report_time = 0;
    static uint32_t last_move_time = 0;
    static float rounding_carry_x = 0;
    static float rounding_carry_y = 0;

    const uint16_t report_t_delta = timer_elapsed32(last_report_time);
    last_report_time = timer_read32();
    const uint16_t move_t_delta = timer_elapsed32(last_move_time);

    if ((mouse_report.x == 0 && mouse_report.y == 0) || !g_maccel_config.enabled) {
        return mouse_report;
    }

    last_move_time = timer_read32();

    // Avoid expensive call to get-device-cpi unless mouse stationary for > 200ms.
    static uint16_t device_dpi = 300;
    // if (report_t_delta > MACCEL_CPI_THROTTLE_MS) {
    if (move_t_delta > MACCEL_CPI_THROTTLE_MS) {
        device_dpi = pointing_device_get_cpi();
        // do the junk-fix until merge with qbk-upstream
        pointing_device_set_cpi(device_dpi);
    }
    if (move_t_delta > MACCEL_ROUNDING_CURRY_TIMEOUT_MS) {
        rounding_carry_x = rounding_carry_y = 0;
    }

    // Dump x & y movements in 10,000th of an inch.
    // Ie. for a typical ball speed x1 inch/sec and pointer-task duty-cycle 1ms,
    // it should print 10.
    const float d_inch_x = (float) mouse_report.x / device_dpi;
    const float d_inch_y = (float) mouse_report.y / device_dpi;
    const float v_inch_x =  d_inch_x / report_t_delta;
    const float v_inch_y =  d_inch_y / report_t_delta;
    printf("MACCEL: DPI:%5i Dinch: %7.3f Vinch: %7.3f\n",
    device_dpi, d_inch_x * 1000, v_inch_x * 1000);
    printf("MACCEL: DPI:%5i Dinch: %7.3f Vinch: %7.3f\n",
    device_dpi, d_inch_y * 1000, v_inch_y * 1000);

    const mouse_xy_report_t x_dots = mouse_report.x;
    const mouse_xy_report_t y_dots = mouse_report.y;
    // euclidean distance: sqrt(x^2 + y^2)
    const float distance_dots = sqrtf(x_dots * x_dots + y_dots * y_dots);
    const float distance_inch = MACCEL_MAGNIFICATION_DPI * distance_dots / device_dpi;
    const float velocity = distance_inch / report_t_delta;
    // acceleration factor: y(x) = M - (M - 1) / {1 + e^[K(x - S)]}^(G/K)
    // Design generalised sigmoid: https://www.desmos.com/calculator/grd1ox94hm
    const float k             = g_maccel_config.takeoff;
    const float g             = g_maccel_config.growth_rate;
    const float s             = g_maccel_config.offset;
    const float m             = g_maccel_config.limit;
    const float maccel_factor = m - (m - 1) / powf(1 + expf(k * (velocity - s)), g / k);
    // Reset carry when pointer swaps direction, to quickly follow user's hand.
    if (mouse_report.x * rounding_carry_x < 0) rounding_carry_x = 0;
    if (mouse_report.y * rounding_carry_y < 0) rounding_carry_y = 0;
    // Convert mouse-report.x/y also to inches and account quantization carry.
    const float de_cpi_n_scale = g_maccel_config.scaling / device_dpi;
    const float x_new          = rounding_carry_x + maccel_factor * x_dots * de_cpi_n_scale;
    const float y_new          = rounding_carry_y + maccel_factor * y_dots * de_cpi_n_scale;
    // Add any remainder from the reported x/y integers as quantization-carry.
    rounding_carry_x = x_new - (int)x_new;
    rounding_carry_y = y_new - (int)y_new;
    // Clamp values and report back accelerated values.
    mouse_report.x = CONSTRAIN_REPORT(x_new);
    mouse_report.y = CONSTRAIN_REPORT(y_new);

// console output for debugging (enable/disable in config.h)
#ifdef MACCEL_DEBUG
    // const float distance_accel = sqrtf(x_new * x_new + y_new * x_new);
    const float distance_out = sqrtf(mouse_report.x * mouse_report.x + mouse_report.y * mouse_report.y);
    const float velocity_out = velocity * maccel_factor;
    printf("MACCEL: DPI:%5i Scl:%7.1f Tko:%6.3f Grw:%.3f Ofs:%.3f Lmt:%6.3f | Acc:%7.3f v.in:%7.3f v.out:%+8.3f t:%5i d.in:%7.3f d.out:%7.3f\n",
    device_dpi, g_maccel_config.scaling, g_maccel_config.takeoff, g_maccel_config.growth_rate, g_maccel_config.offset, g_maccel_config.limit,
    maccel_factor, velocity, (velocity_out - velocity), report_t_delta, distance_dots, distance_out);
#endif // MACCEL_DEBUG

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

bool process_record_maccel(uint16_t keycode, keyrecord_t *record, uint16_t scaling, uint16_t takeoff, uint16_t growth_rate, uint16_t offset, uint16_t limit) {
    if (record->event.pressed) {
        if (keycode == scaling) {
            maccel_set_scaling(maccel_get_scaling() + get_mod_step(MACCEL_CPI_STEP));
#    ifdef MACCEL_DEBUG
            printf("MACCEL:keycode: SCL: %.1f tko: %.3f gro: %.3f ofs: %.3f lmt: %.3f\n", g_maccel_config.scaling, g_maccel_config.takeoff, g_maccel_config.growth_rate, g_maccel_config.offset, g_maccel_config.limit);
#    endif // MACCEL_DEBUG
            return false;
        }
        if (keycode == takeoff) {
            maccel_set_takeoff(maccel_get_takeoff() + get_mod_step(MACCEL_TAKEOFF_STEP));
#    ifdef MACCEL_DEBUG
            printf("MACCEL:keycode: scl: %.1f TKO: %.3f gro: %.3f ofs: %.3f lmt: %.3f\n", g_maccel_config.scaling, g_maccel_config.takeoff, g_maccel_config.growth_rate, g_maccel_config.offset, g_maccel_config.limit);
#    endif // MACCEL_DEBUG
            return false;
        }
        if (keycode == growth_rate) {
            maccel_set_growth_rate(maccel_get_growth_rate() + get_mod_step(MACCEL_GROWTH_RATE_STEP));
#    ifdef MACCEL_DEBUG
            printf("MACCEL:keycode: scl: %.1f tko: %.3f GRO: %.3f ofs: %.3f lmt: %.3f\n", g_maccel_config.scaling, g_maccel_config.takeoff, g_maccel_config.growth_rate, g_maccel_config.offset, g_maccel_config.limit);
#    endif // MACCEL_DEBUG
            return false;
        }
        if (keycode == offset) {
            maccel_set_offset(maccel_get_offset() + get_mod_step(MACCEL_OFFSET_STEP));
#    ifdef MACCEL_DEBUG
            printf("MACCEL:keycode: scl: %.1f tko: %.3f gro: %.3f OFS: %.3f lmt: %.3f\n", g_maccel_config.scaling, g_maccel_config.takeoff, g_maccel_config.growth_rate, g_maccel_config.offset, g_maccel_config.limit);
#    endif // MACCEL_DEBUG
            return false;
        }
        if (keycode == limit) {
            maccel_set_limit(maccel_get_limit() + get_mod_step(MACCEL_LIMIT_STEP));
#    ifdef MACCEL_DEBUG
            printf("MACCEL:keycode: scl: %.1f tko: %.3f gro: %.3f ofs: %.3f LMT: %.3f\n", g_maccel_config.scaling, g_maccel_config.takeoff, g_maccel_config.growth_rate, g_maccel_config.offset, g_maccel_config.limit);
#    endif // MACCEL_DEBUG
            return false;
        }
    }
    return true;
}
#else
bool process_record_maccel(uint16_t keycode, keyrecord_t *record, uint16_t scaling, uint16_t takeoff, uint16_t growth_rate, uint16_t offset, uint16_t limit) {
    // provide a do-nothing keyrecord function so a user doesn't need to unshim when disabling the keycodes
    return true;
}
#endif

// provide weak do-nothing shims so users do not need to unshim when diabling via
__attribute__((weak)) void keyboard_post_init_maccel(void) {
    return;
}
