#pragma once

template <typename T>
static bool betweenIncluding(T inTest,T inLowerBound,T inHigherBound)
{
	return inTest>= inLowerBound && inTest<=inHigherBound;
}