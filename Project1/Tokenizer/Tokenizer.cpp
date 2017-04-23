///////////////////////////////////////////////////////////////////////
// Tokenizer.cpp - read words from a std::stream                     //
// ver 3.3                                                           //
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
* This package provides declares the ConsumeState class.
* Toker reads words from a std::stream, throws away whitespace and comments
* and returns words from the stream in the order encountered.  Quoted
* strings and certain punctuators and newlines are returned as single tokens.
*
* Object of ConsumeState class suffle between different states as per the input 
* from the stream and accordingly change the behaviour. For example, if the stream
* is encountering whitespaces, the state of the ConsumeState object will be changed 
* to EatWhiteSpace and the corresponding behaviour will change. 
*
* Build Process:
* --------------
* Required Files: Tokenizer.h, Tokenizer.cpp
* Build Command: devenv Tokenizer.sln /rebuild debug
*
* Maintenance History:
* --------------------
* ver 3.3 : 08 Feb 2016
* - added functions for adding special characters and option to include
*   comments in token. 
* - Included states to handle Quoted Strings, Special Characters
* - Added conditions to handle C comments properly
* ver 3.2 : 08 Feb 2016
* - provided the basic helper code to start with the project.
*/

#include "Tokenizer.h"
#include <iostream>
#include <cctype>
#include <string>
#include <map>

namespace Scanner
{
  class ConsumeState
  {
  public:
    ConsumeState();
    ConsumeState(const ConsumeState&) = delete;
    ConsumeState& operator=(const ConsumeState&) = delete;
    virtual ~ConsumeState();
    void attach(std::istream* pIn) { _pIn = pIn; }
    virtual void eatChars() = 0;
    void consumeChars() {
      _pState->eatChars();
      _pState = nextState();
    }
    bool canRead() { return _pIn->good(); }
    std::string getTok() { return token; }
    bool hasTok() { return token.size() > 0; }
    ConsumeState* nextState();
	bool setSpecialSingleChars(std::string ssc);
	bool setSpecialCharPairs(std::string scp);
	void setCollectComments(bool value);
	void setConstructDestructFlag();
  protected:
	static bool constructorFlag;
	static bool destructorFlag;
	static std::map<char, int> oneCharTokens;
	static std::map<std::string, int> twoCharTokens;
    static std::string token;
    static std::istream* _pIn;
    static int prevChar;
    static int currChar;
	static bool collectComments;
    static ConsumeState* _pState;
    static ConsumeState* _pEatCppComment;
    static ConsumeState* _pEatCComment;
    static ConsumeState* _pEatWhitespace;
    static ConsumeState* _pEatPunctuator;
    static ConsumeState* _pEatAlphanum;
    static ConsumeState* _pEatNewline;
	static ConsumeState* _pEatQuotedString;
	static ConsumeState* _pEatSpecialChar;
	static ConsumeState* _pEatSpecialNewline;
  };
}

using namespace Scanner;

std::map<char, int> ConsumeState::oneCharTokens = { {'<', NULL}, {'>', NULL}, {'[', NULL}, {']', NULL},
														    {'(', NULL}, {')', NULL}, {'{', NULL}, {'}', NULL},
															{':', NULL}, {'=', NULL}, {'+', NULL}, {'-', NULL},
															{'*', NULL}, {'*', NULL}, {'\n', NULL} };

std::map<std::string, int> ConsumeState::twoCharTokens = { {"<<", NULL}, {">>", NULL}, {"::", NULL}, {"++", NULL},
																   {"--", NULL}, {"==", NULL}, {"+=", NULL}, {"-=", NULL},
																   {"*=", NULL}, };
