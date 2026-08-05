# RFD 0074: A caption label over each billboard card

Developed here, in its own repository, on `weftspun`'s own
convention for a feature too large for one session. Moves back to
[weftspun/weftspun-3d-studio](https://github.com/weftspun/weftspun-3d-studio)'s
`decisions/0074-3d-billboard-labels/` as a pull request once the
open questions below settle and code exists to review.

**State:** prediscussion
**Scope:** `usd_viewer_app/`, RFD 0073's gallery, in the parent repo

## Problem

RFD 0073's billboard shows an image, with no caption. The user
asked for something like Godot's `Label3D`, text that floats in 3D
space and always faces the camera, tied to a card's own position.
`alfredplpl/anime-with-caption-cc0`, RFD 0064's own dataset, already
gives each row a caption. Nothing shows it today.

## Decision

Not decided yet. `AlphaPixel/SlugHorn`, the user's own suggestion,
does real GPU vector-text rendering, the Slug technique, but it is a
C++20 library with no WASM build, no browser binding, and no
renderer of its own. Adopting it means building both a WASM target
and a three.js-side Slug renderer first, real work, not a small
addition. See `DETAILS.md` for that reasoning and for a much
smaller stopgap already available today: an ordinary HTML overlay,
positioned every frame with the camera's own projection math, no
new WASM build, no dependency on an unfinished library.

This RFD exists to pick between those two paths, or name a third,
before any code lands, not to announce a decision already made.

## Related

RFD 0073 gives the billboard card this label sits over. RFD 0053
makes USD the internal format and glTF/VRM the transmission one;
this RFD's labels live in the browser client only, not in USD
itself, the same deliberate scoping RFD 0073 already uses.
