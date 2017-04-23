///////////////////////////////////////////////////////////////////////
// SemiExpression.cpp - collect tokens for analysis                  //
// ver 3.2                                                           //
// Language:    C++, Visual Studio 2015                              //
// Application: Parser component, CSE687 - Object Oriented Design    //
// Source:      Jim Fawcett, Syracuse University, CST 4-187          //
//              jfawcett@twcny.rr.com                                //
// Author:      Satish Goswami, Syracuse University                  //
//              sgoswami@syr.edu                                     //
///////////////////////////////////////////////////////////////////////
/*
* Package Operations:
* -------------------
* This package provides a public SemiExp class that collects and makes
* available sequences of tokens.  SemiExp uses the services of a Toker
* class to acquire tokens.  Each call to SemiExp::get() returns a
* sequence of tokens that ends in {, ;, etc. Conditions for handling
* for is taken care. The entire for gets returned as a single token
* The code also handles the preprocessor directives and displays them
* as a single token. If a ':' is preceeded by any access specifier 
* such as public, private or protected then the entire string should 
* come as a single semi-expression.
*
* Build Process:
* --------------
* Required Files:
*   SemiExpression.h, SemiExpression.cpp, Tokenizer.h, Tokenizer.cpp
*
* Build Command: devenv Project1.sln /rebuild debug
*
* Maintenance History:
* --------------------
* ver 3.2 : 08 Feb 2016
* - Implemented ITokCollection interface.
* - added conditions for checking  comments, for, preprocessor directives
* - added functions in method get() to refactor the code and reduce lines
* ver 3.1 : 02 Feb 2016
* - declared SemiExp copy constructor and assignment operator = delete
* - added default argument for Toker pointer to nullptr so SemiExp
*
*/
#include <fstream>
#include <iostream>
#include <string>
#include <algorithm>
#include <unordered_map>
#include <exception>
#include "SemiExp.h"
#include "../Tokenizer/Tokenizer.h"

using namespace Scanner;

SemiExp::SemiExp(Toker* pToker) : _pToker(pToker) {}

bool SemiExp::get(bool clear)
{
  if (_pToker == nullptr)
    throw(std::logic_error("no Toker reference"));
  std::string currentState = "";
  _tokens.clear();
  int semiColonCount = 0;
  while (true)
  {
	if (semiColonCount == 2)
	{
	  semiColonCount = 0;
	  currentState = "FORISCLOSED";
	}
    std::string token = _pToker->getTok();
    if (token == "")
      break;
	if (token == ":")     //public, private, protected
	{
		if ((_tokens.back() == "public") || (_tokens.back() == "private") || (_tokens.back() == "protected"))
		{
			_tokens.push_back(token);
			return true;
		}
	}
	checkForSingleComment(token, currentState);
	if (token == "#")  //process preprocessor directives
		currentState = "PREPROCESSOR";
	checkForQuotes(token, currentState);
	if (token == "\n" && ((currentState == "PREPROCESSOR") || currentState == "FORISCLOSED" || currentState == "SINGLECOMMENT" || currentState == "QUOTES" ))
		return true;
	if (token == "for")
		currentState = "FORISOPEN";
    _tokens.push_back(token);     //pushing tokens however into _token vector
	if (checkForMultiComments(token) == true)
		return true;
    if (token == "{") return true;
	if (token == "}") return true;
	if (token == ";;" && currentState == "FORISOPEN")
		currentState = "FORISCLOSED";
	if ((token == ";") && ((currentState == "FORISOPEN") && (semiColonCount < 2)))
	{
		semiColonCount++;
		continue;
	}
	else if (token == ";")
		return true;
  }
  return false;
}


void SemiExp :: checkForSingleComment(Token token, Token&currentState)
{
	try
	{
		//std::cout << "\n in checkforsinglecommenttokenis -> " << token<< "||";
		if (token[0] == '/' && token[1] == '/')
			currentState = "SINGLECOMMENT";
	}
	catch (std::exception e) {}
}

void SemiExp :: checkForQuotes(Token token, Token& currentState)
{
	try
	{
		if ((token[token.length() - 1] == '"' || token[token.length() - 1] == '\'') && (!(currentState == "SINGLECOMMENT") || !(currentState == "PREPROCESSOR")))
			currentState = "QUOTES";
	}
	catch (std::exception e) {}
}

