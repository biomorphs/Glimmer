#pragma once

struct SerialiseToJson
{
	template<class T> void archive(const char* name, T& v)
	{
		m_json[name] = v;
	}

	template<class T> void archive(const char* name, T* v)
	{
		m_json[name] = v;
	}

	template<> void archive(const char* name, const char* v)
	{
		m_json[name] = (std::string)v;
	}

	template<class T> void archive(T& v)
	{
		m_json.push_back(v);
	}

	template<class T> void archive(const char* name, std::vector<T>& v)
	{
		std::vector<nlohmann::json::basic_json> listJson;
		for (auto& it : v)
		{
			SerialiseToJson itJson;
			it->Serialise(itJson);
			listJson.push_back(std::move(itJson.m_json));
		}
		m_json[name] = std::move(listJson);
	}
	nlohmann::json::basic_json m_json;
};

struct SerialiseFromJson
{
	template<class T> void archive(const char* name, T& v)
	{
		v = m_json[name];
	}
	nlohmann::json::basic_json m_json;
};