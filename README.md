# Modality


## Overview

Modality is a modal music step sequencer, heavily inspired by Vim.

`Modal` as in:

- Reflecting the approach of Vim's modal editing, where the computer keyboard functions completely differently depending on the selected editor mode

- As related to musical scales

It provides manual and algorithmic sequencing through vim-style keybindings and functionality. Modality sequences MIDI notes to be sent to external devices or software and does not generate sound itself.

## Main Modes

- Normal Mode
- Insert Mode
- Visual Block Mode
- Visual Line Mode

## Contextual menus and settings

Modality uses a contextual menu system inspired by terminal based file managers like Ranger. The menu system is optimised for keyboard navigation, where menu layers are entered into via a single key corresponding to that choice - e.g. `s` for scale.

- Sequence settings
- Modifier settings

When it comes to editing values of individual settings, vim-style motions apply. For example:

- `^` - sets value to `0` [similar to line start]
- `$` - sets value to `100`, or highest value. [similar to line end]
- `M` - sets value to middle point


## Acknowledgments and Inspirations

Developed initially during a half-batch [Recurse](https://www.recurse.com/) retreat.

- [Vim](https://github.com/vim/vim) / [Neovim](https://github.com/neovim/neovim)
- [Ranger](https://github.com/ranger/ranger)
- [Tidal Cycles](https://tidalcycles.org/)
