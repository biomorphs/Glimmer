#pragma once
#include <nlohmann/json.hpp>

namespace SDE
{
	// Specify read/write from json. I don't like this name
	enum class Seraliser
	{
		Reader,
		Writer
	};
}

// add this to the public interface of your class
#define SDE_SERIALISED_CLASS()	\
	void Serialise(nlohmann::json::basic_json& json, SDE::Seraliser op);

// add this to your cpp, generates serialise function + registers factories
#define SDE_SERIALISE_BEGIN(classname)	\
	SDE::Internal::ObjectFactory<classname>::Register s_registerFactory_(#classname,[](){ return new classname; });	\
	void classname::Serialise(nlohmann::json::basic_json& json, SDE::Seraliser op)	\
	{	\
		const char* c_myClassName = #classname;

// add this to your cpp, generates serialise function + registers factories
#define SDE_SERIALISE_BEGIN_WITH_PARENT(classname,parent)	\
	SDE::Internal::ObjectFactory<parent>::Register s_registerFactory_parent_(#classname,[]() { return reinterpret_cast<parent*>(new classname); });	\
	SDE_SERIALISE_BEGIN(classname)	\
		parent::Serialise(json,op);	\

// properties (can handle almost anything)
#define SDE_SERIALISE_PROPERTY(name,p)	\
		if(op == SDE::Seraliser::Writer) {	\
			SDE::ToJson(name, p, json);	\
		}	\
		else    \
		{	\
			SDE::FromJson(name, p, json);	\
		}

// add this at the end!
// our last act is to record the class name. we do it last to avoid 
// having parents overwrite it

#define SDE_SERIALISE_END()	\
		json["SDE_ClassName"] = c_myClassName;	\
	}

#include "serialisation.inl"