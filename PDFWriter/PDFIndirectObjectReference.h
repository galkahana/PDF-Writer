#pragma once

#include "PDFObject.h"
#include "ObjectsBasicTypes.h"

class PDFIndirectObjectReference : public PDFObject
{
public:

	enum EType
	{
		eType = ePDFObjectIndirectObjectReference
	};

	PDFIndirectObjectReference(ObjectIDType inObjectID,unsigned long inVersion);
	virtual ~PDFIndirectObjectReference(void);

	ObjectIDType mObjectID;
	unsigned long mVersion;

};
