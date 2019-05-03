#pragma once

namespace SDE
{
	enum class Seraliser
	{
		Reader,
		Writer
	};

	template <typename T>		// SFINAE trick to detect Serialise member fn
	class HasSerialiser
	{
		typedef char one;
		typedef long two;

		template <typename C> static one test(decltype(&C::Serialise));
		template <typename C> static two test(...);

	public:
		enum { value = sizeof(test<T>(0)) == sizeof(char) };
	};

	template<class T>
	void ToJson(T& v, nlohmann::json::basic_json& json)
	{
		if constexpr (HasSerialiser<T>::value)	// I'm in love
		{
			v.Serialise(json, SDE::Seraliser::Writer);
		}
		else
		{
			json = v;
		}
	}

	template<class T>
	void ToJson(T* v, nlohmann::json::basic_json& json)
	{
		if constexpr (HasSerialiser<T>::value)
		{
			v.Serialise(json, SDE::Seraliser::Writer);
		}
		else
		{
			json = v;
		}
	}

	template<class T>
	void ToJson(const char* name, T& v, nlohmann::json::basic_json& json)
	{
		json[name] = v;
	}

	template<class T>
	void ToJson(const char* name, T* v, nlohmann::json::basic_json& json)
	{
		json[name] = v;
	}

	template<class T>
	void ToJson(const char* name, std::vector<T>& v, nlohmann::json::basic_json& json)
	{
		std::vector<nlohmann::json::basic_json> listJson;
		for (auto& it : v)
		{
			nlohmann::json::basic_json itJson;
			ToJson(*it, itJson);
			listJson.push_back(std::move(itJson));
		}
		json[name] = std::move(listJson);
	}
}