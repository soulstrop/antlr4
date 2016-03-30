/*
 * [The "BSD license"]
 *  Copyright (c) 2016 Mike Lischke
 *  Copyright (c) 2013 Terence Parr
 *  Copyright (c) 2013 Dan McLaughlin
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  3. The name of the author may not be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 *  IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 *  OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 *  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 *  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 *  NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 *  THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#pragma once

#include "RecognitionException.h"
#include "CPPUtils.h"
#include "Strings.h"
#include "ProxyErrorListener.h"
#include "Token.h"
#include "CPPUtils.h"

namespace org {
namespace antlr {
namespace v4 {
namespace runtime {

  template<typename ATNInterpreter>
  std::map<std::vector<std::wstring>, std::map<std::wstring, int>> Recognizer<ATNInterpreter>::_tokenTypeMapCache;

  template<typename ATNInterpreter>
  std::map<std::vector<std::wstring>, std::map<std::wstring, int>> Recognizer<ATNInterpreter>::_ruleIndexMapCache;

  template<typename ATNInterpreter>
  std::map<std::wstring, int> Recognizer<ATNInterpreter>::getTokenTypeMap() {
    const std::vector<std::wstring>& tokenNames = getTokenNames();
    if (tokenNames.empty()) {
      throw L"The current recognizer does not provide a list of token names.";
    }

    std::lock_guard<std::mutex> lck(mtx);
    std::map<std::wstring, int> result;
    auto iterator = _tokenTypeMapCache.find(tokenNames);
    if (iterator != _tokenTypeMapCache.end()) {
      result = iterator->second;
    } else {
      result = antlrcpp::toMap(tokenNames);
      result[L"EOF"] = Token::_EOF;
      _tokenTypeMapCache[tokenNames] = result;
    }

    return result;
  }

  template<typename ATNInterpreter>
  std::map<std::wstring, int> Recognizer<ATNInterpreter>::getRuleIndexMap() {
    const std::vector<std::wstring>& ruleNames = getRuleNames();
    if (ruleNames.empty()) {
      throw L"The current recognizer does not provide a list of rule names.";
    }

    std::lock_guard<std::mutex> lck(mtx);
    std::map<std::wstring, int> result;
    auto iterator = _ruleIndexMapCache.find(ruleNames);
    if (iterator != _ruleIndexMapCache.end()) {
      result = iterator->second;
    } else {
      result = antlrcpp::toMap(ruleNames);
      _ruleIndexMapCache[ruleNames] = result;
    }
    return result;
  }

  template<typename ATNInterpreter>
  int Recognizer<ATNInterpreter>::getTokenType(const std::wstring &tokenName) {

    const std::map<std::wstring, int> &map = getTokenTypeMap();
    int ttype = map.at(tokenName);

    if (ttype != Token::INVALID_TYPE) {
      return ttype;
    }
    return Token::INVALID_TYPE;
  }

  template<typename ATNInterpreter>
  std::wstring Recognizer<ATNInterpreter>::getErrorHeader(RecognitionException *e) {
    // We're having issues with cross header dependencies, these two classes will need to be
    // rewritten to remove that.
    int line = e->getOffendingToken()->getLine();
    int charPositionInLine = e->getOffendingToken()->getCharPositionInLine();
    return std::wstring(L"line ") + std::to_wstring(line) + std::wstring(L":") + std::to_wstring(charPositionInLine);

  }

  template<typename ATNInterpreter>
  std::wstring Recognizer<ATNInterpreter>::getTokenErrorDisplay(Token *t) {
    if (t == nullptr) {
      return L"<no token>";
    }
    std::wstring s = t->getText();
    if (s == L"") {
      if (t->getType() == Token::_EOF) {
        s = L"<EOF>";
      } else {
        s = std::wstring(L"<") + std::to_wstring(t->getType()) + std::wstring(L">");
      }
    }

    antlrcpp::replaceAll(s, L"\n", L"\\n");
    antlrcpp::replaceAll(s, L"\r",L"\\r");
    antlrcpp::replaceAll(s, L"\t", L"\\t");

    return std::wstring(L"'") + s + std::wstring(L"'");
  }

  template<typename ATNInterpreter>
  void Recognizer<ATNInterpreter>::addErrorListener(ANTLRErrorListener *listener) {
    if (listener == nullptr) {
      throw L"listener cannot be null.";
    }

    _listeners.insert(_listeners.end(), listener);
  }

  template<typename ATNInterpreter>
  void Recognizer<ATNInterpreter>::removeErrorListener(ANTLRErrorListener *listener) {
    //_listeners.remove(listener); does this work the same way?
    std::vector<ANTLRErrorListener*>::iterator it;
    it = std::find(_listeners.begin(), _listeners.end(), listener);
    _listeners.erase(it);
  }

  template<typename ATNInterpreter>
  void Recognizer<ATNInterpreter>::removeErrorListeners() {
    _listeners.clear();
  }

  template<typename ATNInterpreter>
  ANTLRErrorListener *Recognizer<ATNInterpreter>::getErrorListenerDispatch() {
    return (ANTLRErrorListener *)new ProxyErrorListener(getErrorListeners());
  }

  template<typename ATNInterpreter>
  bool Recognizer<ATNInterpreter>::sempred(RuleContext *_localctx, int ruleIndex, int actionIndex) {
    return true;
  }


  template<typename ATNInterpreter>
  bool Recognizer<ATNInterpreter>::precpred(RuleContext *localctx, int precedence) {
    return true;
  }


  template<typename ATNInterpreter>
  void Recognizer<ATNInterpreter>::action(RuleContext *_localctx, int ruleIndex, int actionIndex) {
  }


  template<typename ATNInterpreter>
  int Recognizer<ATNInterpreter>::getState() {
    return _stateNumber;
  }

  template<typename ATNInterpreter>
  void Recognizer<ATNInterpreter>::setState(int atnState) {
    _stateNumber = atnState;
    //		if ( traceATNStates ) _ctx.trace(atnState);
  }

  template<typename ATNInterpreter>
  void Recognizer<ATNInterpreter>::InitializeInstanceFields() {
    _stateNumber = -1;
    _interpreter = nullptr;
    _listeners = std::vector<ANTLRErrorListener*>();
  }

  template<typename ATNInterpreter>
  Recognizer<ATNInterpreter>::Recognizer() {
    InitializeInstanceFields();
  }

} // namespace runtime
} // namespace v4
} // namespace antlr
} // namespace org