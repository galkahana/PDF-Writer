#pragma once
/*
	In building the page tree i'm using a balanced tree sort of algorithm.
	The tree lowest level is the only one holding actual pages ID. all levels above it are Page Tree containing Page Trees.
	The adding algorithm tries to add to the current held node. if unsuccesful asks from its parent to create a "brother" page tree.
	if brother page tree is created than the addition is made to it. if unsuccesful the task is carried out to a higher heirarchy, for a "cousin" to be created.
	then the addition is made on the cousin.

	this algorithm creates a balanced tree, and the mCurrentPageTreeNode of The CatalogInformation will always hold the "latest" low node.
	writing the page tree in the end simply goes up the page tree to the root and uses recursion to walk over the tree.

*/

#include "ObjectsBasicTypes.h"

class PageTree;
class IndirectObjectsReferenceRegistry;

class CatalogInformation
{
public:
	CatalogInformation(void);
	~CatalogInformation(void);

	ObjectIDType AddPageToPageTree(ObjectIDType inPageID,IndirectObjectsReferenceRegistry& inObjectsRegistry);

	// indirect objects registry is passed in case there's no page tree root...in which case it'll automatically create one
	PageTree* GetPageTreeRoot(IndirectObjectsReferenceRegistry& inObjectsRegistry);

private:

	PageTree* mCurrentPageTreeNode;
};
