#pragma once

#include "ContainerIterator.h"

/*
	Generic template for list single-self-contained iterator.
	useful for external loops. instead of providing two iterators (list.begin and list.end)
	provide a single iterator.
	usage:

	SingleValueContainerIterator<MyListType> SingleValueContainerIterator(myListOfListType);

	while(SingleValueContainerIterator.MoveNext())
	{
		// do something with m.GetItem()
	}
*/

template <class T>
class SingleValueContainerIterator : public ContainerIterator<T>
{
public:

	SingleValueContainerIterator(T& inList);
	SingleValueContainerIterator(const SingleValueContainerIterator<T>& inOtherIterator);

	typename T::value_type GetItem();
};

template <class T>
SingleValueContainerIterator<T>::SingleValueContainerIterator(T& inList):ContainerIterator(inList)
{
}

template <class T>
SingleValueContainerIterator<T>::SingleValueContainerIterator(const SingleValueContainerIterator<T>& inOtherIterator):ContainerIterator(inOtherIterator)
{
}

template <class T>
typename T::value_type SingleValueContainerIterator<T>::GetItem()
{
	return *mCurrentPosition;
}

