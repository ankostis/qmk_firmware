# Charybdis 4x6 VIA-Miryoku-ColemakDH keymap

A *ColemakDH-esque* keymap for the "big" *Charybdis 4x6*, with *VIA* support,
[home row mods](https://precondition.github.io/home-row-mods) and
[Miryoku-inspired layers](https://github.com/manna-harbour/miryoku).

It was announced in [this *BastardKBD* discord thread](https://discordapp.com/channels/681309835135811648/1193692606568333342)
on Jan 2024. It forked-off from *Charybdis 3x5* QWERTY based layout,
a smaller split ergo keyboard which physically contains the exact same number of
keys as the original *Miryoku* layout.  The extra keys were filled like this:

- top row: `F1`-`F12` to facilitate app shortcuts
  with one hand, eg. *vscode*, *midnight commander*.
- bottom left thumbs: space/tab for both sym & num layers.
- outer columns: ideas from the [Engram layout](https://sunaku.github.io/engram-keyboard-layout.html)
  for programmers.

```txt
F11 F1  F2  F3  F4  F5    F6  F7  F8  F9  F10 F12
Ap  qQ  wW  fF  pP  bB    jJ  lL  uU  yY  '"  /?
Pr  aA  rR  sS  tT  gG    mM  nN  eE  iI  oO  ;:
↥   zZ  xX  cC  dD  vV    kK  hH  -_  ,<  .>  ⇈

media/ESC                               num/Bksp
    nav/SPACE                       sym/Del
        mouse/TAB

   RAlt/;                            fun/Enter
       RCtrl/=
```

## Deviations from Miryoku
 
- Swap `,` with `-`, the former being much usefull in programming, the latter's place
  in the num+left thumb rhymes with `.` also there when num-typing.
- Function-keys in place of the numbers (top row) to facilitate programming shortcuts
  with one hand, eg. *vscode*, *midnight commander*.
- Navigation keys are layed-out in a "cross" pattern.  Many changes in nav & pointer
  layers.

## Features

- [x] VIA enabled
- [x] RGB matrix
- [x] ColemakDH
- [x] all x6 Miryoku-inspired layers (media, nav, mouse, sym, num, fun)
- [ ] single-handed pointer (trackball) with mouse-buttons ergonomically close to home-row
- [ ] x2 non-QWERTY shift-pairs (`'(` and `")`)
- [ ] Decide auto-mouse impl (core vs charybdis), scroll, (auto)snipping
- [ ] Achordion (or bilateral-combinations) home-row-mods
- [ ] layer-state RGB indicators
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
