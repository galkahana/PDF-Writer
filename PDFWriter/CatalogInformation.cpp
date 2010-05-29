#include "CatalogInformation.h"
#include "PageTree.h"
#include "IndirectObjectsReferenceRegistry.h"

CatalogInformation::CatalogInformation(void)
{
	mCurrentPageTreeNode = NULL;
}

CatalogInformation::~CatalogInformation(void)
{
	delete mCurrentPageTreeNode;
}

ObjectIDType CatalogInformation::AddPageToPageTree(ObjectIDType inPageID,IndirectObjectsReferenceRegistry& inObjectsRegistry)
{
	if(!mCurrentPageTreeNode)
		mCurrentPageTreeNode = new PageTree(inObjectsRegistry);

	mCurrentPageTreeNode = mCurrentPageTreeNode->AddNodeToTree(inPageID,inObjectsRegistry);
	return mCurrentPageTreeNode->GetID();
}


PageTree* CatalogInformation::GetPageTreeRoot(IndirectObjectsReferenceRegistry& inObjectsRegistry)
{
	if(mCurrentPageTreeNode)
	{
		PageTree* resultPageTree = mCurrentPageTreeNode;
		while(resultPageTree->GetParent())
			resultPageTree = resultPageTree->GetParent();
		return resultPageTree;
	}
	else
	{
		mCurrentPageTreeNode = new PageTree(inObjectsRegistry);
		return mCurrentPageTreeNode;
	}
}


