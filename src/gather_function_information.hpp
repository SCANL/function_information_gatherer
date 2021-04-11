/**
 * @file FindExpansionsPolicy.hpp
 *
 * @copyright Copyright (C) 2013-2014 SDML (www.srcML.org)
 *
 * The srcML Toolkit is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * The srcML Toolkit is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with the srcML Toolkit; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#ifndef INCLUDED_EXPANSION_POLICY_HPP
#define INCLUDED_EXPANSION_POLICY_HPP

#include <srcSAXEventDispatcher.hpp>
#include <srcSAXHandler.hpp>
#include <exception>
#include <regex>
#include <map>
#include <locale>
#include <cctype>
#include <list>
#include <map>
#include <set>
#include <fstream>
#include <algorithm>
#include <cmath>
#include <stdlib.h>
#include <FunctionSignaturePolicy.hpp>
#include <DeclTypePolicy.hpp>
#include <ParamTypePolicy.hpp>
#include <FunctionTextPolicy.hpp>
using std::regex;
using std::regex_replace;

std::regex Regexcmp("[\\W_0-9]+");
std::regex Raft("([a-zA-Z]+)([0-9]+)");
std::regex Rbef("([0-9]+)([a-zA-Z]+)");
std::regex Rcamel("([a-z])([A-Z])");
std::regex Rcamelthree("([A-Z])([A-Z][a-z])");

std::string ConservativeSplit(std::string rawString){
  std::string processedString = std::regex_replace(rawString, Regexcmp, "$1 $2");
  processedString = std::regex_replace(processedString, Raft, "$1 $2");
  processedString = std::regex_replace(processedString, Rbef, "$1 $2");
  processedString = std::regex_replace(processedString, Rcamel, "$1 $2");
  processedString = std::regex_replace(processedString, Rcamelthree, "$1 $2");
  std::transform(processedString.begin(), processedString.end(), processedString.begin(), ::tolower);
  return processedString;
}

class FunctionMetadata{
  public:
    std::string name;
    std::string splitName;
    std::string returnType;
    std::vector<DeclData> parameters;
    std::vector<DeclData> declarations;
    int lineCount;
    void clear(){
        name = "";
        returnType = "";
        parameters.clear();
        declarations.clear();
        lineCount = 1;
    }
    friend std::ostream& operator<<(std::ostream& os, const FunctionMetadata& fm){
        os << fm.name << "," << fm.splitName << "," << fm.returnType << "," << fm.parameters.size() << "," << fm.declarations.size() << "," << fm.lineCount <<std::endl;
        return os;
    }
};
class GatherFunctionInformationPolicy : public srcSAXEventDispatch::EventListener, public srcSAXEventDispatch::PolicyDispatcher, public srcSAXEventDispatch::PolicyListener {
    public:
        ~GatherFunctionInformationPolicy(){}
        GatherFunctionInformationPolicy(): srcSAXEventDispatch::PolicyDispatcher({}){
            InitializeEventHandlers();
            declPolicy.AddListener(this);
            functionPolicy.AddListener(this);
            function_text_policy.AddListener(this);
            
        }
        void Notify(const PolicyDispatcher * policy, const srcSAXEventDispatch::srcSAXEventContext & ctx) override {
            using namespace srcSAXEventDispatch;
            if((typeid(DeclTypePolicy) == typeid(*policy)) && ctx.IsOpen(ParserState::function)){
                decldata = *policy->Data<DeclData>();
                metadata.declarations.push_back(decldata);
            }else if((typeid(FunctionSignaturePolicy) == typeid(*policy)) && ctx.IsOpen(ParserState::function)){
                functiondata = *policy->Data<SignatureData>();
                metadata.parameters = functiondata.parameters;
                metadata.name = functiondata.name;
                metadata.returnType = functiondata.returnType;
                metadata.splitName = ConservativeSplit(functiondata.name);
            }
            else if(typeid(FunctionTextPolicy) == typeid(*policy)){
                functiontextdata = *policy->Data<FunctionText>();
                metadata.lineCount = functiontextdata.lineCount;
            }
            
        }
        void NotifyWrite(const PolicyDispatcher * policy, srcSAXEventDispatch::srcSAXEventContext & ctx) override {} //doesn't write
    protected:
        void * DataInner() const override {}
    private:
        FunctionMetadata metadata;

        FunctionTextPolicy function_text_policy;
        FunctionText functiontextdata;

        FunctionSignaturePolicy functionPolicy;
        SignatureData functiondata;

        DeclTypePolicy declPolicy;
        DeclData decldata;

        ParamTypePolicy paramPolicy;
        DeclData paramdata;
        void InitializeEventHandlers(){
            using namespace srcSAXEventDispatch;
            closeEventMap[ParserState::tokenstring] = [this](srcSAXEventContext& ctx){};
            openEventMap[ParserState::function] = [this](srcSAXEventContext& ctx) {
                ctx.dispatcher->AddListenerDispatch(&functionPolicy);
                ctx.dispatcher->AddListenerDispatch(&function_text_policy);
            };
            openEventMap[ParserState::functionblock] = [this](srcSAXEventContext& ctx) {
                ctx.dispatcher->RemoveListenerDispatch(&functionPolicy);
            };
            openEventMap[ParserState::parameterlist] = [this](srcSAXEventContext& ctx) {
                //ctx.dispatcher->AddListenerDispatch(&paramPolicy);
            };
            closeEventMap[ParserState::parameterlist] = [this](srcSAXEventContext& ctx) {
                //ctx.dispatcher->RemoveListenerDispatch(&paramPolicy);
            };
            openEventMap[ParserState::declstmt] = [this](srcSAXEventContext& ctx) {
                ctx.dispatcher->AddListenerDispatch(&declPolicy);
            };
            closeEventMap[ParserState::function] = [this](srcSAXEventContext& ctx){
                std::cout<<metadata<<std::endl;
                metadata.clear();
                ctx.dispatcher->RemoveListenerDispatch(&function_text_policy);
            };
            closeEventMap[ParserState::declstmt] = [this](srcSAXEventContext& ctx) {
                ctx.dispatcher->RemoveListenerDispatch(&declPolicy);
            };
            closeEventMap[ParserState::archive] = [this](srcSAXEventContext& ctx) {
            };
        }
};
#endif