//--checks to see if closing symobol of multi-line comments has been encountered
bool SemiExp :: checkForMultiComments(Token& token)
{
	try
	{
		Token cToken = " " + token;
		if ((cToken[cToken.length() - 1] == '/') && (cToken[cToken.length() - 2] == '*'))
		{
			//std::cout << "hello";
			return true;
		}
		else
			return false;
		//std::cout << "Token after close -> " << token << " <-";
	}
	catch (std::exception e) { return false; }
	return false;
}

size_t SemiExp::length()
{
  return _tokens.size();
}

Token& SemiExp::operator[](int n)
{
		if (n < 0 || n >= (int)_tokens.size())
			throw(std::invalid_argument("index out of range"));
		return _tokens[n];
}

size_t SemiExp::find(const std::string& tok) 
{
	unsigned int i = 0;
	while (i < _tokens.size())
	{
		if (_tokens.at(i) == tok)
			return i;
		i++;
	}
	return i;
}

void SemiExp::push_back(const std::string& tok)
{
	_tokens.push_back(tok);
}

bool SemiExp::merge(const std::string& firstTok, const std::string& secondTok)
{
	return true;
}

bool SemiExp::remove(const std::string& tok)
{
	unsigned int index = find(tok);
	if (index >= _tokens.size())
		return false;
	_tokens.erase(_tokens.begin() + index);
	return true;
}

bool SemiExp::remove(size_t i)
{
	_tokens.erase(_tokens.begin() + i);
	return true;
}

void SemiExp::toLower()
{
	for (int i = 0; i < (int)_tokens.size(); i++)
		std::transform(_tokens[i].begin(), _tokens[i].end(), _tokens[i].begin(), tolower);
}

//--trims the leading newlines inside a semi-expression
void SemiExp::trimFront()
{
	if (_tokens.size() == 0)
		return;
	int i = 0;
	while (_tokens[i]=="\n")
	{
		remove(i);
		if (_tokens.size() == 0)
			break;
	}
}

//clears token's vector
void SemiExp::clear()
{
	_tokens.clear();
}

Token SemiExp::show(bool showNewLines)
{
  std::cout << "\n  ";
  for (auto token : _tokens)
    if(token != "\n")
      std::cout << token << " ";
	else 
		if (showNewLines)
			std::cout << token << " ";
  std::cout << "\n";
  return "";
}

#ifdef TEST_SEMIEXP
int main()
{
  Toker toker;
  std::string fileSpec = "../SemiExp/SemiExpTestFile.txt";
  std::fstream in(fileSpec);
  if (!in.good())
  {
    std::cout << "\n  can't open file " << fileSpec << "\n\n";
    return 1;
  }
  toker.attach(&in);
  toker.setSpecialSingleChars("/");
  toker.setSpecialCharPairs("/=");
  toker.setCollectComments(true);  //testing comments
  SemiExp semi(&toker);
  while(semi.get())
  {
    std::cout << "\n  -- semiExpression --";
    semi.show();
  }
  /*
     May have collected tokens, but reached end of stream
     before finding SemiExp terminator.
   */
  if (semi.length() > 0)  
  {
    std::cout << "\n  -- semiExpression --";
    semi.show();
    std::cout << "\n\n";
  }
  //testing the remaining functions of ITok Interface
  semi.clear();
  semi.push_back("\n");
  semi.push_back("\n");
  semi.push_back("\n");
  semi.push_back("\n");
  semi.push_back("Testing");
  semi.push_back("ITok");
  semi.push_back("Interface");
  semi.push_back("for");
  semi.push_back("Tokenizer");
  int index = 2;
  std::cout << "\n Testing ITokCollection Interface functions : \n";
  std::cout << "\n - Token Vector contains -> ";
  semi.show();
  semi.trimFront();
  std::cout << "\n - Token Vector contains after trim contains -> ";
  semi.show();
  semi.toLower();
  std::cout << "\n - Token Vector contains after toLower contains -> ";
  semi.show();
  semi.remove(2);
  std::cout << "\n - Token Vector contains after removing from position 2 contains -> ";
  semi.show();
  semi.remove("itok");  //uses find function
  std::cout << "\n - Token Vector contains after removing string contains -> ";
  semi.show();
  std::cout << "\n - Token at position "<<index<<" in Token Vector is -> "<<semi[index];
  std::cout << "\n\n";
  return 0;
}
#endif