bool ConsumeState :: constructorFlag = true;
bool ConsumeState :: destructorFlag = true;
std::string ConsumeState::token;
std::istream* ConsumeState::_pIn = nullptr;
int ConsumeState::prevChar;
int ConsumeState::currChar;
bool ConsumeState::collectComments = false;
ConsumeState* ConsumeState::_pState = nullptr;
ConsumeState* ConsumeState::_pEatCppComment = nullptr;
ConsumeState* ConsumeState::_pEatCComment = nullptr;
ConsumeState* ConsumeState::_pEatWhitespace = nullptr;
ConsumeState* ConsumeState::_pEatPunctuator = nullptr;
ConsumeState* ConsumeState::_pEatAlphanum = nullptr;
ConsumeState* ConsumeState::_pEatQuotedString = nullptr;
ConsumeState* ConsumeState::_pEatSpecialChar = nullptr;
ConsumeState* ConsumeState::_pEatSpecialNewline = nullptr;
ConsumeState* ConsumeState::_pEatNewline;

void testLog(const std::string& msg);

//--adds single special characters inside the default's list
bool ConsumeState :: setSpecialSingleChars(std::string ssc)
{
		if (oneCharTokens.count(ssc[0]) > 0)   //checks if the string is already present.
			return false;
		oneCharTokens[ssc[0]] = 1;
		return true;
}

void ConsumeState :: setConstructDestructFlag()
{
	constructorFlag = true;
	destructorFlag = true;
}

//--adds special characters pairs inside the default's list
bool ConsumeState :: setSpecialCharPairs(std::string scp)
{
	if (twoCharTokens.count(scp) > 0)
		return false;
	twoCharTokens[scp] = 1;
	return true;
}

//--sets whether to include comments in tokens
void ConsumeState :: setCollectComments(bool value)
{
	if (value)
		collectComments = true;
	else
		collectComments = false;
}

//--sets the state of the object depending upon the current character of input stream
ConsumeState* ConsumeState::nextState()
{
  if (!(_pIn->good()))
    return nullptr;

  int chNext = _pIn->peek();

  if (chNext == EOF)
    _pIn->clear();   // _pIn->good() will return false.  clear() restores state to good

  if (currChar == '\\' && chNext == 'n')
	  return _pEatSpecialNewline;
  if (currChar == '\"' || currChar == '\'')
	  return _pEatQuotedString;

  if (std::isspace(currChar) && currChar != '\n')
    return _pEatWhitespace;

  if (currChar == '/' && chNext == '/')
    return _pEatCppComment;

  if (currChar == '/' && chNext == '*')
    return _pEatCComment;

  if (currChar == '\n')
    return _pEatNewline;

  if (oneCharTokens.count(currChar) > 0)
	  return _pEatSpecialChar;

  if (std::isalnum(currChar))
    return _pEatAlphanum;

  if (ispunct(currChar))
    return _pEatPunctuator;

  if (!_pIn->good())
    return _pEatWhitespace;
  throw(std::logic_error("invalid type"));
}

class EatSpecialNewLine : public ConsumeState
{
public:
	virtual void eatChars()
	{
		token.clear();
		//std::cout << "\n  eating alphanum";
		token += currChar;
		token += _pIn->get();
		currChar = _pIn->get();
		/*if (!_pIn->good())  // end of stream
			return;
		currChar = _pIn->get();
		if (_pIn->good())
		{
			token += currChar;
			currChar = _pIn->good();
		}
		*/
	}
};

//--here eatChars() behaviour changes to tokenize the special characters
//  present in twoCharTokens and oneCharTokens
class EatSpecialChar : public ConsumeState
{
	public:
		virtual void eatChars()
		{
			token.clear();
			std::string comparatorToken;
			token += currChar;
			comparatorToken += currChar;
			currChar = _pIn->get();
			if (!_pIn->good())
				return;
			comparatorToken += currChar;
			if (twoCharTokens.count(comparatorToken) > 0)
			{
				token += currChar;
				currChar = _pIn->get();
			}
		}
};

