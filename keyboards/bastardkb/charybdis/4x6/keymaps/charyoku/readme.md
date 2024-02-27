# Charybdis 4x6 VIA-Miryoku-ColemakDH keymap

A *ColemakDH-esque* keymap for the "big" *Charybdis 4x6*, with *VIA* support,
[home row mods](https://precondition.github.io/home-row-mods) and
[Miryoku-inspired layers](https://github.com/manna-harbour/miryoku):

[![succinct charyoku layers](assets/charyoku-kle-reference.png)](http://www.keyboard-layout-editor.com/#/gists/57926252da9ebaea7a88cba9ed3bfa78)

It was announced in [this *BastardKBD* discord thread](https://discordapp.com/channels/681309835135811648/1193692606568333342)
on Jan 2024. It forked-off from *Charybdis 3x5* QWERTY based layout,
a smaller split ergo keyboard which physically contains the exact same number of
keys as the original *Miryoku* layout.  The extra keys do not contain any 
essential functionatlity, to have a gracefull degradation to 3x5 form,
like this:

- top row: Put F-keys in place of number-keys (top non-miryoku row) to facilitate
  app shortcuts with one hand, eg. *midnight commander*, *vscode*.  Besides,
  a non-char key is handy in Esc's position ;-)
- x2 bottom left thumbs: `;,` in base layer and so as to have all QWERTY keys exposed
  without layer;  space/tab for both sym & num layers.
- outer columns: caps-lock keys and nice-to-have keys like `;` and `/`.

## Deviations from Miryoku
 
- Swap `,` with `-`, the former being much usefull in programming, the latter's place
  in the num+left thumb rhymes with `.` also there when num-typing.  
- Navigation keys are layed-out in a "cross" pattern;  many changes in nav & pointer
  layers are due to this.

## Features

- [x] VIA enabled
- [x] RGB matrix
- [x] ColemakDH
- [x] all x6 Miryoku-inspired layers (media, nav, mouse, sym, num, fun)
- [x] Only QWERTY shift-pairs
- [x] single-handed pointer (trackball) with mouse-buttons ergonomically close to home-row
- [x] layer-state & capslock RGB indicators
- [x] lock layers with thumb combos
- [x] Charybdis auto-mouse impl, scroll, (auto)snipping
- [x] Generalised sigmoid mouse & drag-scroll acceleration
  NOTE: maccel is not integrated yet as officially suggested,
  but facilitates experimentation with fast builds.
  - [ ] maccel configed through *via*
- [ ] Opinionated on same-side keys (unassigned in miryoku)
- [ ] Achordion (or bilateral-combinations) home-row-mods
- ...

## Layout

[![charyoku layers](assets/charyoku-layers.svg)](https://caksoylar.github.io/keymap-drawer?keymap_yaml=H4sIAAAAAAAC_9VZW1MjRRR-51ccW5RVmktCYCFeIWxYFNwYYHVFFieZJkllkhnnAptCLN983Dd92p-gVe7Deil_Db_Ec7o7Q-aasO4qUDWTnv76XPpyvu4-WMbADvzyFMA5fNvrHnfFoGEbrlmGhuH5WOg2Fpptwx00zI63UHqysnBaXPAcq9PvDo6XuJSxlA7YWX_04GAfLqawQrgeKUUlgn4B5uCwWihwqOJzDti6WuTQLgOT2u1uwOBiiCwp5HRxvjRfkPXVEj7LR6GmFfy8i88qPmukdJFexbDBuuNw-JzDF1jNocZhI4Q-4bDD4YDDI22OzTAOXhlm2EzoAltQdewj_PUHjihDq217PlyEeuQv_dVcf6_Jw08pvy57sLN1sI0qo1BdQeuWn4D2FFTxXSuB7Stsr905iQhuDYtJx3ZjKj6TKup71f2E9nsKquzvJKDtbH8fZPeSvacHsBwfwIS_FcPx4Mx2TQ5fcfiSQ4XDJoeHyQ59emXkftzenLZ3zJK-cI29z6TDPTvwRLLVvG71oWrVN05H20gvLbvZzR5vNZJeUxkRZsdIGpme7pmd8jDK5jzHaIrp6VSLatqNRp7PMXWu8AO3P9TnDXoJCSXQMJpdbVpZDnopE5Mzo7RYcEVsuRmTO5T5QMvMahla2mNENoWlGp8E_VGnsIADFJLJhm37HOqiIX-ZEI5r96BiCcNFcdTSCFrg262WJegbH5yZsGtyllhaw5ii0ADZw_k3TOGyUA3TqqNPCNeFaSPV9Ol9-dNzDtt9T7iordY6cFQNzm_YXPZfsYkeIt81-h6REsUYl0HIFT9wTQURezWkbIGmfv6B7P1NrxdkbNM-66uKrPUX9WEkImN-qL5UArRbsZ2B6rE0m1w11Orqg1qHX-y-3RNw-eNvLD779_om1j_PDlTpT1tYZmLlXEVozOvMdU2rYDLdkTDM0A854RhvMhKA7CXiLtOD8VGXEMF4iwSZ7GIYZpF1f6vi7dkfu_T-S7V6LXH27Pc6vV_s0PtP7c6_jLCwW16_44iRgTJdIzLA1ws4Ke01Xduy2Pg1HzP-knEmJ10NT8xQ3saVEiGhxuGI09TiulWbQtC7oas1diSkI2lu2B7qsD1POcLc1djbKdiqxt5NwdY0dicFO9LYBcsjSRlU4ZcMrky6Sdq4OkcnsJmvZ9LkiGrzGTE1dDNJM4mVNDadgi1r7K0UbEVjj1ke5aaMpeaLIZMMiUWxjGScsawRMxfyBxszDt9ov75P8bmgsTdSsKLGPk7BljT2Zs6cfpe7pnKY6jqn-bwzfBILeYtNfEAYr3RRY-9MvPYnOCSk0_INOR_greKG8m0WzRKjcqJOThzJ4Q4nzruZTJdzNpETjPc15CYOjznMvkp-GUcrxCCcqIITJ3AKfk5RfutiG3dCTuHKVW4nfjBJHD5uSNThZbic4cl_TPHX3j6zRjBvO30dx508qs7dvjN5pVooRvOQMnAnva3khLrKlMqcJz4rHPbkyV2u8Ij6nJDPSsNpI5PeR3Rqlct8LWVmawbeD3P7mHtLSK4GGtlJs2QZ0TLCpBNtsMks2PX32MkCfHz2Ql26KVt4Oy_dbqsBjn0mXA5UbAdCFTzD11U929RVjhBYZFTWzry6S_nl0185PLStOQ67AW2HWJ7Fy-LTX5Txk86TEWO5W112Z-lzo769qQAsHaSFdWRHqVnGAAPYt53hm2r-p22FjWSbppp2r2HLf9PMoS6nDIdLSDWl1SMOXVSoMth6U7UQpX_mIGaUwRL0T4i44JoWpDx2qljD9n1cdnHB5cWhRZnvSxV1O6121GQJeXe5oCUp1T2Zp1KsOPQ06E3sqRRc1oKUm8718x_soPjdWBsAAA%3D%3D)

Original generated with this commands and heavily hand-tweaked yaml:

```shell
qmk c2json | \
    tee keyboards/bastardkb/charybdis/4x6/keymaps/charyoku/assets/charyoku-layers.json | \
    keymap parse -q | keymap draw - > keyboards/bastardkb/charybdis/4x6/keymaps/charyoku/assets/charyoku-layers.svg
```


 
## Tips

- Store puncts in toggling-side for SYM/NUM so as to type easier `!=` and regexes like
  `/.*\\n|#.*//` or paths like `~/.*` without changing from *Sym* layer.
- [![accelaration-profiles](assets/maccel-curve.png)](https://www.desmos.com/calculator/xkhejelty8)

enjoy!

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
