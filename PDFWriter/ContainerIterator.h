#pragma once

/*
	Abstract Generic template for list single-self-contained iterator.
	useful for external loops. instead of providing two iterators (map.begin and map.end)
	provide a single iterator.
	usage:

	SingleValueContainerIterator<MyListType> SingleValueContainerIterator(myListOfListType);

	while(SingleValueContainerIterator.MoveNext())
	{
		// do something derived class item getter
	}
*/

template <class T>
class ContainerIterator
{
public:

	ContainerIterator(T& inContainer);
	ContainerIterator(const ContainerIterator<T>& inOtherIterator);

	bool MoveNext();
	bool IsFinished();

private:
	
	typename T::iterator mEndPosition;
	bool mFirstMove;

protected:
	typename T::iterator mCurrentPosition;
};

template <class T>
ContainerIterator<T>::ContainerIterator(T& inList)
{
	mCurrentPosition = inList.begin();
	mEndPosition = inList.end();
	mFirstMove = true;
}

template <class T>
ContainerIterator<T>::ContainerIterator(const ContainerIterator<T>& inOtherIterator)
{
	mCurrentPosition = inOtherIterator.mCurrentPosition;
	mEndPosition = inOtherIterator.mEndPosition;
	mFirstMove = inOtherIterator.mFirstMove;
}

template <class T>
bool ContainerIterator<T>::MoveNext()
{
	if(mCurrentPosition == mEndPosition)
		return false;
	if(mFirstMove)
	{
		mFirstMove = false;
	}
	else
	{
		if(++mCurrentPosition == mEndPosition)
			return false;
	}
	return true;
}

template <class T>
bool ContainerIterator<T>::IsFinished()
{
	return mCurrentPosition == mEndPosition;
}