//--Here eatChars() behaviour changes to handle the quoted strings
class EatQuotedString : public ConsumeState
{
public:
	virtual void eatChars()
	{
		bool quotesOpen = true, inBetween = true, backSlashIsComing = false, backSlashCame = true;
		token.clear();
		token += currChar;
		do
		{
			if (!_pIn->good())  // end of stream
				return;
			currChar = _pIn->get();
			if (currChar == '\\' && _pIn->peek() == '\\')  //if its a escape sequence for backslash
			{
				if (backSlashCame)
				{
					backSlashIsComing = true;
					backSlashCame = false;
					token += currChar;
				}
				else
				{
					if (backSlashIsComing)
					{
						backSlashCame = true;
						token += currChar;
						backSlashIsComing = false;
					}
				}
			}
			else if ((currChar == '\\' && (_pIn->peek() == '"' || _pIn->peek() == '\'')))
				checkForBackSlash(backSlashCame, inBetween);    //1. Refactored to reduce lines
			else
				checkForClosingQuote(quotesOpen, inBetween);  //2. Refactored to reduce lines
		} while (quotesOpen);
		token += currChar;
		currChar = _pIn->get();
		_pState = nextState();
	}
	void checkForClosingQuote(bool& quotesOpen, bool& inBetween)  //checks if the quotes is getting closed
	{
		if (!(currChar == '"' || currChar == '\''))
			token += currChar;
		else
		{
			if (inBetween)
				quotesOpen = false;
			else
			{
				token += currChar;
				inBetween = true;
			}
		}
	}
	void checkForBackSlash(bool& backSlashCame, bool &inBetween)  //checks if it is a valid backslash
	{
		if (backSlashCame)
		{
			inBetween = false;
			token += currChar;
		}
		else
		{
			token += currChar;
			inBetween = true;
		}
	}
};

//--Here eatChars() behaviour changes to omit all the whitespaces 
//  encountered in the input stream
class EatWhitespace : public ConsumeState
{
public:
  virtual void eatChars()
  {
    token.clear();
    //std::cout << "\n  eating whitespace";
    do {
      if (!_pIn->good())  // end of stream
        return;
      currChar = _pIn->get();
    } while (std::isspace(currChar) && currChar != '\n');
  }
};

//--Here eatChars() behaviour changes to handle cpp style comments
//  it tokenizes the comment if the collectComment flag is set to 
//  true else it omits.
class EatCppComment : public ConsumeState
{
public:
  virtual void eatChars()
  {
    token.clear();
    do {
		if (collectComments)
			token += currChar;
		if (!_pIn->good())  // end of stream
		    return;
		currChar = _pIn->get();
	} while (currChar != '\n');
  }
};

//--Here eatChars() behaviour changes to handle the C style comments
//  performs the same behaviour for tokenizing as in cpp style above
class EatCComment : public ConsumeState
{
public:
  virtual void eatChars()
  {
    token.clear();
	do {
		if (!_pIn->good())  // end of stream
			return;
		if (collectComments)
			token += currChar;
      currChar = _pIn->get();
    } while (!(currChar == '*' && _pIn->peek() == '/'));
	if (collectComments)
	{
		token += currChar;
		token += _pIn->get();
	}
    _pIn->get();
    currChar = _pIn->get();
  }
};

//--Here eatChars() behaviour changes to hande the punctuators
class EatPunctuator : public ConsumeState
{
public:
  virtual void eatChars()
  {
    token.clear();
    //std::cout << "\n  eating punctuator";
    do {
      token += currChar;
      if (!_pIn->good())  // end of stream
        return;
      currChar = _pIn->get();
    } while (ispunct(currChar) && (!((oneCharTokens.count(currChar) > 0) || (currChar == '\\' && _pIn->peek() == 'n'))));
  }
};

//--Here eatChars() behaviour changes to tokenize the AlphaNumerics
class EatAlphanum : public ConsumeState
{
public:
  virtual void eatChars()
  {
    token.clear();
    //std::cout << "\n  eating alphanum";
    do {
      token += currChar;
      if (!_pIn->good())  // end of stream
        return;
      currChar = _pIn->get();
    } while (isalnum(currChar));
  }
};

