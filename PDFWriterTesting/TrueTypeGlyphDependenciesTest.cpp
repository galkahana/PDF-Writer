/*
   Source File : TrueTypeGlyphDependenciesTest.cpp


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


   Regression tests for TrueTypeGlyphDependencies::CollectComponentGlyphs, the
   composite-glyph dependency walker extracted from
   TrueTypeEmbeddedFontWriter so its cycle handling can be tested without
   a full font-embedding pipeline.

   V-018: pre-fix the walker recursed on `mComponentGlyphs` without
   tracking visited glyphs, so a malformed TrueType font with a
   self-referencing composite glyph (or a 2-cycle) drove the call stack
   until it overflowed. Same bug shape as the /Parent cycle bugs fixed
   in PRs #346/#347/#348/#349 but on the font-subsetting side. Post-fix
   the walker recurses only on first-seen components — the visited set
   doubles as the dedup mechanism.
*/
#include "OpenTypeFileInput.h"
#include "TrueTypeGlyphDependencies.h"

#include <assert.h>
#include <iostream>

using namespace std;

// Build a stack-only glyf table from caller-supplied component lists.
// Stack-only construction (no new/delete) keeps the tests obvious — every
// fixture lives in the test function frame. Capacity is sized for the
// deepest fixture (depth-cap test chain).
static const unsigned int scStaticGlyfCapacity = 32;

struct StaticGlyfTable {
	GlyphEntry mEntries[scStaticGlyfCapacity];
	GlyphEntry* mTable[scStaticGlyfCapacity];
	unsigned int mNumGlyphs;
};

static void initGlyfTable(StaticGlyfTable& outTable, unsigned int inNumGlyphs) {
	// Refuse silent OOB writes if a future test asks for more glyphs than
	// the fixture's fixed-size arrays can hold. Bump scStaticGlyfCapacity
	// rather than relying on this triggering at runtime.
	assert(inNumGlyphs <= scStaticGlyfCapacity);
	outTable.mNumGlyphs = inNumGlyphs;
	for(unsigned int i = 0; i < inNumGlyphs; ++i)
		outTable.mTable[i] = &outTable.mEntries[i];
}

// Test 1: glyph 1 references itself. Pre-fix the recursion descends into
// glyph 1 forever; post-fix the second visit to glyph 1 is short-circuited
// by the visited-set check.
static bool CollectComponentGlyphs_SelfReferencingGlyph_TerminatesAndPopulatesSet() {
	// Arrange: 2 glyphs, glyph 0 simple, glyph 1 -> {1} (self).
	StaticGlyfTable t;
	initGlyfTable(t, 2);
	t.mEntries[1].mComponentGlyphs.push_back(1);
	UIntSet result;

	// Act
	bool isComposite = TrueTypeGlyphDependencies::CollectComponentGlyphs(1, t.mTable, t.mNumGlyphs, result);

	// Assert
	if(!isComposite) {
		cout << "TrueTypeGlyphDependenciesTest [CollectComponentGlyphs::SelfReferencingGlyph_TerminatesAndPopulatesSet]: expected composite, got simple" << endl;
		return false;
	}
	if(result.size() != 1 || result.find(1) == result.end()) {
		cout << "TrueTypeGlyphDependenciesTest [CollectComponentGlyphs::SelfReferencingGlyph_TerminatesAndPopulatesSet]: expected {1}, got set of size "
		     << result.size() << endl;
		return false;
	}
	return true;
}

// Test 2: glyph 1 -> {2}, glyph 2 -> {1}. Pre-fix mutual recursion never
// terminates; post-fix the back-edge to 1 is short-circuited.
static bool CollectComponentGlyphs_TwoCycleGlyphs_TerminatesAndPopulatesSet() {
	// Arrange: 3 glyphs, glyph 0 simple, glyph 1 -> {2}, glyph 2 -> {1}.
	StaticGlyfTable t;
	initGlyfTable(t, 3);
	t.mEntries[1].mComponentGlyphs.push_back(2);
	t.mEntries[2].mComponentGlyphs.push_back(1);
	UIntSet result;

	// Act
	bool isComposite = TrueTypeGlyphDependencies::CollectComponentGlyphs(1, t.mTable, t.mNumGlyphs, result);

	// Assert
	if(!isComposite) {
		cout << "TrueTypeGlyphDependenciesTest [CollectComponentGlyphs::TwoCycleGlyphs_TerminatesAndPopulatesSet]: expected composite, got simple" << endl;
		return false;
	}
	if(result.size() != 2 || result.find(1) == result.end() || result.find(2) == result.end()) {
		cout << "TrueTypeGlyphDependenciesTest [CollectComponentGlyphs::TwoCycleGlyphs_TerminatesAndPopulatesSet]: expected {1, 2}, got set of size "
		     << result.size() << endl;
		return false;
	}
	return true;
}

