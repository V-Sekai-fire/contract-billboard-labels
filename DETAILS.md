# RFD 0074 details: what SlugHorn actually is, and the smaller path beside it

## What SlugHorn is, read from its own repository

`github.com/AlphaPixel/SlugHorn`, MIT licensed, is a C++20 library
implementing the Slug GPU vector-graphics technique: perspective-correct
text and vector shapes, rendered from Bezier curves, no tessellation
artifacts at any zoom level. It reads fonts and vector art through
FreeType, NanoSVG, Skia, Cairo, and Blend2D, and converts them into a
glTF-compatible, GPU-ready curve format.

Its own description calls it "a unified source of truth for vector
data," explicitly not a graphics engine. It names OpenGL, Vulkan,
WebGL, WebGPU, DirectX, and Metal as technically reachable backends,
but ships no code for any of them beyond a demo integration with
OpenSceneGraph, `osgSlug`. No WASM build exists. No browser
JavaScript exists. No three.js binding exists.

Adopting it for `usd-viewer`'s WebGL scene means building two real
things ourselves, not configuring an existing one:

1. A WASM build of SlugHorn's C++20 core, a new build target this
   repository does not have today, alongside the existing
   `emHdBindings.wasm` USD build.
2. A three.js-side renderer that implements Slug's own draw
   technique against SlugHorn's curve output. Slug is a specific
   GPU rendering algorithm, described in its own published paper,
   not a drop-in text mesh a general 3D engine already knows how to
   draw.

Both are real, substantial builds. Neither is a small addition to
tonight's session.

## The smaller path: an HTML overlay, not a new WASM build

Godot's `Label3D` places ordinary flat text in 3D space, always
facing the camera. The same effect exists in WebGL without any GPU
vector-text library: project the card's 3D position through the
active camera's own matrix each frame
(`Vector3.project(camera)` in three.js, exposed already through
`usd-viewer`'s own `THREE.Camera` instance), and position an
ordinary HTML element at the resulting 2D screen coordinate. Three.js's
own documentation calls this the CSS2DRenderer pattern, real,
widely used, and needs no new WASM build, no unfinished dependency,
and no change to the USD stage itself.

The visible difference: SlugHorn's text would render inside the
WebGL scene itself, correctly occluded by 3D geometry in front of
it. The HTML overlay always draws on top, never occluded. For a flat
billboard card with nothing in front of it, RFD 0073's actual scene
today, that difference does not show.

## Open, for the next session

Which caption text to show (the dataset's own caption field, a
shortened version, on hover only), whether every card gets a label
or only the one on screen, and whether the smaller HTML-overlay path
covers the real need well enough that SlugHorn stays a future
option rather than a near-term one. Nothing here shipped or
deployed this session.
