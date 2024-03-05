// Copyright 2024 burkfers (@burkfers)
// Copyright 2024 Wimads (@wimads)
// SPDX-License-Identifier: GPL-2.0-or-later

#include "quantum.h" // IWYU pragma: keep
#include "maccel.h"
#include "math.h"

static uint32_t maccel_timer;

/**
 * MACCEL_SCALE (C)
 *
 * It corresponds to the DPI desired to drive the accel curve, that is
 * how many dots (±1) to push into the curve for an inch of mouse-drift.
 * This is independent of device's CPI setting (which now can be set freely,
 * to control the hardware accuracy of the mouse alone).
 *
 * It can be considered a device specific parameter
 * (apart from a user-comfort control).
 *
 * lower/higher value --> pointer speedier/slower
 */
#ifndef MACCEL_SCALE
#    ifdef POINTING_DEVICE_DRIVER_pmw3360
#        define MACCEL_SCALE 220.0
#    elif POINTING_DEVICE_DRIVER_cirque_pinnacle_spi
#        define MACCEL_SCALE 220.0
#    else
#        warning "Unsupported pointing device driver! Please manually set the scaling parameter MACCEL_SCALE to achieve a consistent acceleration curve!"
#        define MACCEL_SCALE 220.0
#    endif
#endif
#ifndef MACCEL_TAKEOFF
#    define MACCEL_TAKEOFF 4.44 // (K) lower/higher value = curve starts more smoothly/abrubtly
#endif
#ifndef MACCEL_GROWTH_RATE
#    define MACCEL_GROWTH_RATE 0.5 // (G) lower/higher value = curve reaches its upper limit slower/faster
#endif
#ifndef MACCEL_OFFSET
#    define MACCEL_OFFSET 1.1 // (S) lower/higher value = acceleration kicks in earlier/later
#endif
#ifndef MACCEL_LIMIT
#    define MACCEL_LIMIT 6.0 // (M) upper limit of accel curve (maximum acceleration factor)
#endif
#ifndef MACCEL_CPI_THROTTLE_MS
#    define MACCEL_CPI_THROTTLE_MS 200 // milliseconds to wait between requesting the device's current DPI
#endif
#ifndef MACCEL_ROUNDING_CURRY_TIMEOUT_MS
#    define MACCEL_ROUNDING_CURRY_TIMEOUT_MS 12000 // mouse report delta time after which quantization carry gets reset
#endif

maccel_config_t g_maccel_config = {
    // clang-format off
    .scaling =      MACCEL_SCALE,
    .growth_rate =  MACCEL_GROWTH_RATE,
    .offset =       MACCEL_OFFSET,
    .limit =        MACCEL_LIMIT,
    .takeoff =      MACCEL_TAKEOFF,
    .enabled =      true
    // clang-format on
};

#ifdef MACCEL_USE_KEYCODES
#    ifndef MACCEL_TAKEOFF_STEP
#        define MACCEL_SCALE_STEP 20
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
    static float rounding_carry_x = 0;
    static float rounding_carry_y = 0;

    const uint16_t time_delta = timer_elapsed32(maccel_timer);

    if ((mouse_report.x == 0 && mouse_report.y == 0) || !g_maccel_config.enabled) {
        if (time_delta > MACCEL_ROUNDING_CURRY_TIMEOUT_MS) {
            rounding_carry_x = rounding_carry_y = 0;
        }
        return mouse_report;
    }

    maccel_timer = timer_read32();

    // Reset carry when pointer swaps direction, to follow user's hand.
    if (mouse_report.x * rounding_carry_x < 0) rounding_carry_x = 0;
    if (mouse_report.y * rounding_carry_y < 0) rounding_carry_y = 0;

    // Avoid expensive call to get-device-cpi unless mouse stationary for > 200ms.
    static uint16_t device_dpi = 300;
    if (time_delta > MACCEL_CPI_THROTTLE_MS) {
        device_dpi = pointing_device_get_cpi();
        // do the junk-fix until merge with qbk-upstream
        pointing_device_set_cpi(device_dpi);
    }

    const mouse_xy_report_t x_dots = mouse_report.x;
    const mouse_xy_report_t y_dots = mouse_report.y;
    // euclidean distance: sqrt(x^2 + y^2)
    const float distance_dots = sqrtf(x_dots * x_dots + y_dots * y_dots);
    const float distance_inch = distance_dots / device_dpi;
    const float velocity_inch = distance_inch / time_delta;
    // Scale velocity in the range where the acceleration curve grows.
    const float velocity = velocity_inch * g_maccel_config.scaling;
    // acceleration factor: y(x) = M - (M - 1) / {1 + e^[K(x - S)]}^(G/K)
    // Design generalised sigmoid: https://www.desmos.com/calculator/sodvw10g89
    const float k             = g_maccel_config.takeoff;
    const float g             = g_maccel_config.growth_rate;
    const float s             = g_maccel_config.offset;
    const float m             = g_maccel_config.limit;
    const float maccel_factor = m - (m - 1) / powf(1 + expf(k * (velocity - s)), g / k);
    // Convert mouse-report.x/y also to inches and account quantization carry.
    const float de_cpi_n_scale = g_maccel_config.scaling / device_dpi;
    const float x_new          = rounding_carry_x + maccel_factor * x_dots * de_cpi_n_scale;
    const float y_new          = rounding_carry_y + maccel_factor * y_dots * de_cpi_n_scale;
    // Add any remainder from the reported x/y integers as quantization-carry.
    rounding_carry_x = x_new - (int)x_new;
    rounding_carry_y = y_new - (int)y_new;
    // Clamp values and report back accelerated values.
    const mouse_xy_report_t x = mouse_report.x = CONSTRAIN_REPORT(x_new);
    const mouse_xy_report_t y = mouse_report.y = CONSTRAIN_REPORT(y_new);

// console output for debugging (enable/disable in config.h)
#ifdef MACCEL_DEBUG
    // const float distance_accel = sqrtf(x_new * x_new + y_new * x_new);
    const float distance_out = sqrtf(x * x + y * y);
    const float velocity_out = velocity * maccel_factor;
    printf("MACCEL: DPI:%5i Scl:%7.1f Tko: %6.3f Grw: %.3f Ofs: %.3f Lmt: %6.3f | Fct: %7.3f v.in: %7.3f v.out: %+7.3f d.in: %7.3f d.out: %7.3f\n", device_dpi, g_maccel_config.scaling, g_maccel_config.takeoff, g_maccel_config.growth_rate, g_maccel_config.offset, g_maccel_config.limit, maccel_factor, velocity, velocity_out - velocity, distance_inch, distance_out);
/*
 */
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
            maccel_set_scaling(maccel_get_scaling() + get_mod_step(MACCEL_SCALE_STEP));
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
