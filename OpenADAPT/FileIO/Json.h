#ifndef ADAPT_FILEIO_JSON_H
#define ADAPT_FILEIO_JSON_H

#if __has_include(<rapidjson/document.h>)
#define ADAPT_JSON_HAS_RAPIDJSON 1
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
#elif __has_include("rapidjson/document.h")
#define ADAPT_JSON_HAS_RAPIDJSON 1
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
#else
#define ADAPT_JSON_HAS_RAPIDJSON 0
#endif

#if ADAPT_JSON_HAS_RAPIDJSON == 1

#include <algorithm>
#include <charconv>
#include <concepts>
#include <format>
#include <limits>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include <OpenADAPT/Common/Common.h>
#include <OpenADAPT/Container/Tree.h>
#include <OpenADAPT/Utility/Exception.h>

namespace adapt::json
{

class Schema;

struct InferOptions
{
	bool flatten_objects = true;
	std::string nested_name_separator = "_";//メンバであれば.が一般的だが、ADAPT_GET_PLACEHOLDERSと相性が悪いので、_をデフォルトにする。
	std::string array_object_name_separator = "_";
};

struct ImportOptions
{
	bool allow_string_to_number = true;
	bool allow_number_to_string = false;
	bool skip_missing_fields = false;
	bool skip_null_fields = false;
};

namespace detail
{

struct FieldBinding
{
	std::vector<std::string> path;//JSON内のフィールドへのパス。基本的にはキーのみだが、objectの入れ子になっている場合にその入れ子構造を保管している。
	bool optional = false;
};

struct LayerSpec
{
	std::vector<std::pair<std::string, FieldType>> fields;
	std::vector<FieldBinding> bindings;
	std::vector<std::string> prefix;//このレイヤーの要素がJSON内のどこにあるかを示すパス。下層要素は必ず親要素の中で1個以上のkeyを経由しないと到達できないので、その情報を保存している。
	//LayerType parent = -2_layer;
};

inline std::vector<std::string> SplitPath(std::string_view path)
{
	std::vector<std::string> tokens;
	std::string current;
	for (size_t i = 0; i < path.size(); ++i)
	{
		char c = path[i];
		if (c == '/' || c == '.')
		{
			if (!current.empty())
			{
				tokens.push_back(current);
				current.clear();
			}
			continue;
		}
		if (c == '[' && i + 1 < path.size() && path[i + 1] == ']')
		{
			if (!current.empty())
			{
				tokens.push_back(current);
				current.clear();
			}
			tokens.emplace_back("");
			i += 1;
			continue;
		}
		current += c;
	}
	if (!current.empty())
		tokens.push_back(current);
	return tokens;
}

/*inline std::string JoinPath(const std::string& lhs, std::string_view rhs, std::string_view sep)
{
	if (lhs.empty()) return std::string(rhs);
	if (rhs.empty()) return lhs;
	return lhs + std::string(sep) + std::string(rhs);
}*/
inline std::string JoinPath(const std::vector<std::string>& segments, const std::string& sep)
{
	std::string result;
	for (const auto& seg : segments)
	{
		if (!result.empty()) result += (sep + seg);
	}
	return result;
}

inline const rapidjson::Value* FindMember(const rapidjson::Value& value, std::string_view key)
{
	if (!value.IsObject())
		return nullptr;
	for (auto it = value.MemberBegin(); it != value.MemberEnd(); ++it)
	{
		std::string_view name(it->name.GetString(), it->name.GetStringLength());
		if (name == key)
			return &it->value;
	}
	return nullptr;
}

inline std::vector<const rapidjson::Value*> ResolveMany(const rapidjson::Value& root, const std::vector<std::string>& path)
{
	std::vector<const rapidjson::Value*> current{ &root };
	//const auto tokens = SplitPath(path);
	if (path.empty())
		return current;

	for (const auto& token : path)
	{
		std::vector<const rapidjson::Value*> next;
		if (token.empty())
		{
			for (const auto* value : current)
			{
				if (!value->IsArray())
					throw InvalidArg("JSON path expects an array for wildcard.");
				for (const auto& child : value->GetArray())
					next.push_back(&child);
			}
		}
		else
		{
			for (const auto* value : current)
			{
				if (!value->IsObject())
					throw InvalidArg(std::format("JSON path segment '{}' expects an object.", token));
				if (const auto* child = FindMember(*value, token))
					next.push_back(child);
			}
		}
		current = std::move(next);
	}

	return current;
}

inline const rapidjson::Value* ResolveOne(const rapidjson::Value& root, const std::vector<std::string>& path)
{
	auto values = ResolveMany(root, path);
	if (values.size() != 1)
		throw InvalidArg(std::format("JSON path did not resolve to a single value."));
	return values.front();
}

inline std::string LastPathSegment(std::string_view path)
{
	const auto tokens = SplitPath(path);
	for (auto it = tokens.rbegin(); it != tokens.rend(); ++it)
		if (*it != "*")
			return *it;
	return {};
}

inline FieldType InferFieldType(const rapidjson::Value& value)
{
	if (value.IsNull()) return FieldType::Emp;
	if (value.IsBool()) return FieldType::I08;
	if (value.IsInt64())
	{
		return FieldType::I64;
	}
	if (value.IsUint64())
	{
		return FieldType::I64;
	}
	if (value.IsNumber()) return FieldType::F64;
	if (value.IsString()) return FieldType::Str;
	return FieldType::Emp;
}

inline FieldType MergeFieldType(FieldType a, FieldType b)
{
	if (a == FieldType::Emp) return b;
	if (b == FieldType::Emp) return a;
	if (a == b) return a;
	if ((DFieldInfo::IsInt(a) && DFieldInfo::IsInt(b)) || (DFieldInfo::IsFlt(a) && DFieldInfo::IsFlt(b)))
		return (DFieldInfo::IsFlt(a) || DFieldInfo::IsFlt(b)) ? FieldType::F64 : FieldType::I64;
	if ((DFieldInfo::IsInt(a) && DFieldInfo::IsFlt(b)) || (DFieldInfo::IsFlt(a) && DFieldInfo::IsInt(b)))
		return FieldType::F64;
	if (DFieldInfo::IsStr(a) || DFieldInfo::IsStr(b))
		return FieldType::Str;
	return FieldType::Str;
}

template <class Fields, class Bindings>
inline void EnsureField(Fields& fields, Bindings& bindings, const std::vector<std::string>& path, FieldType type)
{
	auto it = std::find_if(fields.begin(), fields.end(), [&](const auto& pair) { return pair.first == path.back(); });
	if (it == fields.end())
	{
		fields.emplace_back(JoinPath(path, "_"), type);
		bindings.push_back(FieldBinding{ path, false });
	}
	else
	{
		it->second = MergeFieldType(it->second, type);
	}
}

template <class Writer, class ElementRefT>
inline void WriteFieldValue(Writer& writer, const DTree& tree, const ElementRefT& ref, LayerType layer, uint16_t index, FieldType type)
{
	auto ph = tree.GetPlaceholder(layer, index);
	auto field = ref[ph];
	switch (type)
	{
	case FieldType::I08: writer.Int(field.template to<int8_t>()); break;
	case FieldType::I16: writer.Int(field.template to<int16_t>()); break;
	case FieldType::I32: writer.Int(field.template to<int32_t>()); break;
	case FieldType::I64: writer.Int64(field.template to<int64_t>()); break;
	case FieldType::F32: writer.Double(field.template to<float>()); break;
	case FieldType::F64: writer.Double(field.template to<double>()); break;
	case FieldType::Str:
		{
			auto s = field.template to<std::string>();
			writer.String(s.c_str(), static_cast<rapidjson::SizeType>(s.size()));
			break;
		}
	default:
		writer.Null();
		break;
	}
}

} // namespace detail

class Schema
{
public:
	Schema& SetTopLayer(const std::vector<std::pair<std::string, FieldType>>& mems)
	{
		m_top_fields = mems;
		m_top_bindings.assign(m_top_fields.size(), detail::FieldBinding{});
		for (size_t i = 0; i < m_top_fields.size(); ++i)
			m_top_bindings[i].path = detail::SplitPath(m_top_fields[i].first);
		return *this;
	}

