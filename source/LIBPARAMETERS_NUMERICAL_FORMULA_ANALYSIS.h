//# Licensed under the BSD 3-Clause License - https://opensource.org/licenses/BSD-3-Clause

/*! @file
@brief formula analysis

@code
BNF
//AdditionAndSubtraction :: = <MultiplicationAndDivision> | {<MultiplicationAndDivision>{+|-}{<AdditionAndSubtraction}>}
AdditionAndSubtraction :: = <MultiplicationAndDivision> | {<AdditionAndSubtraction>{+|-}<MultiplicationAndDivision>}
MultiplicationAndDivision :: = <Pulus_MinusSign> | {<Pulus_MinusSign> {*|/} <MultiplicationAndDivision>}
Pulus_MinusSign :: = {+|-| } <Power>
Power	 :: = <Factor> | {<Power> {^} <Factor>}
Factor	 :: = <Number> | (<AdditionAndSubtraction>)
@endcode

@author Kazumasa HORIE
@date 2015/11/30~
*/

#pragma once
#include <string>


namespace libsdnn
{
	namespace parameters
	{
		namespace nfa
		{
			double NumericalFormulaAnalysis(const std::string &formula);
		}
	}
}
