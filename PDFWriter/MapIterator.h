#pragma once
#include "ContainerIterator.h"

/*
	Generic template for maps single-self-contained iterator.
	useful for external loops. instead of providing two iterators (map.begin and map.end)
	provide a single iterator.
	usage:

	MapIterator<MyMapType> mapIterator(myMapOfMapType);

	while(mapIterator.MoveNext())
	{
		// do something with m.GetKey() and m.GetValue(), which point to the current iterated key and value
	}
*/

template <class T>
class MapIterator : public ContainerIterator<T>
{
public:

	MapIterator(T& inMap);
	MapIterator(const MapIterator<T>& inOtherIterator);

	typename T::key_type GetKey();
	typename T::mapped_type GetValue();

};

template <class T>
MapIterator<T>::MapIterator(T& inMap):ContainerIterator(inMap)
{
}

template <class T>
MapIterator<T>::MapIterator(const MapIterator<T>& inOtherIterator):ContainerIterator(inOtherIterator)
{
}

template <class T>
typename T::key_type MapIterator<T>::GetKey()
{
	return mCurrentPosition->first;
}

template <class T>
typename T::mapped_type MapIterator<T>::GetValue()
{
	return mCurrentPosition->second;
}