#pragma once

/*
	Smart pointer implementing automatic ref count addition/reduction

	Constructors assume that the object was AddRefed, and will make sure that on Ptr destructin Release is called.
	other operators take care of various scenarios/

*/ 


template <typename T>
class RefCountPtr
{
public:

	RefCountPtr();
	// This one will not call AddRef (assume called from outside)
	RefCountPtr(T* inValue);

	// This one calls Release
	virtual ~RefCountPtr();

	// The next two call AddRef in insert
	RefCountPtr<T>&  operator =(T* inValue); 
	RefCountPtr<T>&  operator =(RefCountPtr<T>& inOtherPtr); 

	// Will return the actual pointer
	T* operator->();

	// equality/inequality for included pointer
	bool operator ==(T* inOtherValue); 
	bool operator ==(RefCountPtr<T>& inOtherPtr); 

	bool operator !=(T* inOtherValue); 
	bool operator !=(RefCountPtr<T>& inOtherPtr); 

	// get pointer directly
	T* GetPtr();

	bool operator!() const;

private:
	T* mValue;
};

template <typename T>
RefCountPtr<T>::RefCountPtr()
{
	mValue = NULL;
}

template <typename T>
RefCountPtr<T>::RefCountPtr(T* inValue)
{
	mValue = inValue;
}

// This one calls Release
template <typename T>
RefCountPtr<T>::~RefCountPtr()
{
	mValue->Release();
}

template <typename T>
RefCountPtr<T>&  RefCountPtr<T>::operator =(T* inValue)
{
	if(mValue)
		mValue->Release();
	mValue = inValue;
	if(mValue)
		mValue->AddRef();
	return *this;
}

template <typename T>
RefCountPtr<T>&  RefCountPtr<T>::operator =(RefCountPtr<T>& inOtherPtr)
{
	if(mValue)
		mValue->Release();
	mValue = inOtherPtr.mValue;
	if(mValue)
		mValue->AddRef();
	return *this;
}


// Will return the actual pointer
template <typename T>
T* RefCountPtr<T>::operator->()
{
	return mValue;
}

template <typename T>
bool RefCountPtr<T>::operator ==(T* inOtherValue)
{
	return mValue == inOtherValue;
}

template <typename T>
bool RefCountPtr<T>::operator ==(RefCountPtr<T>& inOtherPtr)
{
	return mValue == inOtherPtr.mValue;
}

template <typename T>
bool RefCountPtr<T>::operator !=(T* inOtherValue)
{
	return mValue != inOtherValue;
}

template <typename T>
bool RefCountPtr<T>::operator !=(RefCountPtr<T>& inOtherPtr)
{
	return mValue != inOtherPtr.mValue;
}

template <typename T>
T* RefCountPtr<T>::GetPtr()
{
	return mValue;
}

template <typename T>
bool RefCountPtr<T>::operator!() const
{
	return !mValue;
}
