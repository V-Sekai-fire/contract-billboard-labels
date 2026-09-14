#!/usr/bin/env bash
# Builds binding.cpp + SlugHorn's core into slughorn.js / slughorn.wasm.
#
# Requires emsdk activated in the current shell (source emsdk_env.sh), or
# pass EMSDK_ROOT pointing at an installed emsdk checkout.
set -euo pipefail

HERE="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
SLUGHORN_ROOT="$HERE/../../third_party/slughorn"

EMCC="${EMCC:-em++}"

"$EMCC" \
	-std=c++20 -O2 \
	-I "$SLUGHORN_ROOT" \
	"$HERE/binding.cpp" \
	"$SLUGHORN_ROOT/slughorn/slughorn.cpp" \
	-sMODULARIZE=1 \
	-sEXPORT_ES6=1 \
	-sEXPORT_NAME=SlugHornModule \
	-sENVIRONMENT=web,node \
	-sALLOW_MEMORY_GROWTH=1 \
	-sEXPORTED_RUNTIME_METHODS=HEAPU8 \
	-sEXPORTED_FUNCTIONS=_slughorn_buildBatchAtlas,_slughorn_shapeCount,_slughorn_curveTexPtr,_slughorn_curveTexLen,_slughorn_curveTexWidth,_slughorn_curveTexHeight,_slughorn_bandTexPtr,_slughorn_bandTexLen,_slughorn_bandTexWidth,_slughorn_bandTexHeight,_slughorn_atlasTexWidthLog2,_slughorn_shapeField \
	-o "$HERE/slughorn.js"

echo "Built $HERE/slughorn.js + slughorn.wasm"
