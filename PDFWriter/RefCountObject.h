#pragma once

class RefCountObject
{
public:
	RefCountObject(void); // Constructor automatically initiates the first Ref Count
	virtual ~RefCountObject(void);

	void AddRef();
	void Release();

private:

	unsigned long mRefCount;
};