	Schema& AddLayer(std::vector<std::pair<std::string, FieldType>> mems)
	{
		m_layers.emplace_back();
		auto& layer = m_layers.back();
		layer.fields = std::move(mems);
		layer.bindings.assign(layer.fields.size(), detail::FieldBinding{});
		for (size_t i = 0; i < layer.fields.size(); ++i)
			layer.bindings[i].path = detail::SplitPath(layer.fields[i].first);
//		layer.parent = m_layers.size() == 1 ? -1_layer : LayerType(m_layers.size()) - 2_layer;
		return *this;
	}

	Schema& SetLayer(LayerType layer, const std::vector<std::pair<std::string, FieldType>>& mems)
	{
		if (layer < 0) return SetTopLayer(mems);
		const size_t idx = static_cast<size_t>(layer);
		if (m_layers.size() <= idx) m_layers.resize(idx + 1);
		m_layers[idx].fields = mems;
		m_layers[idx].bindings.assign(m_layers[idx].fields.size(), detail::FieldBinding{});
		for (size_t i = 0; i < m_layers[idx].fields.size(); ++i)
			m_layers[idx].bindings[i].path = detail::SplitPath(m_layers[idx].fields[i].first);
		//m_layers[idx].parent = layer - 1_layer;
		return *this;
	}