//--Here eatChars() behaviour changes to tokenize an encountered newline
class EatNewline : public ConsumeState
{
public:
  virtual void eatChars()
  {
    token.clear();
    //std::cout << "\n  eating alphanum";
    token += currChar;
    if (!_pIn->good())  // end of stream
      return;
    currChar = _pIn->get();
  }
};



ConsumeState::ConsumeState()
{
  if (constructorFlag)
  {
    constructorFlag = false;
    _pEatAlphanum = new EatAlphanum();
    _pEatCComment = new EatCComment();
    _pEatCppComment = new EatCppComment();
    _pEatPunctuator = new EatPunctuator();
    _pEatWhitespace = new EatWhitespace();
    _pEatNewline = new EatNewline();
	_pEatQuotedString = new EatQuotedString();
	_pEatSpecialChar = new EatSpecialChar();
	_pEatSpecialNewline = new EatSpecialNewLine();
    _pState = _pEatWhitespace;
  }
}

ConsumeState::~ConsumeState()
{
  static bool first = true;
  if (destructorFlag)
  {
    destructorFlag = false;
    delete _pEatAlphanum;
    delete _pEatCComment;
    delete _pEatCppComment;
    delete _pEatPunctuator;
    delete _pEatWhitespace;
    delete _pEatNewline;
	delete _pEatQuotedString;
	delete _pEatSpecialChar;
	delete _pEatSpecialNewline;
  }
}

Toker::Toker() : pConsumer(new EatWhitespace()) 
{
	pConsumer->setConstructDestructFlag();
}

Toker::~Toker() { delete pConsumer; }

bool Toker::attach(std::istream* pIn)
{
  if (pIn != nullptr && pIn->good())
  {
    pConsumer->attach(pIn);
    return true;
  }
  return false;
}

std::string Toker::getTok()
{
  while(true) 
  {
    if (!pConsumer->canRead())
      return "";
    pConsumer->consumeChars();
    if (pConsumer->hasTok())
      break;
  }
  return pConsumer->getTok();
}


bool Toker::canRead() { return pConsumer->canRead(); }

//--adds special characters to the default maintained list 'oneCharTokens'
bool Toker :: setSpecialSingleChars(std::string ssc)
{
	std::cout << "\n\n ::Special Single Character \'" << ssc << "\' added to the defaults.";
	return pConsumer->setSpecialSingleChars(ssc);
}

//--adds special characters to the default maintained list 'twoCharTokens'
bool Toker :: setSpecialCharPairs(std::string scp)
{
	std::cout << "\n\n ::Special Character Pair \"" << scp << "\" added to the defaults.";
	return pConsumer->setSpecialCharPairs(scp);
}

//--sets the collectComments flag to true or false indicating whether 
//  or not to tokenize the comments 
void Toker::setCollectComments(bool value)
{
	pConsumer->setCollectComments(value);	
}

void Toker :: setConstructorDestructorflag()
{
	pConsumer->setConstructDestructFlag();
}

void testLog(const std::string& msg)
{
#ifdef TEST_LOG
  std::cout << "\n  " << msg;
#endif
}

//----< test stub >--------------------------------------------------

#ifdef TEST_TOKENIZER

#include <fstream>

int main()
{
  //std::string fileSpec = "../Tokenizer/Tokenizer.cpp";
  //std::string fileSpec = "../Tokenizer/Tokenizer.h";
	
  std::string fileSpec = "../Tokenizer/TokenizerTestFile.txt";

  std::ifstream in(fileSpec);
  if (!in.good())
  {
    std::cout << "\n  can't open " << fileSpec << "\n\n";
    return 1;
  }
  Toker toker;
  toker.attach(&in);
  toker.setSpecialSingleChars("/");  
  toker.setSpecialCharPairs("/=");
  toker.setCollectComments(true);  //testing comments

  do
  {
    std::string tok = toker.getTok();
    if (tok == "\n")
      tok = "newline";
    std::cout << "\n -- " << tok;
  } while (in.good());

  std::cout << "\n\n";
  return 0;
}
#endif
