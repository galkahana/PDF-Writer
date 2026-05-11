/*
   Source File : TrueTypeGlyphDependencies.h


   Copyright 2026 Gal Kahana PDFWriter

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.


*/
#pragma once

#include "OpenTypeFileInput.h"

#include <set>

typedef std::set<unsigned int> UIntSet;

// Composite-glyph dependency utilities over a TrueType `glyf` table.
class TrueTypeGlyphDependencies
{
public:
	// Insert every transitive component referenced by `inGlyphID` into
	// `ioComponents`. The initial glyph is NOT added; only its components
	// (and theirs, transitively) are. Returns true iff `inGlyphID` is
	// itself composite (has any direct components).
	//
	// Two recursion safeguards:
	//   * Visited-set guard: a component is recursed into only when it
	//     was newly inserted, so a self-reference or a cycle (glyph
	//     A -> B -> A) terminates instead of blowing the stack.
	//   * Depth cap: cuts off acyclic but very deep chains before they
	//     overflow the call stack. NumGlyphs is uint16 (max 65535) and
	//     a chain that long would still overflow typical stacks despite
	//     the visited-set bound. The exact cap is a private detail of
	//     the implementation.
	//
	// `inGlyfTable[i]` may be NULL for missing or zero-length glyphs.
	// `inGlyphID >= inNumGlyphs` returns false without touching the set.
	// `inDepth` is for internal recursion accounting — callers pass 0
	// (the default).
	static bool CollectComponentGlyphs(unsigned int inGlyphID,
		GlyphEntry* const* inGlyfTable,
		unsigned int inNumGlyphs,
		UIntSet& ioComponents,
		unsigned int inDepth = 0);
};
