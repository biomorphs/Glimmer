#pragma once
#include <nlohmann/json.hpp>

#define SDE_SERIALISED_CLASS()	\
	template<class Archive> void Serialise(Archive&);

//#define SDE_SERIALISE_BEGIN(classname)	\
//	template<class Archive> void classname::Serialise(Archive& a)	\
//	{	
//
//#define SDE_SERIALISE_PROPERTY(name,p)	\
//		a.archive(name,p);
//
//#define SDE_SERIALISE_END()	\
//	}

#define SDE_SERIALISE_BEGIN(classname)	\
	void className::Serialise(Archive& a)

struct SerialiseToJson;
struct SerialiseFromJson;

#include "serialisation.inl"