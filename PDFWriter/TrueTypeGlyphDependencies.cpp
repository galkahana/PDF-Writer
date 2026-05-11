/*
   Source File : TrueTypeGlyphDependencies.cpp


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
#include "TrueTypeGlyphDependencies.h"

#include "Trace.h"

// Recursion depth cap. Adobe's TrueType reference recommends max
// composite depth of 16; we use that literal rather than the font-
// supplied maxp.MaxComponentDepth because the latter is attacker-
// controlled.
static const unsigned int scMaxCompositeDepth = 16;

bool TrueTypeGlyphDependencies::CollectComponentGlyphs(unsigned int inGlyphID,
	GlyphEntry* const* inGlyfTable,
	unsigned int inNumGlyphs,
	UIntSet& ioComponents,
	unsigned int inDepth)
{
	if(inGlyphID >= inNumGlyphs)
	{
		TRACE_LOG2("TrueTypeGlyphDependencies::CollectComponentGlyphs, error, requested glyph index %u is out of range for a font with %u glyphs.",
			inGlyphID, inNumGlyphs);
		return false;
	}

	if(inDepth > scMaxCompositeDepth)
	{
		// Cycles are blocked by the visited-set guard below, but a
		// malicious font can still build a deeply nested acyclic chain.
		// numGlyphs caps the visited set at 65535 entries, which is
		// well past typical stack limits, so cap depth here too.
		TRACE_LOG2("TrueTypeGlyphDependencies::CollectComponentGlyphs, composite depth %u exceeds cap %u, refusing to recurse further.",
			inDepth, scMaxCompositeDepth);
		return false;
	}

	GlyphEntry* glyfTableEntry = inGlyfTable[inGlyphID];
	if(glyfTableEntry == NULL || glyfTableEntry->mComponentGlyphs.size() == 0)
		return false;

	UIntList::iterator itComponentGlyphs;
	for(itComponentGlyphs = glyfTableEntry->mComponentGlyphs.begin();
		itComponentGlyphs != glyfTableEntry->mComponentGlyphs.end();
		++itComponentGlyphs)
	{
		// Recurse only when the component is new to the set. A glyph
		// referencing itself or two glyphs referencing each other would
		// otherwise drive the call stack until it overflows. The set
		// doubles as the visited marker for cycle detection.
		if(ioComponents.insert(*itComponentGlyphs).second)
			CollectComponentGlyphs(*itComponentGlyphs, inGlyfTable, inNumGlyphs, ioComponents, inDepth + 1);
	}
	return true;
}
