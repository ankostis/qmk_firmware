# Charyoku Changes

## v0.4.2, 27 Feb 2024 - layer reorder, layer-diagrams, doc improvements

- feat: parenthensis in NUM bottom-L-thumbs, to allow typing expressions
  without lifting the R-thumb. 
- doc: added `keymap-drawer` layer diagrams to the readme.

## v0.4.1, 25 Feb 2024 - layer reorder

## v0.4.0, 23 Feb 2024 - maccel for mouse & drag-scroll, DPI stability

- feat: more punctuatios & symbols in toggling-side for *sym*/*num* layers,
  so as to type `!=` & regexes like `/.*\\n|#.*//` without switching those layers.
- BREAK: updated maccel param to recent shorter names.
- fix: workaround DPI stability issues by avoiding the DPI change between
  mouse & drag-scroll alltogether;  both are now set to 400;
  to set your mouse speed, instead of fiddling with DPIs, start by resetting 
  your OS's mouse settings to base and then ++/-- to your liking.
- feat: by default, coalesce scroll-wheel events to values above ±1
  (changed from queuing ±1 events).
- Doc: generate all layers with `keymap-drawer`.
- doc: explain config options & alternatives in the sources where they are defined.
- chore: polished git history, reasoning added in the commit messages.