// Test 3 (happy path): glyph 1 -> {2}, glyph 2 -> {3}, glyph 3 simple.
// Proves the visited-set guard didn't accidentally drop the normal
// transitive-component case (where every component IS new on first visit).
static bool CollectComponentGlyphs_LinearDependencyChain_GathersAllTransitive() {
	// Arrange: 4 glyphs, 0 simple, 1 -> {2}, 2 -> {3}, 3 simple.
	StaticGlyfTable t;
	initGlyfTable(t, 4);
	t.mEntries[1].mComponentGlyphs.push_back(2);
	t.mEntries[2].mComponentGlyphs.push_back(3);
	UIntSet result;

	// Act
	bool isComposite = TrueTypeGlyphDependencies::CollectComponentGlyphs(1, t.mTable, t.mNumGlyphs, result);

	// Assert
	if(!isComposite) {
		cout << "TrueTypeGlyphDependenciesTest [CollectComponentGlyphs::LinearDependencyChain_GathersAllTransitive]: expected composite, got simple" << endl;
		return false;
	}
	if(result.size() != 2 || result.find(2) == result.end() || result.find(3) == result.end()) {
		cout << "TrueTypeGlyphDependenciesTest [CollectComponentGlyphs::LinearDependencyChain_GathersAllTransitive]: expected {2, 3}, got set of size "
		     << result.size() << endl;
		return false;
	}
	return true;
}

// Test 4 (happy path): glyph 1 -> {2, 3}, where both 2 and 3 reference
// glyph 4. The dedup property of the visited set means glyph 4 should
// appear once even though two siblings independently depend on it.
static bool CollectComponentGlyphs_SharedComponentAcrossSiblings_DedupesViaVisitedSet() {
	// Arrange: 5 glyphs, 0 simple, 1 -> {2, 3}, 2 -> {4}, 3 -> {4}, 4 simple.
	StaticGlyfTable t;
	initGlyfTable(t, 5);
	t.mEntries[1].mComponentGlyphs.push_back(2);
	t.mEntries[1].mComponentGlyphs.push_back(3);
	t.mEntries[2].mComponentGlyphs.push_back(4);
	t.mEntries[3].mComponentGlyphs.push_back(4);
	UIntSet result;

	// Act
	bool isComposite = TrueTypeGlyphDependencies::CollectComponentGlyphs(1, t.mTable, t.mNumGlyphs, result);

	// Assert
	if(!isComposite) {
		cout << "TrueTypeGlyphDependenciesTest [CollectComponentGlyphs::SharedComponentAcrossSiblings_DedupesViaVisitedSet]: expected composite, got simple" << endl;
		return false;
	}
	if(result.size() != 3
	   || result.find(2) == result.end()
	   || result.find(3) == result.end()
	   || result.find(4) == result.end()) {
		cout << "TrueTypeGlyphDependenciesTest [CollectComponentGlyphs::SharedComponentAcrossSiblings_DedupesViaVisitedSet]: expected {2, 3, 4}, got set of size "
		     << result.size() << endl;
		return false;
	}
	return true;
}

// Test 5: simple glyph (no components) returns false and leaves the
// caller's set unchanged.
static bool CollectComponentGlyphs_SimpleGlyph_ReturnsFalseAndLeavesSetEmpty() {
	// Arrange: 2 glyphs, both simple.
	StaticGlyfTable t;
	initGlyfTable(t, 2);
	UIntSet result;

	// Act
	bool isComposite = TrueTypeGlyphDependencies::CollectComponentGlyphs(0, t.mTable, t.mNumGlyphs, result);

	// Assert
	if(isComposite) {
		cout << "TrueTypeGlyphDependenciesTest [CollectComponentGlyphs::SimpleGlyph_ReturnsFalseAndLeavesSetEmpty]: expected simple, got composite" << endl;
		return false;
	}
	if(!result.empty()) {
		cout << "TrueTypeGlyphDependenciesTest [CollectComponentGlyphs::SimpleGlyph_ReturnsFalseAndLeavesSetEmpty]: expected empty set, got size "
		     << result.size() << endl;
		return false;
	}
	return true;
}

