#pragma once
#include <nlohmann/json.hpp>

// add this to the public interface of your class
#define SDE_SERIALISED_CLASS()	\
	void Serialise(nlohmann::json::basic_json& json, SDE::Seraliser op);

// add this to your cpp
#define SDE_SERIALISE_BEGIN(classname)	\
	void classname::Serialise(nlohmann::json::basic_json& json, SDE::Seraliser op)	\
	{

// calls parent serialise fn
#define SDE_SERIALISE_PARENT(parentClass)	\
		parentClass::Serialise(json,op);

// properties (can handle almost anything)
#define SDE_SERIALISE_PROPERTY(name,p)	\
		if(op == SDE::Seraliser::Writer) {	\
			SDE::ToJson(name, p, json);	\
		}

// add this at the end!
#define SDE_SERIALISE_END()	\
	}

#include "serialisation.inl"