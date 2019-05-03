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

// add this to your cpp
#define SDE_SERIALISE_BEGIN(classname)	\
	void classname::Serialise(nlohmann::json::basic_json& json, SDE::Seraliser op)	\
	{	\
		const char* c_myClassName = #classname;

// calls parent serialise fn
#define SDE_SERIALISE_PARENT(parentClass)	\
		parentClass::Serialise(json,op);

// properties (can handle almost anything)
#define SDE_SERIALISE_PROPERTY(name,p)	\
		if(op == SDE::Seraliser::Writer) {	\
			SDE::ToJson(name, p, json);	\
		}

// add this at the end!
// our last act is to record the class name. we do it last to avoid 
// having parents overwrite it

#define SDE_SERIALISE_END()	\
		json["SDE_ClassName"] = c_myClassName;	\
	}

#include "serialisation.inl"