/*
   Source File : FreeTypeWrapperTest.cpp


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


   Regression tests for FreeTypeWrapper's FreeType-boundary memory hygiene:

     * V-073: CleanStreamsForFace ran mOpenStreams.erase(it) OUTSIDE the
       if(it != end()) block, so DoneFace on a face that was never
       registered (or any second DoneFace) called std::map::erase(end())
       on an empty/foreign iterator - undefined behavior on the container's
       internal tree. The unregistered-face case is the clean public-API
       discriminator: it does not double-free the FT_Face. ASAN flags the
       pre-fix tree walk; the post-fix path is a no-op.

     * V-074: CreateFTStreamForPath leaked the freshly-new'd InputFile* when
       InputFile::OpenFile failed (NewFace on a non-existent path). Pure
       leak with no functional signal; only a leak sanitizer can observe
       it directly (LSan is unavailable on Darwin - see the per-finding
       note in the PR). The committed assertion locks in the observable
       contract: NewFace returns NULL for a bad path and the wrapper stays
       usable for a subsequent valid load.

     * V-075: NewFace(file, secondaryFile, index) read a possibly-
       uninitialized FT_Open_Args on an early-fail path; today only safe by
       FillOpenFaceArgumentsForUTF8String always writing .stream = NULL on
       failure. Latent (per the latent-fix principle: no synthetic no-op
       case). The committed assertion locks in the observable contract: a
       bad secondary path returns NULL without crashing and leaves the
       wrapper usable.
*/
#include "FreeTypeWrapper.h"

#include "testing/TestIO.h"

#include <iostream>
#include <string>

using namespace std;
using namespace PDFHummus;

// V-073: a face created directly on the wrapper's FT_Library is never
// registered in mOpenStreams, so DoneFace's CleanStreamsForFace finds
// end(). Pre-fix it then called mOpenStreams.erase(end()) on an empty map
// (undefined behavior; ASAN aborts in the red-black tree). Post-fix the
// erase is inside the if(found) block, so an unregistered face is a clean
// no-op. FT_Done_Face on this face is well-defined (FT_New_Face created
// it on the same library), so this isolates the erase-with-end bug without
// a double-free confound.
static bool DoneFace_UnregisteredFace_DoesNotCorruptStreamMap(char* argv[]) {
	bool ok = false;
	FreeTypeWrapper ft;
	FT_Face foreignFace = NULL;
	do {
		// Arrange
		string arialPath = BuildRelativeInputPath(argv, "fonts/arial.ttf");
		FT_Error newStatus = FT_New_Face((FT_Library)ft, arialPath.c_str(), 0, &foreignFace);
		if(newStatus != 0 || foreignFace == NULL) {
			cout << "FreeTypeWrapperTest [DoneFace::UnregisteredFace_DoesNotCorruptStreamMap]: FT_New_Face failed for arial.ttf" << endl;
			break;
		}

		// Act
		// foreignFace was never registered via FreeTypeWrapper::NewFace
		FT_Error doneStatus = ft.DoneFace(foreignFace);
		foreignFace = NULL;

		// Assert
		if(doneStatus != 0) {
			cout << "FreeTypeWrapperTest [DoneFace::UnregisteredFace_DoesNotCorruptStreamMap]: DoneFace returned a nonzero FT_Error " << doneStatus << endl;
			break;
		}
		// Map must still be usable after the unregistered DoneFace: a fresh
		// valid load + DoneFace must succeed (a corrupted tree would crash
		// or misbehave here).
		FT_Face validFace = ft.NewFace(BuildRelativeInputPath(argv, "fonts/arial.ttf"), 0);
		if(validFace == NULL) {
			cout << "FreeTypeWrapperTest [DoneFace::UnregisteredFace_DoesNotCorruptStreamMap]: NewFace failed after the unregistered DoneFace" << endl;
			break;
		}
		FT_Error secondDone = ft.DoneFace(validFace);
		if(secondDone != 0) {
			cout << "FreeTypeWrapperTest [DoneFace::UnregisteredFace_DoesNotCorruptStreamMap]: second DoneFace returned a nonzero FT_Error " << secondDone << endl;
			break;
		}
		ok = true;
	} while(false);

	if(foreignFace != NULL)
		ft.DoneFace(foreignFace);
	return ok;
}