	Schema& BindLayer(LayerType layer, std::string path)
	{
		if (layer < 0)
		{
			assert(layer == -1_layer);
			assert(path.empty());
			return *this;
		}
		const size_t idx = static_cast<size_t>(layer);
		if (m_layers.size() <= idx) m_layers.resize(idx + 1);
		m_layers[idx].prefix = detail::SplitPath(path);
		return *this;
	}

	Schema& BindParent(LayerType layer, LayerType parent)
	{
		if (layer < 0) return *this;
		const size_t idx = static_cast<size_t>(layer);
		if (m_layers.size() <= idx)
			m_layers.resize(idx + 1);
		//m_layers[idx].parent = parent;
		return *this;
	}

	Schema& BindField(LayerType layer, std::string_view field, std::string path, bool optional = false)
	{
		auto bind_impl = [&](auto& fields, auto& bindings, std::string_view layer_name)
		{
			auto it = std::find_if(fields.begin(), fields.end(), [&](const auto& pair) { return pair.first == field; });
			if (it == fields.end())
				throw InvalidArg(std::format("No field '{}' in {}.", field, layer_name));
			const size_t index = static_cast<size_t>(it - fields.begin());
			bindings[index].path = detail::SplitPath(path);
			bindings[index].optional = optional;
		};

		if (layer < 0)
		{
			bind_impl(m_top_fields, m_top_bindings, "top layer");
			return *this;
		}

		const size_t idx = static_cast<size_t>(layer);
		if (m_layers.size() <= idx)
			m_layers.resize(idx + 1);
		bind_impl(m_layers[idx].fields, m_layers[idx].bindings, "layer");
		return *this;
	}

	void VerifyStructure() const
	{
		if (m_top_bindings.size() != m_top_fields.size())
			throw InvalidArg("Top layer binding mismatch.");
		for (const auto& layer : m_layers)
			if (layer.bindings.size() != layer.fields.size())
				throw InvalidArg("Layer binding mismatch.");
	}

