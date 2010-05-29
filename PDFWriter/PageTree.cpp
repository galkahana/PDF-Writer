#include "PageTree.h"
#include "IndirectObjectsReferenceRegistry.h"

PageTree::PageTree(IndirectObjectsReferenceRegistry& inObjectsRegistry)
{
	mPageTreeID = inObjectsRegistry.AllocateNewObjectID();
	mKidsIndex = 0;
	mIsLeafParent = true;
	mParent = NULL;
}

PageTree::~PageTree(void)
{
	if(!mIsLeafParent)
	{
		for(int i=0;i<mKidsIndex;++i)
			delete mKidsNodes[i];
	}
}

PageTree* PageTree::AddNodeToTree(ObjectIDType inNodeID,IndirectObjectsReferenceRegistry& inObjectsRegistry)
{
	if(mKidsIndex < PAGE_TREE_LEVEL_SIZE)
	{
		mKidsIDs[mKidsIndex++] = inNodeID;
		mIsLeafParent = true;
		return this;
	}
	else
	{
		if(!mParent)
		{
			mParent = new PageTree(inObjectsRegistry);
			mParent->AddNodeToTree(this,inObjectsRegistry); // will surely succeed - first one
		}
		PageTree* brotherOrCousin = mParent->CreateBrotherOrCousin(inObjectsRegistry);
		brotherOrCousin->AddNodeToTree(inNodeID,inObjectsRegistry); // will surely succeed - first one
		return brotherOrCousin;
	}
}

PageTree* PageTree::AddNodeToTree(PageTree* inPageTreeNode,IndirectObjectsReferenceRegistry& inObjectsRegistry)
{
	if(mKidsIndex < PAGE_TREE_LEVEL_SIZE)
	{
		mKidsNodes[mKidsIndex++] = inPageTreeNode;
		mIsLeafParent = false;
		inPageTreeNode->SetParent(this);
		return this;
	}
	else
	{
		if(!mParent)
		{
			mParent = new PageTree(inObjectsRegistry);
			mParent->AddNodeToTree(this,inObjectsRegistry); // will surely succeed - first one
		}
		PageTree* brotherOrCousin = mParent->CreateBrotherOrCousin(inObjectsRegistry);
		brotherOrCousin->AddNodeToTree(inPageTreeNode,inObjectsRegistry); // will surely succeed - first one
		return brotherOrCousin;
	}
}

PageTree* PageTree::CreateBrotherOrCousin(IndirectObjectsReferenceRegistry& inObjectsRegistry)
{
	if(mKidsIndex < PAGE_TREE_LEVEL_SIZE)
	{
		mKidsNodes[mKidsIndex] = new PageTree(inObjectsRegistry);
		mIsLeafParent = false;
		mKidsNodes[mKidsIndex]->SetParent(this);
		return mKidsNodes[mKidsIndex++];
	}
	else
	{
		if(!mParent)
		{
			mParent = new PageTree(inObjectsRegistry);
			mParent->AddNodeToTree(this,inObjectsRegistry); // will surely succeed - first one
		}		
		PageTree* brotherOrCousin = mParent->CreateBrotherOrCousin(inObjectsRegistry);
		PageTree* nephew = brotherOrCousin->CreateBrotherOrCousin(inObjectsRegistry); // will surely create brother - first one
		return nephew;
	}
}

ObjectIDType PageTree::GetID()
{
	return mPageTreeID;
}

PageTree* PageTree::GetParent()
{
	return mParent;
}

bool PageTree::IsLeafParent()
{
	return mIsLeafParent;
}

int PageTree::GetNodesCount()
{
	return mKidsIndex;
}

PageTree* PageTree::GetPageTreeChild(int i)
{
	if(mIsLeafParent || mKidsIndex <= i)
		return NULL;
	else
		return mKidsNodes[i];

}

ObjectIDType PageTree::GetPageIDChild(int i)
{
	if(!mIsLeafParent || mKidsIndex <= i)
		return 0;
	else
		return mKidsIDs[i];
}

void PageTree::SetParent(PageTree* inParent)
{
	mParent = inParent;
}
