# RFD 0074 prototype: SlugHorn → WASM → real Slug shader

Proves the path DETAILS.md scoped as "real work, not a small addition":
SlugHorn's C++20 core compiles to WASM, and its curve/band texture output
drives an actual GPU implementation of the Slug technique (Lengyel 2017) in
WebGL2 — not a placeholder, the same fragment shader math as SlugHorn's own
GLFW example (`third_party/slughorn/example/slughorn-example-glfw.cpp`),
ported to GLSL ES 300.

## What this does and does not cover yet

**Covered:**
- `third_party/slughorn` vendored via `git subtree` (squashed).
- SlugHorn's core `slughorn` CMake target builds clean under Emscripten
  6.0.6 (see `../slughorn-wasm-harness/`).
- `binding.cpp`: a thin WASM binding, hand-authoring one shape (a five-point
  star, straight-line "curves") through `Atlas::addShape()` /
  `Atlas::build()`, exporting the resulting curve/band texture bytes and
  shape metrics as plain `extern "C"` functions.
- `index.html`: loads the wasm module, uploads the curve texture (RGBA32F)
  and band texture (RGBA16UI) to WebGL2, and renders the shape with the
  ported Slug fragment shader on a camera-facing billboard quad in a real
  3D scene with a depth-tested occluder panel. Confirmed by screenshot: the
  star gets correctly clipped by the panel's silhouette, something the
  HTML-overlay path (`../html-overlay-poc.html`) cannot do.

**Not covered (next milestones):**
- Real font glyphs. This binding hand-authors curves; it does not call
  FreeType. `SLUGHORN_FREETYPE` needs `find_package(Freetype REQUIRED)`,
  which does not resolve against Emscripten's `-sUSE_FREETYPE=1` port
  automatically — that wiring is the next step before real caption text
  can render.
- Layer/composite batching for multiple shapes (six billboard cards' worth
  of captions) in one draw call. This POC draws one shape with uniforms;
  a real integration would follow the example's attribute-per-instance
  approach for many shapes sharing one atlas.
- A Godot GDExtension binding. SlugHorn's C++20 core is the same code this
  WASM build compiles — the Godot path reuses it through a GDExtension
  binding instead of an Emscripten one, deferred per the user's own
  "don't touch Godot yet" sequencing for this session.

## Build

Requires an activated Emscripten SDK (`source emsdk_env.sh`), or set `EMCC`
to a full path to `em++`:

```sh
bash build.sh
```

Produces `slughorn.js` + `slughorn.wasm` next to `binding.cpp`.

## Run

`fetch()` of the `.wasm` file needs real HTTP, not `file://`:

```sh
python -m http.server 8934
```

Then open `http://localhost:8934/index.html`. Drag to orbit, scroll to
zoom, toggle the occluder panel and auto-rotate from the on-page panel.
