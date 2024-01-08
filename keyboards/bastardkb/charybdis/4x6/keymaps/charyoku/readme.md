# Charybdis 4x6 VIA-Miryoku-ColemakDH keymap

A *ColemakDH-esque* keymap for the "big" *Charybdis 4x6*, with *VIA* support,
[home row mods](https://precondition.github.io/home-row-mods) and
[Miryoku-inspired layers](https://github.com/manna-harbour/miryoku).

It was announced in [this *BastardKBD* discord thread](https://discordapp.com/channels/681309835135811648/1193692606568333342)
on Jan 2024. It forked-off from *Charybdis 3x5* QWERTY based layout,
a much smaller split ergo keyboard which physically contains the exact same number of
keys as the original Miryoku layout.  The extra keys were decided with ideas from
the [Engram layout](https://sunaku.github.io/engram-keyboard-layout.html) regarding
the placement of symbols in the 2 outer columns:

```txt
`~ 1! 2@ 3# 4$ 5%    6^ 7& 8* 9/ 0? -_
[{ qQ wW fF pP bB    jJ lL uU yY \| ]}
'( aA rR sS tT gG    mM nN eE iI oO ")
,< zZ xX cC dD vV    kK hH ;: /? =+ .>

     Media / ESC     Symbols / ENTER
Navigation / SPACE   Numeral / BACKSPACE
     Mouse / TAB       FKeys / DELETE
      RAlt / APP
         ?????
```

## Features

- [x] VIA enabled
- [x] RGB matrix
- [ ] ColemakDH
- [ ] all x6 Miryoku-inspired layers (media, nav, mouse, sym, num, fun)
- [ ] non-QWERTY shift-pairs (`'(` and `")`)
- [ ] Auto-mouse, scroll, snipping
- [ ] Achordion (or bilateral-combinations) home-fow-mods
- ...

---

Original Charybdis 3x5 readme follows.

This layout supports RGB matrix. However, due to space constraints on the MCU,
only a limited number of effect can be enabled at once. Look at the `config.h` file and
enable your favorite effect.

## Customizing the keymap

### Dynamic DPI scaling

Use the following keycodes to change the default DPI:

-   `POINTER_DEFAULT_DPI_FORWARD`: increases the DPI; decreases when shifted;
-   `POINTER_DEFAULT_DPI_REVERSE`: decreases the DPI; increases when shifted.

There's a maximum of 16 possible values for the sniping mode DPI. See the [Charybdis documentation](../../README.md) for more information.

Use the following keycodes to change the sniping mode DPI:

-   `POINTER_SNIPING_DPI_FORWARD`: increases the DPI; decreases when shifted;
-   `POINTER_SNIPING_DPI_REVERSE`: decreases the DPI; increases when shifted.

There's a maximum of 4 possible values for the sniping mode DPI. See the [Charybdis documentation](../../README.md) for more information.

### Drag-scroll

Use the `DRAGSCROLL_MODE` keycode to enable drag-scroll on hold. Use the `DRAGSCROLL_TOGGLE` keycode to enable/disable drag-scroll on key press.

### Sniping

Use the `SNIPING_MODE` keycode to enable sniping mode on hold. Use the `SNIPING_TOGGLE` keycode to enable/disable sniping mode on key press.

Change the value of `CHARYBDIS_AUTO_SNIPING_ON_LAYER` to automatically enable sniping mode on layer change. By default, sniping mode is enabled on the pointer layer:

```c
#define CHARYBDIS_AUTO_SNIPING_ON_LAYER LAYER_POINTER
```

### Auto pointer layer

The pointer layer can be automatically enabled when moving the trackball. To enable or disable this behavior, add or remove the following define:

```c
#define CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_ENABLE
```

By default, the layer is turned off 1 second after the last registered trackball movement:

```c
#define CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_TIMEOUT_MS 1000
```

The trigger sensibility can also be tuned. The lower the value, the more sensible the trigger:

```c
#define CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_THRESHOLD 8
```

## Layout

![Keymap layout (generated with keyboard-layout-editor.com)](https://i.imgur.com/uHEnqEN.png)
