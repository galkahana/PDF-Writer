/*
   Source File : RefCountTest.cpp


   Copyright 2011 Gal Kahana PDFWriter

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.

   
*/
#include "RefCountTest.h"
#include "RefCountPtr.h"
#include "RefCountObject.h"

#include <iostream>

using namespace std;
using namespace PDFHummus;

RefCountTest::RefCountTest(void)
{
}

RefCountTest::~RefCountTest(void)
{
}

// this will serve as our sample class
class MyClass : public RefCountObject
{
public:
	static int TotalObjectsCount;

	MyClass(int inID){mID = inID; ++TotalObjectsCount;}
	virtual ~MyClass(){--TotalObjectsCount;}
	
	int GetID(){return mID;}

private:
	int mID;
};

int MyClass::TotalObjectsCount = 0;

EStatusCode RefCountTest::Run(const TestConfiguration& inTestConfiguration)
{
	EStatusCode status = PDFHummus::eSuccess;

	if(MyClass::TotalObjectsCount != 0)
	{
		cout<<"Total objects count is supposed to be 0 at the beginning, but it's "<<MyClass::TotalObjectsCount<<"\n";
		status = PDFHummus::eFailure;
	}

	// simple 1 ref test
	{
		RefCountPtr<MyClass> firstPtr(new MyClass(1));
		if(MyClass::TotalObjectsCount != 1)
		{
			cout<<"simple 1 ref test failed, TotalObjectCount (should be 1) = "<<MyClass::TotalObjectsCount<<"\n";
			status = PDFHummus::eFailure;
		}
		if(firstPtr->GetID() != 1)
		{
			cout<<"simple 1 ref test failed, wrond object id\n";
			status = PDFHummus::eFailure;
		}
	}
	if(MyClass::TotalObjectsCount != 0)
	{
		cout<<"simple 1 ref test failed, TotalObjectCount (should be 0) = "<<MyClass::TotalObjectsCount<<"\n";
		status = PDFHummus::eFailure;
	}

	// 2 refs to direct object
	{
		MyClass* aClass = new MyClass(2);

		if(MyClass::TotalObjectsCount != 1)
		{
			cout<<"Total objects count is supposed to be 1 after creating object 2, but it's "<<MyClass::TotalObjectsCount<<"\n";
			status = PDFHummus::eFailure;
		}
		RefCountPtr<MyClass> firstPtr(aClass);
		if(firstPtr->GetID() != 2)
		{
			cout<<"2 ref test failed, wrond object id for pointer 1\n";
			status = PDFHummus::eFailure;
		}

		aClass->AddRef();
		RefCountPtr<MyClass> secondPtr(aClass);
		if(secondPtr->GetID() != 2)
		{
			cout<<"2 ref test failed, wrond object id for pointer 2\n";
			status = PDFHummus::eFailure;
		}
		if(MyClass::TotalObjectsCount != 1)
		{
			cout<<"Total objects count is supposed to be 1 after another pointer for object 2, but it's "<<MyClass::TotalObjectsCount<<"\n";
			status = PDFHummus::eFailure;
		}
	}
	if(MyClass::TotalObjectsCount != 0)
	{
		cout<<"2 ref test failed, TotalObjectCount = "<<MyClass::TotalObjectsCount<<"\n";
		status = PDFHummus::eFailure;
	}

	// assignment scenarios
	{
		RefCountPtr<MyClass> firstPtr(new MyClass(3));

		if(MyClass::TotalObjectsCount != 1)
		{
			cout<<"Total objects count is supposed to be 1 after creating object 3, but it's "<<MyClass::TotalObjectsCount<<"\n";
			status = PDFHummus::eFailure;
		}
		if(firstPtr->GetID() != 3)
		{
			cout<<"assignment test failed, wrond object id for pointer 1\n";
			status = PDFHummus::eFailure;
		}

		{
			RefCountPtr<MyClass> secondPtr;

			secondPtr = firstPtr;
			if(secondPtr->GetID() != 3)
			{
				cout<<"assignment test failed, wrond object id for pointer 2\n";
				status = PDFHummus::eFailure;
			}
		}
		if(MyClass::TotalObjectsCount != 1)
		{
			cout<<"Total objects count is supposed to be 1 after smart pointer assignment object 3, but it's "<<MyClass::TotalObjectsCount<<"\n";
			status = PDFHummus::eFailure;
		}

		{
			RefCountPtr<MyClass> thirdPtr;

			thirdPtr = firstPtr.GetPtr();
			if(thirdPtr->GetID() != 3)
			{
				cout<<"assignment test failed, wrond object id for pointer 2\n";
				status = PDFHummus::eFailure;
			}
		}
		if(MyClass::TotalObjectsCount != 1)
		{
			cout<<"Total objects count is supposed to be 1 after pointer assignment object 3, but it's "<<MyClass::TotalObjectsCount<<"\n";
			status = PDFHummus::eFailure;
		}
	}
	if(MyClass::TotalObjectsCount != 0)
	{
		cout<<"assignment test failed, TotalObjectCount = "<<MyClass::TotalObjectsCount<<"\n";
		status = PDFHummus::eFailure;
	}

	// pointer equality
	{
		MyClass* anObject = new MyClass(4);
		
		RefCountPtr<MyClass> firstPtr(anObject);
		RefCountPtr<MyClass> secondPtr = firstPtr;

		if(firstPtr != secondPtr)
		{
			cout<<"smart pointers equality failed (not equal)\n";
			status = PDFHummus::eFailure;
		}

		if(!(firstPtr == secondPtr))
		{
			cout<<"smart pointers equality failed (equal)\n";
			status = PDFHummus::eFailure;
		}

		if(firstPtr != anObject)
		{
			cout<<"smart pointer to pointer equality failed (not equal)\n";
			status = PDFHummus::eFailure;
		}

		if(!(firstPtr == anObject))
		{
			cout<<"smart pointer to pointer equality failed (equal)\n";
			status = PDFHummus::eFailure;
		}
	}
	if(MyClass::TotalObjectsCount != 0)
	{
		cout<<"Pointer equality, TotalObjectCount = "<<MyClass::TotalObjectsCount<<"\n";
		status = PDFHummus::eFailure;
	}


	// boolean test
	{
		RefCountPtr<MyClass> aPtr(new MyClass(5));

		if(!aPtr)
		{
			cout<<"Problem, should not be false!\n";
			status = PDFHummus::eFailure;
		}		
		
		RefCountPtr<MyClass> aNother;
		if(!(!aNother))
		{
			cout<<"Problem, should be false!\n";
			status = PDFHummus::eFailure;			
		}
	}

	return status;
}


ADD_CATEGORIZED_TEST(RefCountTest,"PDFEmbedding")