// V-073 happy path: a face loaded through FreeTypeWrapper::NewFace IS
// registered, so DoneFace must take the found branch, delete the streams
// and erase the entry. Locks in that moving the erase inside the if did
// not regress the normal lifecycle (a real glyph still resolves before
// teardown, and a second NewFace/DoneFace cycle still works).
static bool NewFaceThenDoneFace_RegisteredFace_RoundTrips(char* argv[]) {
	bool ok = false;
	FreeTypeWrapper ft;
	FT_Face face = NULL;
	do {
		// Arrange
		face = ft.NewFace(BuildRelativeInputPath(argv, "fonts/arial.ttf"), 0);
		if(face == NULL) {
			cout << "FreeTypeWrapperTest [NewFaceThenDoneFace::RegisteredFace_RoundTrips]: NewFace failed for arial.ttf" << endl;
			break;
		}

		// Act
		FT_UInt glyphIndex = FT_Get_Char_Index(face, 'A');
		FT_Error doneStatus = ft.DoneFace(face);
		face = NULL;

		// Assert
		if(glyphIndex == 0) {
			cout << "FreeTypeWrapperTest [NewFaceThenDoneFace::RegisteredFace_RoundTrips]: no glyph for 'A'" << endl;
			break;
		}
		if(doneStatus != 0) {
			cout << "FreeTypeWrapperTest [NewFaceThenDoneFace::RegisteredFace_RoundTrips]: DoneFace returned a nonzero FT_Error " << doneStatus << endl;
			break;
		}
		// A second full cycle proves the map entry was erased cleanly.
		FT_Face face2 = ft.NewFace(BuildRelativeInputPath(argv, "fonts/arial.ttf"), 0);
		if(face2 == NULL) {
			cout << "FreeTypeWrapperTest [NewFaceThenDoneFace::RegisteredFace_RoundTrips]: second NewFace failed" << endl;
			break;
		}
		if(ft.DoneFace(face2) != 0) {
			cout << "FreeTypeWrapperTest [NewFaceThenDoneFace::RegisteredFace_RoundTrips]: second DoneFace returned a nonzero FT_Error" << endl;
			break;
		}
		ok = true;
	} while(false);

	if(face != NULL)
		ft.DoneFace(face);
	return ok;
}

// V-074: NewFace on a non-existent path makes InputFile::OpenFile fail
// inside CreateFTStreamForPath. Pre-fix the new'd InputFile* was dropped
// on the floor (leak). The leak itself needs a leak sanitizer to observe
// (unavailable on Darwin); this asserts the observable contract - NewFace
// returns NULL and the wrapper is still usable for a subsequent valid
// load (no state corruption from the failed attempt).
static bool NewFace_NonExistentPath_ReturnsNullAndWrapperStaysUsable(char* argv[]) {
	bool ok = false;
	FreeTypeWrapper ft;
	FT_Face validFace = NULL;
	do {
		// Arrange / Act
		FT_Face missing = ft.NewFace("/no/such/font/file.ttf", 0);

		// Assert
		if(missing != NULL) {
			cout << "FreeTypeWrapperTest [NewFace::NonExistentPath_ReturnsNullAndWrapperStaysUsable]: expected NULL for a non-existent path" << endl;
			break;
		}
		validFace = ft.NewFace(BuildRelativeInputPath(argv, "fonts/arial.ttf"), 0);
		if(validFace == NULL) {
			cout << "FreeTypeWrapperTest [NewFace::NonExistentPath_ReturnsNullAndWrapperStaysUsable]: a valid load failed after a failed one" << endl;
			break;
		}
		if(FT_Get_Char_Index(validFace, 'A') == 0) {
			cout << "FreeTypeWrapperTest [NewFace::NonExistentPath_ReturnsNullAndWrapperStaysUsable]: no glyph for 'A' on the recovered face" << endl;
			break;
		}
		ok = true;
	} while(false);

	if(validFace != NULL)
		ft.DoneFace(validFace);
	return ok;
}

// V-075: NewFace(file, secondaryFile, index) with a valid primary font but
// a non-existent secondary path. FillOpenFaceArgumentsForUTF8String fails,
// the inner do-while breaks early, then the function reads
// attachStreamArguments via CloseOpenFaceArgumentsStream. Pre-fix this was
// only safe by Fill always zeroing .stream; the fix value-initializes the
// FT_Open_Args so the early-fail path is well-defined regardless. Asserts
// the observable contract: NULL, no crash, wrapper still usable.
static bool NewFaceSecondary_BadSecondaryPath_ReturnsNullAndWrapperStaysUsable(char* argv[]) {
	bool ok = false;
	FreeTypeWrapper ft;
	FT_Face recovered = NULL;
	do {
		// Arrange / Act
		FT_Face face = ft.NewFace(BuildRelativeInputPath(argv, "fonts/arial.ttf"),
		                          "/no/such/secondary/file.afm",
		                          0);

		// Assert
		if(face != NULL) {
			cout << "FreeTypeWrapperTest [NewFaceSecondary::BadSecondaryPath_ReturnsNullAndWrapperStaysUsable]: expected NULL when the secondary path is bad" << endl;
			break;
		}
		recovered = ft.NewFace(BuildRelativeInputPath(argv, "fonts/arial.ttf"), 0);
		if(recovered == NULL) {
			cout << "FreeTypeWrapperTest [NewFaceSecondary::BadSecondaryPath_ReturnsNullAndWrapperStaysUsable]: a valid load failed after the bad-secondary attempt" << endl;
			break;
		}
		if(FT_Get_Char_Index(recovered, 'A') == 0) {
			cout << "FreeTypeWrapperTest [NewFaceSecondary::BadSecondaryPath_ReturnsNullAndWrapperStaysUsable]: no glyph for 'A' on the recovered face" << endl;
			break;
		}
		ok = true;
	} while(false);

	if(recovered != NULL)
		ft.DoneFace(recovered);
	return ok;
}

int FreeTypeWrapperTest(int argc, char* argv[]) {
	(void)argc;

	if(!DoneFace_UnregisteredFace_DoesNotCorruptStreamMap(argv)) return 1;
	if(!NewFaceThenDoneFace_RegisteredFace_RoundTrips(argv)) return 1;
	if(!NewFace_NonExistentPath_ReturnsNullAndWrapperStaysUsable(argv)) return 1;
	if(!NewFaceSecondary_BadSecondaryPath_ReturnsNullAndWrapperStaysUsable(argv)) return 1;
	return 0;
}
