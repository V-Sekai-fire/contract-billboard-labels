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

## Update: SlugHorn/WASM prototyped and working

The Godot-portability requirement settled the choice DETAILS.md's first
half described as open: SlugHorn over the HTML overlay, because the DOM
overlay cannot port to Godot at all, and SlugHorn's core has no
browser-only dependency baked in.

`prototype/slughorn-wasm-poc/` proves the pipeline, not just the plan:

- `third_party/slughorn` vendored via `git subtree` (squashed onto its own
  commit, `main` at the time of vendoring).
- The subtree brought five gitlinks with it, at `third_party/slughorn/ext/`.
  They are gone: CLAUDE.md blocklists submodules, none of the five was ever
  initialised, and the core `slughorn` target does not want them. Each sits
  behind an option that defaults off -- `SLUGHORN_SERIAL` needs `ext/json`,
  `SLUGHORN_NANOSVG` needs `ext/nanosvg`, `SLUGHORN_MSDF` needs `ext/msdfgen`.
  Turning one on means supplying that directory, and SlugHorn's own CMake will
  still tell you to run `git submodule update`, which this tree has nothing for.
  A `<project>` in the goal manifest is how it would come back.
- SlugHorn's core `slughorn` CMake target builds clean under Emscripten
  6.0.6 (`prototype/slughorn-wasm-harness/`), confirmed by inspecting the
  compiled object: a genuine `WebAssembly (wasm) binary version 0x1`.
- `binding.cpp` hand-authors one shape (a five-point star, since font
  loading is not wired up yet) through SlugHorn's own `Atlas::addShape()` /
  `build()`, and exports the resulting curve/band texture bytes as plain
  `extern "C"` functions reading directly out of wasm linear memory.
- `index.html` uploads those bytes to WebGL2 as the RGBA32F curve texture
  and RGBA16UI band texture SlugHorn's format expects, and renders the
  shape with the actual Slug fragment shader (Lengyel 2017) ported
  near-verbatim from SlugHorn's own `example/slughorn-example-glfw.cpp` —
  not a placeholder shader, the same coverage-solve math.
- Verified in-browser (Playwright, screenshotted): the rendered star is
  geometrically correct, and — the entire reason this path was chosen over
  the DOM overlay — gets properly depth-tested against a real 3D occluder
  panel in the scene. Part of the star is visibly clipped by the panel's
  silhouette. The HTML-overlay prototype cannot do this; it always draws
  on top, by construction.

See `prototype/slughorn-wasm-poc/README.md` for build/run instructions and
what remains uncovered.

## Open, for the next session

**Carried over from the original decision, now answerable with real
captions once font loading lands:** which caption text to show (the
dataset's own caption field, a shortened version, on hover only), and
whether every card gets a label or only the one on screen. The WASM
prototype's `binding.cpp` has no caption text yet — it renders one
hand-authored shape, not a font glyph.

**New, from getting SlugHorn actually building:**

- **Real glyphs.** `SLUGHORN_FREETYPE` requires `find_package(Freetype
REQUIRED)`, which does not resolve against Emscripten's
  `-sUSE_FREETYPE=1` port automatically (the port fetches/links at
  Emscripten's own build time, not through a CMake-discoverable install).
  Wiring FreeType into the Emscripten CMake build is the next real step
  before any caption text — not just a hand-authored shape — can render.
- **Multi-shape batching, built.** `prototype/slughorn-wasm-batch-poc/`
  extends `binding.cpp` to build one `Atlas` holding six distinct
  hand-authored shapes (5 to 10 points each, so each is visibly
  different, not six copies of one shape), and moves
  `a_bandXform`/`a_shapeData`/the per-shape world position and bbox
  from uniforms to `flat`-shaded per-vertex attributes, the same
  layout SlugHorn's own GLFW example
  (`a_emCoord`/`a_bandXform`/`a_shapeData`) uses. One interleaved
  vertex buffer, one `drawElements` call, all six shapes. The WASM
  build compiled clean under the same Emscripten 6.0.6 toolchain
  (`docker.io/emscripten/emsdk:latest`), confirmed a genuine
  `WebAssembly (wasm) binary version 0x1`. Verified live with
  Playwright: six visibly distinct shapes (5 to 10 points each, six
  different colors) render correctly, at their own positions along a
  row, the on-page status line reads "running: 6 shapes, one draw
  call (real WebGL2 Slug shader)," and the occluder panel correctly
  depth-tests against them individually, the same real-occlusion
  property the single-shape POC proved, now holding across a batch.
- **The real integration gap, found late: this is still a standalone
  canvas, not `usd-viewer`.** Every artifact under `prototype/` so
  far, both the single-shape POC and the batch POC, draws into its
  own bare `<canvas>` with hand-rolled camera math, a separate WebGL2
  context from `usd-viewer`'s own `THREE.WebGLRenderer`. The actual
  goal, stated plainly and only after this prototyping, is labels
  living inside `usd-viewer`'s own scene, for a real gallery in
  `usd-viewer`'s own style, comparable to Sketchfab or Fab.com, many
  cards, each with its own label, not a single demo shape on its own
  page. Grepping this repository confirms zero references to
  `usd-viewer` anywhere outside this note; the SlugHorn work and the
  usd-viewer integration have not been connected yet.

  The path there is not a rewrite: SlugHorn's output is a curve
  texture, a band texture, and per-shape metrics, all plain data,
  usable from a `THREE.ShaderMaterial` + `THREE.Mesh` just as easily
  as from raw `gl.*` calls, with the fragment shader above ported
  in almost unchanged (GLSL ES 300 is GLSL ES 300 either way). A
  `THREE.Mesh` is an `Object3D`, so it can enter `usd-viewer`'s own
  scene through the same narrow, single-purpose extension point an
  earlier, since-dropped attempt at this RFD already established:
  one public method on the element, adding a child into the group
  `usd-viewer` already builds for the loaded model, nothing broader.
  Not built this session; recorded here so the next session starts
  from the real gap, not from the standalone canvas as if it were
  the finish line.
- **The Godot GDExtension binding.** Deferred this session by explicit
  request ("don't touch Godot"). SlugHorn's core is the same C++20 code
  either way; the Godot path swaps the Emscripten binding for a
  GDExtension one around the identical `Atlas` API this prototype already
  exercises.
