# RFD 0074 prototype: batched SlugHorn shapes, one draw call

Proves the next milestone `../slughorn-wasm-poc/README.md` named as not yet
covered: "Layer/composite batching for multiple shapes ... a real
integration would follow the example's attribute-per-instance approach for
many shapes sharing one atlas."

## What this does

- `binding.cpp`: builds one `slughorn::Atlas` holding six hand-authored star
  shapes, point count 5 through 10, so each is visibly distinct in a
  screenshot, not six copies of one shape proving nothing about per-shape
  data actually varying across a batch. Exports a `slughorn_shapeField(shapeIdx, idx)`
  accessor, extending the single-shape POC's own accessor with a shape
  index.
- `index.html`: builds one interleaved vertex buffer covering all six
  shapes' quads (21 floats/vertex: `a_uv`, `a_center`, `a_worldSize`,
  `a_bbox`, `a_bandXform`, `a_shapeData`, `a_color`), the attribute-per-instance
  layout `third_party/slughorn/example/slughorn-example-glfw.cpp`'s own
  `k_VertSrc` uses (`a_emCoord`/`a_bandXform`/`a_shapeData` as vertex
  attributes, not uniforms), `bandXform`/`shapeData` marked `flat` in both
  shader stages so each shape's own atlas location is not interpolated
  against its neighbors. One `drawElements` call renders all six.
- Same camera, ground marker, and occluder-panel setup as
  `../slughorn-wasm-poc/index.html`, so the real-occlusion property that POC
  proved (SlugHorn's output gets correctly depth-tested against 3D geometry,
  unlike a DOM overlay) is exercised here too, per shape.

Verified with Playwright: six visibly distinct shapes, six colors, one
`drawElements` call, correct per-shape depth-testing against the occluder
panel. The on-page status line reports the live shape count and confirms a
real WebGL2 Slug shader, not a placeholder.

## What this does not cover yet

Same open items as the single-shape POC: real font glyphs (FreeType is not
wired into the Emscripten CMake build), and the Godot GDExtension binding
(explicitly deferred this session). See `DETAILS.md`'s own "Open, for the
next session" for both, and for the real remaining gap found after this
batch POC: none of `prototype/`'s work is wired into `usd-viewer`'s own
scene yet — every artifact here draws into its own standalone canvas.

## Build

Requires an activated Emscripten SDK (`source emsdk_env.sh`), or set `EMCC`
to a full path to `em++`. Verified in this session with
`docker.io/emscripten/emsdk:latest`:

```sh
docker run --rm -v "$(pwd)/../..:/work" -w /work/prototype/slughorn-wasm-batch-poc \
  emscripten/emsdk:latest bash build.sh
```

Produces `slughorn.js` + `slughorn.wasm` next to `binding.cpp`.

## Run

`fetch()` of the `.wasm` file needs real HTTP, not `file://`, and
`SharedArrayBuffer`/COEP is not required here (unlike `usd-viewer`'s own
WASM build) since this POC uses no threads:

```sh
python -m http.server 8123
```

Then open `http://localhost:8123/index.html`. Drag to orbit, scroll to
zoom, toggle the occluder panel and auto-rotate from the on-page panel.
