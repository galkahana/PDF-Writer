#pragma once
/*
	see comment in Catalog Information for details on the algorithm to build the page tree
*/


#include "ObjectsBasicTypes.h"

class IndirectObjectsReferenceRegistry;

#define PAGE_TREE_LEVEL_SIZE 10

class PageTree
{
public:
	PageTree(IndirectObjectsReferenceRegistry& inObjectsRegistry);
	~PageTree(void);

	ObjectIDType GetID();
	PageTree* GetParent();
	bool IsLeafParent();
	int GetNodesCount();
	// will return null for improper indexes or if has page IDs as children
	PageTree* GetPageTreeChild(int i);

	// will return 0 for improper indexes or if has page nodes as children
	ObjectIDType GetPageIDChild(int i);

	PageTree* AddNodeToTree(ObjectIDType inNodeID,IndirectObjectsReferenceRegistry& inObjectsRegistry);

	PageTree* CreateBrotherOrCousin(IndirectObjectsReferenceRegistry& inObjectsRegistry);
	PageTree* AddNodeToTree(PageTree* inPageTreeNode,IndirectObjectsReferenceRegistry& inObjectsRegistry);

	void SetParent(PageTree* inParent);
private:
	PageTree* mParent;
	ObjectIDType mPageTreeID;
	bool mIsLeafParent;

	int mKidsIndex;
	PageTree* mKidsNodes[PAGE_TREE_LEVEL_SIZE];
	ObjectIDType mKidsIDs[PAGE_TREE_LEVEL_SIZE];

};
