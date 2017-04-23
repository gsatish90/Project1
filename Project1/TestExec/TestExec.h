#ifndef TESTEXEC_H
#define TESTEXEC_H
///////////////////////////////////////////////////////////////////////
// TestExec.h - demonstrates requirements for Tokenizer and          // 
//              SemiExpression                                       //
// ver 1.0                                                           //
// Language:    C++, Visual Studio 2015                              //
// Application: Parser component, CSE687 - Object Oriented Design    //
// Author:      Satish Goswami, Syracuse University                  //
//              sgoswami@syr.edu                                     //
///////////////////////////////////////////////////////////////////////
/*
* Package Operations:
* -------------------
* This package provides the public TestExec class and its functions to test
* Tokenizer and SemiExp individually.
*
* Build Process:
* --------------
* Required Files:
*   TestExec.h, TestExec.cpp, SemiExpression.h, SemiExpression.cpp, 
*   Tokenizer.h, Tokenizer.cpp
*
* Build Command: devenv Project1.sln /rebuild debug
*
* Maintenance History:
* --------------------
* ver 1.0 : 08 Feb 2016
* - Made class and added functions to test Tokenizer and SemiExp 
*
*/

#include "../Tokenizer/Tokenizer.h"
#include "../SemiExp/SemiExp.h"

namespace Scanner {

	class TestExec
	{
	public:
		TestExec();
		void testTokenizer(Toker& toker, std::istream& in);
		void testSemiExp(Toker& toker, std::istream& in);
		void demonstrateRequirement9(SemiExp &semi, std::istream& in);
		~TestExec();
	};
}
#endif

