# contract-billboard-labels

A caption label that floats over each billboard card in the 3D gallery and always faces the camera.

## What it is for

RFD 1074 is developed here. The gallery's cards show an image with no caption, and the label draws the caption with the Slug vector-text technique from a WebAssembly build of slughorn, so the same C++ source can also be bound into the engine. `DETAILS.md` carries the reasoning and what the prototypes found.

## Build and run

Each WebAssembly prototype carries its own build script and a page that loads its output.

## Licence

MIT. See [LICENSE](LICENSE). The vendored slughorn is MIT.
