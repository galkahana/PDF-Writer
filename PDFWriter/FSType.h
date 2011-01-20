#pragma once

class FSType
{
public:
	FSType(unsigned short inFSTypeValue) {mFSTypeValue = inFSTypeValue;}

	// using Adobe applications policy as published in FontPolicies.pdf
	bool CanEmbed(){return (mFSTypeValue & 0x2) == 0 && (mFSTypeValue & 0x0200) == 0;}
private:

	unsigned short mFSTypeValue;
};