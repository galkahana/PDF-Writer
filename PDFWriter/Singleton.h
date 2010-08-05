#pragma once

#ifndef NULL
#define NULL 0
#endif

template <class T> 
class Singleton
{
public:
	static T* GetInstance();
	static void Reset();
private:
	Singleton();
	static T* mInstance;
};

template <class T>
T* Singleton<T>::mInstance = 0;

template <class T>
T* Singleton<T>::GetInstance()
{
	if(!mInstance)
		mInstance = new T();	
	return mInstance;
}

template <class T>
void Singleton<T>::Reset()
{
	delete mInstance;
	mInstance = NULL;
}