// Test 6: a long acyclic chain of composite glyphs must hit some depth
// cap and stop, instead of recursing all the way down (which would
// overflow the stack on a malicious font with thousands of nested
// composites). Chain is sized comfortably longer than any reasonable
// cap so the test doesn't break if the exact cap value is later tuned —
// it just asserts that the tail of the chain wasn't reached.
static bool CollectComponentGlyphs_DeepAcyclicChain_StopsBeforeChainEnd() {
	const unsigned int chainLength = 30;             // > any plausible cap
	const unsigned int numGlyphs = chainLength + 1;  // chain + glyph 0 (simple)

	// Arrange: chain glyph 1 -> 2 -> ... -> chainLength.
	StaticGlyfTable t;
	initGlyfTable(t, numGlyphs);
	for(unsigned int i = 1; i < chainLength; ++i)
		t.mEntries[i].mComponentGlyphs.push_back(i + 1);
	UIntSet result;

	// Act
	bool isComposite = TrueTypeGlyphDependencies::CollectComponentGlyphs(1, t.mTable, t.mNumGlyphs, result);

	// Assert: the starting glyph is composite (so the walk did enter the
	// chain), but the depth cap stopped descent before the tail was
	// reached. Whatever the exact cap value is, glyph `chainLength`
	// sits past it.
	if(!isComposite) {
		cout << "TrueTypeGlyphDependenciesTest [CollectComponentGlyphs::DeepAcyclicChain_StopsBeforeChainEnd]: expected composite, got simple" << endl;
		return false;
	}
	if(result.find(chainLength) != result.end()) {
		cout << "TrueTypeGlyphDependenciesTest [CollectComponentGlyphs::DeepAcyclicChain_StopsBeforeChainEnd]: tail glyph "
		     << chainLength << " was reached despite depth cap" << endl;
		return false;
	}
	if(result.size() >= chainLength - 1) {
		cout << "TrueTypeGlyphDependenciesTest [CollectComponentGlyphs::DeepAcyclicChain_StopsBeforeChainEnd]: full chain (" << (chainLength - 1)
		     << " components) was traversed; depth cap didn't fire" << endl;
		return false;
	}
	return true;
}

// Test 7: glyph ID at or beyond the table size returns false and leaves
// the caller's set unchanged (this is the existing pre-V-018 guard, kept
// as part of the contract).
static bool CollectComponentGlyphs_GlyphIDBeyondTableSize_ReturnsFalse() {
	// Arrange: 2 glyphs, both simple.
	StaticGlyfTable t;
	initGlyfTable(t, 2);
	UIntSet result;

	// Act: ask for glyph 5 (table only holds 0..1).
	bool isComposite = TrueTypeGlyphDependencies::CollectComponentGlyphs(5, t.mTable, t.mNumGlyphs, result);

	// Assert
	if(isComposite) {
		cout << "TrueTypeGlyphDependenciesTest [CollectComponentGlyphs::GlyphIDBeyondTableSize_ReturnsFalse]: out-of-range glyph reported composite" << endl;
		return false;
	}
	if(!result.empty()) {
		cout << "TrueTypeGlyphDependenciesTest [CollectComponentGlyphs::GlyphIDBeyondTableSize_ReturnsFalse]: out-of-range glyph populated set" << endl;
		return false;
	}
	return true;
}

int TrueTypeGlyphDependenciesTest(int argc, char* argv[]) {
	(void)argc;
	(void)argv;
	if(!CollectComponentGlyphs_SelfReferencingGlyph_TerminatesAndPopulatesSet()) return 1;
	if(!CollectComponentGlyphs_TwoCycleGlyphs_TerminatesAndPopulatesSet()) return 1;
	if(!CollectComponentGlyphs_LinearDependencyChain_GathersAllTransitive()) return 1;
	if(!CollectComponentGlyphs_SharedComponentAcrossSiblings_DedupesViaVisitedSet()) return 1;
	if(!CollectComponentGlyphs_SimpleGlyph_ReturnsFalseAndLeavesSetEmpty()) return 1;
	if(!CollectComponentGlyphs_DeepAcyclicChain_StopsBeforeChainEnd()) return 1;
	if(!CollectComponentGlyphs_GlyphIDBeyondTableSize_ReturnsFalse()) return 1;
	return 0;
}
