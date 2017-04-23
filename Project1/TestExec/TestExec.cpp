#include "TestExec.h"
#include "../Tokenizer/Tokenizer.h"
#include "../SemiExp/SemiExp.h"
#include "fstream"
#include "iostream"
///////////////////////////////////////////////////////////////////////
// TestExec.cpp - demonstrates requirements for Tokenizer and        // 
//                SemiExpression                                     //
// ver 1.0                                                           //
// Language:      C++, Visual Studio 2015                            //
// Application:   Parser component, CSE687 - Object Oriented Design  //
// Author:        Satish Goswami, Syracuse University                //
//                sgoswami@syr.edu                                   //
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

using namespace Scanner;

TestExec :: TestExec(){}

void TestExec :: testTokenizer(Toker &toker, std::istream &in)
{
	toker.setCollectComments(true);
	std::cout << "\n\n <-----------Tokenizer (Displaying Tokens)------------> \n";
	toker.setSpecialSingleChars("@");      //Requirement 4 (appending to default special special character set)
	toker.setSpecialCharPairs("/=");
	std::cout << "\n\n ::Function SetSpecialChars(ssc) & setSpecialCharPairs(scp) Implemented in Tokenizer.cpp and called in TestExec.cpp\n";
	do
	{
		std::string tok = toker.getTok();
		if (tok == "\n")
			tok = "newline";
		std::cout << "\n\n -- " << tok;
	} while (in.good());

	std::cout << "\n\n";
}

void TestExec::testSemiExp(Toker& toker, std::istream& in)
{
	toker.setCollectComments(false);
	std::cout << "\n\n <-----------SemiExpressions------------> \n";
	SemiExp semi(&toker);
	while (semi.get())
	{
		std::cout << "\n  -- SemiExpression --";
		semi.show();
	}
	/*
	May have collected tokens, but reached end of stream
	before finding SemiExp terminator.
	*/
	if (semi.length() > 0)
	{
		std::cout << "\n  -- SemiExpression --";
		semi.show();
		std::cout << "\n\n";
	}
	std::string fileSpecITok = "ITokTestFile.txt";

	//--Tokenizer Test Setup
	Toker t;
	SemiExp sem(&t);
	std::fstream inITok(fileSpecITok);
	if (!inITok.good())
	{
		std::cout << "\n  can't open file " << fileSpecITok << "\n\n";
		return;
	}
	t.attach(&inITok);
	demonstrateRequirement9(sem, inITok);
}

void TestExec :: demonstrateRequirement9(SemiExp &semi, std::istream& in)
{
	std::cout << "\n\n\n <-----------ITokCollection Interface methods Implementation------------> \n";
	semi.clear();
	semi.get();
	int index = 2; std::string str = "itok";
	//std::cout << "\n Testing ITokCollection Interface functions : \n";
	std::cout << "\n\n - Token Vector contains : ";
	semi.show(true);
	semi.trimFront();
	std::cout << "\n - Token Vector after trim contains : ";
	semi.show();
	semi.toLower();
	std::cout << "\n - Token Vector after toLower contains : ";
	semi.show();
	semi.remove(2);
	std::cout << "\n - Token Vector after removing from position "<<index<<" contains : ";
	semi.show();
	semi.remove("itok");  //uses find function
	std::cout << "\n - Token Vector after removing \"" << str << "\" contains : ";
	semi.show();
	std::cout << "\n - Token at position " << index << " in Token Vector is : " << semi[index];
	semi.clear();
	std::cout << "\n - Token Vector after clear() method contains : ";
	semi.show();
	std::cout << "\n\n";
}

TestExec :: ~TestExec() {}


int main()
{
	TestExec testExec;
	std::string fileSpecTokenizer = "../Tokenizer/TokenizerTestFile.txt";
	std::string fileSpecSemiExp = "../SemiExp/SemiExpTestFile.txt";

	//--Tokenizer Test Setup
	std::fstream inTokenizer(fileSpecTokenizer);
	if (!inTokenizer.good())
	{
		std::cout << "\n  can't open file " << fileSpecTokenizer << "\n\n";
		return 1;
	}
	Toker tokerForTokenizer;
	tokerForTokenizer.attach(&inTokenizer);
	testExec.testTokenizer(tokerForTokenizer, inTokenizer);

	//--SemiExp Test Setup
	std::fstream inSemiExp(fileSpecSemiExp);
	if (!inSemiExp.good())
	{
		std::cout << "\n  can't open file " << fileSpecSemiExp << "\n\n";
		return 1;
	}
	Toker tokerForSemiExp;
	tokerForSemiExp.attach(&inSemiExp);
	testExec.testSemiExp(tokerForSemiExp, inSemiExp);
}