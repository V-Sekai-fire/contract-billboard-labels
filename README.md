# RFD 0074: A caption label over each billboard card

Developed here, in its own repository, on `weftspun`'s own
convention for a feature too large for one session. Moves back to
[weftspun/weftspun-3d-studio](https://github.com/weftspun/weftspun-3d-studio)'s
`decisions/0074-3d-billboard-labels/` as a pull request once the
open questions below settle and code exists to review.

**State:** decided (SlugHorn/WASM), prototype proves the pipeline; font
loading, multi-shape batching, and the Godot GDExtension binding remain
**Scope:** `usd_viewer_app/`, RFD 0073's gallery, in the parent repo

## Problem

RFD 0073's billboard shows an image, with no caption. The user
asked for something like Godot's `Label3D`, text that floats in 3D
space and always faces the camera, tied to a card's own position.
`alfredplpl/anime-with-caption-cc0`, RFD 0064's own dataset, already
gives each row a caption. Nothing shows it today.

## Decision

**`AlphaPixel/SlugHorn`, via a WASM build.** The deciding requirement,
added after the HTML-overlay stopgap was already prototyped: the user
wants this label technique portable to Godot, not just the browser
client. An ordinary HTML overlay is a DOM technique; it cannot port to
Godot at all. SlugHorn's core is plain C++20 with no browser or engine
dependency baked in, so it is the same source compiled two ways: a WASM
build for `usd-viewer`'s three.js scene, and (future session) a
GDExtension binding for Godot. See `DETAILS.md` for the prototype that
proves this path works, not just that it should.

The HTML-overlay stopgap (`prototype/html-overlay-poc.html`) stays in
the repo as the documented baseline it was built as, superseded by this
decision rather than deleted.

## Related

RFD 0073 gives the billboard card this label sits over. RFD 0053
makes USD the internal format and glTF/VRM the transmission one;
this RFD's labels live in the browser client only, not in USD
itself, the same deliberate scoping RFD 0073 already uses.
