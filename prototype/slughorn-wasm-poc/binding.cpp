// RFD 0074 prototype: thin WASM binding over SlugHorn's core Atlas.
//
// No FreeType yet -- that is the next milestone (real glyph outlines from a
// font file). This binding hand-authors one shape, a five-point star, as a
// closed contour of straight-line "curves" (quadratic Beziers with the
// control point at the segment midpoint -- a straight line is a degenerate
// quadratic curve, and Atlas takes Curve data at whatever degree the caller
// hands it). That is enough to exercise the real pipeline SlugHorn documents:
// addShape() -> build() -> curve/band texture bytes, ready for a GPU upload.
//
// Exported as plain extern "C" functions (EMSCRIPTEN_KEEPALIVE), not Embind,
// so the JS side can read the texture bytes directly out of wasm linear
// memory (Module.HEAPU8.buffer) with zero copy through a typed-array view.

#include "slughorn/slughorn.hpp"

#include <emscripten/emscripten.h>

#include <cmath>
#include <memory>

using namespace slughorn;

namespace {
	constexpr double kPi = 3.14159265358979323846;

	std::unique_ptr<Atlas> g_atlas;
	const Key g_key("star");
}

extern "C" {

// Builds the atlas. Call once before any of the accessors below.
EMSCRIPTEN_KEEPALIVE
void slughorn_buildStarAtlas() {
	g_atlas = std::make_unique<Atlas>(512);

	Atlas::ShapeInfo info;
	info.autoMetrics = true;

	// Five-point star, em-space [0,1]x[0,1], point-up, one closed contour,
	// no holes -- deliberately the simplest shape that still needs the
	// band/curve machinery (more than a handful of curves, non-trivial
	// bounding structure) without risking a winding-direction mistake.
	constexpr int kPoints = 10;
	constexpr double cx = 0.5, cy = 0.5;
	constexpr double rOuter = 0.48, rInner = 0.19;

	double px[kPoints], py[kPoints];
	for(int i = 0; i < kPoints; i++) {
		double ang = -kPi / 2.0 + i * kPi / 5.0;
		double r = (i % 2 == 0) ? rOuter : rInner;
		px[i] = cx + r * std::cos(ang);
		py[i] = cy + r * std::sin(ang);
	}

	for(int i = 0; i < kPoints; i++) {
		int j = (i + 1) % kPoints;
		Atlas::Curve c;
		c.x1 = static_cast<slug_t>(px[i]);
		c.y1 = static_cast<slug_t>(py[i]);
		c.x2 = static_cast<slug_t>((px[i] + px[j]) / 2.0);
		c.y2 = static_cast<slug_t>((py[i] + py[j]) / 2.0);
		c.x3 = static_cast<slug_t>(px[j]);
		c.y3 = static_cast<slug_t>(py[j]);
		info.curves.push_back(c);
	}

	g_atlas->addShape(g_key, info);
	g_atlas->build();
}

EMSCRIPTEN_KEEPALIVE const uint8_t* slughorn_curveTexPtr() { return g_atlas->getCurveTextureData().bytes.data(); }
EMSCRIPTEN_KEEPALIVE int slughorn_curveTexLen() { return static_cast<int>(g_atlas->getCurveTextureData().bytes.size()); }
EMSCRIPTEN_KEEPALIVE int slughorn_curveTexWidth() { return static_cast<int>(g_atlas->getCurveTextureData().width); }
EMSCRIPTEN_KEEPALIVE int slughorn_curveTexHeight() { return static_cast<int>(g_atlas->getCurveTextureData().height); }

EMSCRIPTEN_KEEPALIVE const uint8_t* slughorn_bandTexPtr() { return g_atlas->getBandTextureData().bytes.data(); }
EMSCRIPTEN_KEEPALIVE int slughorn_bandTexLen() { return static_cast<int>(g_atlas->getBandTextureData().bytes.size()); }
EMSCRIPTEN_KEEPALIVE int slughorn_bandTexWidth() { return static_cast<int>(g_atlas->getBandTextureData().width); }
EMSCRIPTEN_KEEPALIVE int slughorn_bandTexHeight() { return static_cast<int>(g_atlas->getBandTextureData().height); }

EMSCRIPTEN_KEEPALIVE int slughorn_atlasTexWidthLog2() {
	// Atlas(512) above; must match SLUG_INDIRECTION_SIZE-adjacent bit math in the shader.
	return static_cast<int>(std::log2(static_cast<double>(g_atlas->getTextureWidth())));
}

// Shape fields the vertex/fragment shader needs, read out one float at a
// time to keep this binding to plain scalar-returning functions.
// idx: 0=bandTexX 1=bandTexY 2=bandMaxX 3=bandMaxY 4=bandScaleX 5=bandScaleY
//      6=bandOffsetX 7=bandOffsetY 8=bearingX 9=bearingY 10=width 11=height
EMSCRIPTEN_KEEPALIVE
float slughorn_shapeField(int idx) {
	auto shape = g_atlas->getShape(g_key);
	if(!shape) return 0.0f;

	switch(idx) {
		case 0: return static_cast<float>(shape->bandTexX);
		case 1: return static_cast<float>(shape->bandTexY);
		case 2: return static_cast<float>(shape->bandMaxX);
		case 3: return static_cast<float>(shape->bandMaxY);
		case 4: return shape->bandScaleX;
		case 5: return shape->bandScaleY;
		case 6: return shape->bandOffsetX;
		case 7: return shape->bandOffsetY;
		case 8: return shape->bearingX;
		case 9: return shape->bearingY;
		case 10: return shape->width;
		case 11: return shape->height;
		default: return 0.0f;
	}
}

} // extern "C"