	const std::vector<std::pair<std::string, FieldType>>& TopFields() const { return m_top_fields; }
	const std::vector<detail::FieldBinding>& TopBindings() const { return m_top_bindings; }
	const std::vector<detail::LayerSpec>& Layers() const { return m_layers; }
	const detail::LayerSpec& GetLayer(LayerType layer) const { return m_layers[static_cast<size_t>(layer)]; }
	LayerType MaxLayer() const { return m_layers.empty() ? -1_layer : LayerType(m_layers.size()) - 1_layer; }

	template <class Container>
	void Apply(Container& container) const
	{
		container.SetTopLayer(m_top_fields);
		for (const auto& layer : m_layers)
			container.AddLayer(layer.fields);
		container.VerifyStructure();
	}


public:
	std::vector<std::pair<std::string, FieldType>> m_top_fields;
	std::vector<detail::FieldBinding> m_top_bindings;
	std::vector<detail::LayerSpec> m_layers;
};

namespace detail
{

// Jsonオブジェクトのフィールドをスキーマに追加する。
// 下層要素となるarray<object>が空だった場合は推測不可能であるためfalseを返す。
// そうでない場合はtrueを返す。
inline bool InferObjectFields(const rapidjson::Value& obj, const InferOptions& opt, Schema& schema, LayerType layer, const std::vector<std::string>& prefix)
{
	if (!obj.IsObject())
		return true;
	auto& fields = [&]() -> auto& { return layer < 0 ? schema.m_top_fields : schema.m_layers[layer].fields; }();
	auto& bindings = [&]() -> auto& { return layer < 0 ? schema.m_top_bindings : schema.m_layers[layer].bindings; }();

	bool result = true;
	for (auto it = obj.MemberBegin(); it != obj.MemberEnd(); ++it)
	{
		const std::string key(it->name.GetString(), it->name.GetStringLength());
		const auto& value = it->value;
		std::vector<std::string> nested_prefix = prefix;
		nested_prefix.push_back(key);
		if (value.IsObject() && opt.flatten_objects)
		{
			InferObjectFields(value, opt, schema, layer, nested_prefix);
		}
		else if (value.IsArray())
		{
			if (value.Empty())
			{
				result = false;
			}
			else if (value.Begin()->IsObject())
			{
				LayerType next_layer = layer + 1_layer;
				if (next_layer < 0)
					next_layer = 0_layer;
				if (schema.m_layers.size() <= size_t(next_layer))
					schema.m_layers.resize(size_t(next_layer) + 1);
				auto& layer_spec = schema.m_layers[size_t(next_layer)];
				if (layer_spec.prefix.empty())
					layer_spec.prefix.push_back(key);
				//if (layer_spec.parent == -2_layer)
				//	layer_spec.parent = layer;
				nested_prefix.emplace_back();
				for (auto it = value.Begin(); it != value.End(); ++it)
				{
					//もし戻り値がfalseの場合、next_layerの中に何らかのarrayが含まれており、かつ配列が空だったことを意味する。
					//この場合、この配列が下層要素になりうるのか、それともarray<scalar>なのかを識別できない。
					//よって、要素を１つ進めて再度構造推定を行う。
					if (InferObjectFields(*it, opt, schema, next_layer, nested_prefix)) break;
				}
			}
			else
			{
				//detail::EnsureField(fields, bindings, prefix.empty() ? key : JoinPath(prefix, key, opt.nested_name_separator), FieldType::Str);
				adapt::PrintWarning("scalar array is not supported. Field '{}' will be ignored.", prefix.empty() ? key : JoinPath(nested_prefix, opt.nested_name_separator));
			}
		}
		else
		{
			detail::EnsureField(fields, bindings, nested_prefix, InferFieldType(value));
		}
	}
	return result;
}

template <class T>
T ConvertJsonScalar(const rapidjson::Value& value, const ImportOptions& opt)
{
	if constexpr (std::same_as<T, std::string>)
	{
		if (value.IsString())
			return std::string(value.GetString(), value.GetStringLength());
		if (opt.allow_number_to_string)
		{
			if (value.IsBool()) return value.GetBool() ? "true" : "false";
			if (value.IsInt64()) return std::to_string(value.GetInt64());
			if (value.IsUint64()) return std::to_string(value.GetUint64());
			if (value.IsNumber()) return std::to_string(value.GetDouble());
		}
		throw MismatchType("JSON value cannot be converted to string.");
	}
	else if constexpr (std::integral<T> && !std::same_as<T, bool>)
	{
		if (value.IsBool()) return static_cast<T>(value.GetBool());
		if (value.IsInt64()) return static_cast<T>(value.GetInt64());
		if (value.IsUint64()) return static_cast<T>(value.GetUint64());
		if (value.IsNumber()) return static_cast<T>(value.GetDouble());
		if (value.IsString() && opt.allow_string_to_number)
		{
			T out{};
			std::string_view s(value.GetString(), value.GetStringLength());
			auto [ptr, ec] = std::from_chars(s.data(), s.data() + s.size(), out, 10);
			if (ec == std::errc{} && ptr == s.data() + s.size())
				return out;
		}
		throw MismatchType("JSON value cannot be converted to an integer.");
	}
	else if constexpr (std::floating_point<T>)
	{
		if (value.IsBool()) return static_cast<T>(value.GetBool() ? 1 : 0);
		if (value.IsInt64()) return static_cast<T>(value.GetInt64());
		if (value.IsUint64()) return static_cast<T>(value.GetUint64());
		if (value.IsNumber()) return static_cast<T>(value.GetDouble());
		if (value.IsString() && opt.allow_string_to_number)
			return static_cast<T>(std::stod(std::string(value.GetString(), value.GetStringLength())));
		throw MismatchType("JSON value cannot be converted to a floating-point number.");
	}
	else if constexpr (std::same_as<T, bool>)
	{
		if (value.IsBool()) return value.GetBool();
		if (value.IsInt64()) return value.GetInt64() != 0;
		if (value.IsUint64()) return value.GetUint64() != 0;
		if (value.IsString() && opt.allow_string_to_number)
		{
			std::string_view s(value.GetString(), value.GetStringLength());
			return s == "true" || s == "1";
		}
		throw MismatchType("JSON value cannot be converted to bool.");
	}
	else
	{
		throw MismatchType("Unsupported target type.");
	}
}

template <class Container, class ElementRefT>
void PopulateElement(Container& container, ElementRefT& ref, const Schema& schema, LayerType layer, const rapidjson::Value& node, const ImportOptions& opt)
{
	const auto& fields = layer < 0 ? schema.TopFields() : schema.GetLayer(layer).fields;
	const auto& bindings = layer < 0 ? schema.TopBindings() : schema.GetLayer(layer).bindings;

	for (size_t i = 0; i < fields.size(); ++i)
	{
		const auto& binding = bindings[i];
		const rapidjson::Value* src = nullptr;
		try
		{
			src = ResolveOne(node, binding.path);
		}
		catch (...)
		{
			if (binding.optional || opt.skip_missing_fields)
				continue;
			throw;
		}

		if (src == nullptr)
		{
			if (binding.optional || opt.skip_missing_fields)
				continue;
			throw InvalidArg(std::format("Missing JSON field '{}'.", binding.path));
		}
		if (src->IsNull() && opt.skip_null_fields)
			continue;

		auto ph = container.GetPlaceholder(layer, static_cast<uint16_t>(i));
#define CODE(ttype, ftype, vtype) \
case FieldType::ttype: ref[ph].template as<FieldType::ttype>() = ConvertJsonScalar<vtype>(*src, opt); break;
		switch (fields[i].second)
		{
		ADAPT_FOR_EACH_TYPE(CODE)
		default: throw MismatchType("Unsupported field type in JSON import.");
		}
	}

	if (layer >= schema.MaxLayer()) return;

	LayerType child_layer = layer + 1_layer;
	const LayerSpec& child_spec = schema.GetLayer(child_layer);
	std::vector<const rapidjson::Value*> children;
	try
	{
		children = ResolveMany(node, child_spec.prefix);
	}
	catch (...)
	{
		if (opt.skip_missing_fields) return;
		throw;
	}

	if (children.empty()) return;

	ref.Reserve(static_cast<BindexType>(children.size()));
	for (const auto* child : children)
	{
		ref.PushDefaultElement();
		auto child_ref = ref.Back();
		PopulateElement(container, child_ref, schema, child_layer, *child, opt);
	}
}

/*template <class Writer, class ElementRefT>
void WriteElement(Writer& writer, const DTree& tree, const ElementRefT& ref, const Schema& schema, LayerType layer)
{
	writer.StartObject();
	const auto& fields = layer < 0 ? schema.TopFields() : schema.GetLayer(layer).fields;
	for (size_t i = 0; i < fields.size(); ++i)
	{
		writer.Key(fields[i].first.c_str(), static_cast<rapidjson::SizeType>(fields[i].first.size()));
		WriteFieldValue(writer, tree, ref, layer, static_cast<uint16_t>(i), fields[i].second);
	}

	if (layer < schema.MaxLayer())
	{
		const LayerType child_layer = layer + 1_layer;
		const auto& child_spec = schema.GetLayer(child_layer);
		const auto child_name = LastPathSegment(child_spec.prefix);
		if (!child_name.empty())
		{
			writer.Key(child_name.c_str(), static_cast<rapidjson::SizeType>(child_name.size()));
			writer.StartArray();
			for (const auto& child_ref : ref.GetLowerElements())
				WriteElement(writer, tree, child_ref, schema, child_layer);
			writer.EndArray();
		}
	}
	writer.EndObject();
}*/

} // namespace detail

inline Schema InferSchema(const rapidjson::Value& root, const InferOptions& opt = {})
{
	if (!root.IsObject())
		throw InvalidArg("InferSchema expects a JSON object root.");
	Schema schema;
	detail::InferObjectFields(root, opt, schema, -1_layer, {});
	schema.VerifyStructure();
	return schema;
}

inline Schema InferSchema(const rapidjson::Document& doc, const InferOptions& opt = {})
{
	return InferSchema(static_cast<const rapidjson::Value&>(doc), opt);
}

inline DTree ImportDTree(const rapidjson::Value& root, const Schema& schema, const ImportOptions& opt = {})
{
	DTree tree;
	/*schema.Apply(tree);
	const auto* top = detail::ResolveOne(root, schema.TopSource());
	DTree::ElementRef top_ref = tree.GetTopElement();
	detail::PopulateElement(tree, top_ref, schema, -1_layer, *top, opt);*/
	return tree;
}

inline DTree ImportDTree(const rapidjson::Document& doc, const Schema& schema, const ImportOptions& opt = {})
{
	return ImportDTree(static_cast<const rapidjson::Value&>(doc), schema, opt);
}

inline DTree ImportDTree(std::string_view json_text, const Schema& schema, const ImportOptions& opt = {})
{
	rapidjson::Document doc;
	doc.Parse(json_text.data(), json_text.size());
	if (doc.HasParseError())
		throw InvalidArg(std::format("Failed to parse JSON text at offset {}.", doc.GetErrorOffset()));
	return ImportDTree(doc, schema, opt);
}

/*inline std::string ExportJson(const DTree& tree, const Schema& schema)
{
	rapidjson::StringBuffer buffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	detail::WriteElement(writer, tree, tree.GetTopElement(), schema, -1_layer);
	return { buffer.GetString(), buffer.GetSize() };
}*/

} // namespace adapt::json

#endif

#endif
