#ifndef ADAPT_FILEIO_JSON_H
#define ADAPT_FILEIO_JSON_H

#ifdef ADAPT_USE_RAPIDJSON
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/ostreamwrapper.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/writer.h>

#include <algorithm>
#include <charconv>
#include <concepts>
#include <format>
#include <limits>
#include <string>
#include <string_view>
#include <unordered_map>
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

/*inline std::vector<std::string> SplitPath(std::string_view path)
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
}*/

/*inline std::string JoinPath(const std::string& lhs, std::string_view rhs, std::string_view sep)
{
	if (lhs.empty()) return std::string(rhs);
	if (rhs.empty()) return lhs;
	return lhs + std::string(sep) + std::string(rhs);
}*/
inline std::string JoinPath(const std::vector<std::string>& path, const std::string& sep)
{
	std::string result;
	for (const auto& seg : path)
	{
		if (!result.empty())
		{
			result += (sep + seg);
		}
		else result += seg;
	}
	return result;
}
inline std::string JoinPath(const std::vector<std::string>& path, const std::vector<std::string>& keys, const std::string& sep)
{
	std::string result;
	for (const auto& seg : path)
	{
		if (!result.empty())
		{
			result += (sep + seg);
		}
		else result += seg;
	}
	for (const auto& seg : keys)
	{
		if (!result.empty()) result += sep;
		result += seg;
	}
	return result;
}

struct FieldBinding
{
	//JSON内のフィールドへのパス。オブジェクトのネストがある場合はその経路も含めて保存する。
	//ユーザーからの指定がない場合、LayerSpec::prefixとpathとを結合して
	//フィールド名を生成する。
	//[ "employeeinfo", "name" ]みたいな。
	std::vector<std::string> path;
	bool optional = false;
};

struct LayerSpec
{
	std::vector<std::pair<std::string, FieldType>> fields;//コンテナへのAddLayer等を呼び出すための引数。
	std::vector<FieldBinding> bindings;
	std::unordered_map<std::string, size_t> field_indices;//フィールド名からfieldsのインデックスを引くためのマップ。
	//このレイヤーの要素がJSON内のどこにあるかを示すパス。
	//この要素までの経路を、階層を意味する配列やオブジェクトのネスト経路を含めて保存する。
	//例えばある企業の部署、課、従業員という構造があり、課から従業員までの経路を表す場合、
	//[ "employee" ]
	//のような形で保管される。オブジェクトのネストがある場合はその経路も含めて。
	std::vector<std::string> route;
	//LayerType parent = -2_layer;
};

inline void RebuildFieldIndices(LayerSpec& layerspec)
{
	layerspec.field_indices.clear();
	layerspec.field_indices.reserve(layerspec.fields.size());
	for (size_t i = 0; i < layerspec.fields.size(); ++i)
		layerspec.field_indices.emplace(layerspec.fields[i].first, i);
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

/*inline std::string LastPathSegment(std::string_view path)
{
	const auto tokens = SplitPath(path);
	for (auto it = tokens.rbegin(); it != tokens.rend(); ++it)
		if (*it != "*")
			return *it;
	return {};
}*/

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
// Jsonオブジェクトのフィールドをスキーマに追加する。
// 下層要素となるarray<object>が空だった場合は推測不可能であるためfalseを返す。
// そうでない場合はtrueを返す。
inline static void EnsureField(detail::LayerSpec& layerspec,
							   const std::vector<std::string>& keys, FieldType type)
{
	//std::views::concatはC++26から……
	std::string field_name = detail::JoinPath(layerspec.route, keys, "_");
	if (auto it = layerspec.field_indices.find(field_name); it == layerspec.field_indices.end())
	{
		layerspec.field_indices.emplace(field_name, layerspec.fields.size());
		layerspec.fields.emplace_back(field_name, type);
		layerspec.bindings.push_back(detail::FieldBinding{ keys, false });
	}
	else
	{
		layerspec.fields[it->second].second = detail::MergeFieldType(layerspec.fields[it->second].second, type);
	}
}

}

class Schema
{
public:
	inline void SetTopLayer(const std::vector<std::pair<std::string, FieldType>>& mems)
	{
		m_top_layer.fields = mems;
		m_top_layer.bindings.assign(m_top_layer.fields.size(), detail::FieldBinding{});
		for (size_t i = 0; i < m_top_layer.fields.size(); ++i)
			m_top_layer.bindings[i].path.push_back(m_top_layer.fields[i].first);
	}

	inline void AddLayer(const std::vector<std::pair<std::string, FieldType>>& mems)
	{
		m_layers.emplace_back();
		SetLayer(LayerType(m_layers.size()) - 1_layer, mems);
	}

	inline void SetLayer(LayerType layer, const std::vector<std::pair<std::string, FieldType>>& mems)
	{
		if (layer < 0) return SetTopLayer(mems);
		const size_t idx = static_cast<size_t>(layer);
		if (m_layers.size() <= idx) m_layers.resize(idx + 1);
		m_layers[idx].fields = mems;
		m_layers[idx].bindings.assign(m_layers[idx].fields.size(), detail::FieldBinding{});
		for (size_t i = 0; i < m_layers[idx].fields.size(); ++i)
			m_layers[idx].bindings[i].path.push_back(m_layers[idx].fields[i].first);
		//m_layers[idx].parent = layer - 1_layer;
	}

	// あるlayerまでのobjectのネスト経路を指定する。LayerSpec::routeに保存される。
	// 例えば、ある企業の部署、課、従業員という構造があり、課から従業員への経路を指定する場合、
	// [ "employee" ]
	// のような形で指定する。オブジェクトのネストがある場合はその経路も含めて。
	// デフォルトでは[ "layer0" ]、[ "layer1" ]などになる。
	inline void BindLayer(LayerType layer, const std::vector<std::string>& route)
	{
		if (layer < 0)
		{
			assert(layer == -1_layer);
			m_top_layer.route = route;
			return;
		}
		size_t layer_ = (size_t)layer;
		if (m_layers.size() <= layer_) m_layers.resize(layer_ + 1);
		m_layers[layer_].route = route;
	}
private:
	// BindFieldの実装。
	// ユーザーが何らかの指定をしている場合、VerifyStructureからは書き換えたくないので、overwrite==falseで呼ぶ。
	// ユーザーが明示的にBindFieldを呼んでいる場合は、overwrite==trueとする。
	// BindLayerならrouteがemptyかどうかでユーザー指定の有無を判別できるが、
	// BindFieldはunordered_mapからの検索を挟んでおり、検索の二度手間を防ぐためにoverwriteの有無で判別する。
	inline void BindField_impl(LayerType layer, std::string_view fieldname, const std::vector<std::string>& path, bool optional = false, bool overwrite = true)
	{
		auto bind_impl = [&](detail::LayerSpec& layerspec)
		{
			auto it = layerspec.field_indices.find(std::string(fieldname));
			if (it == layerspec.field_indices.end())
				throw InvalidArg(std::format("No field '{}' in layer {}.", fieldname, layer));
			const size_t index = it->second;

			// empty()==trueの場合はそもそも指定されていないので、問答無用で代入する。
			// empty()==falseの場合は何らかの理由で既に指定されているので、overwrite==trueの場合のみ上書きする。
			if (overwrite || layerspec.bindings[index].path.empty())
				layerspec.bindings[index].path = path;
			if (overwrite)
				layerspec.bindings[index].optional = optional;
		};

		if (layer < 0)
		{
			bind_impl(m_top_layer);
			return;
		}

		size_t layer_ = (size_t)layer;
		if (m_layers.size() <= layer_) m_layers.resize(layer_ + 1);
		bind_impl(m_layers[layer_]);
	}
public:
	// あるlayerのfieldnameに対して、JSON内のパスを指定する。LayerSpec::bindings[...].pathに保存される。
	// デフォルトでは[ fieldname ]である。
	inline void BindField(LayerType layer, std::string_view fieldname, const std::vector<std::string>& path, bool optional = false)
	{
		BindField_impl(layer, fieldname, path, optional, true);
	}

	inline void VerifyStructure()
	{
		auto bind = [this](detail::LayerSpec& layerspec, LayerType l)
		{
			RebuildFieldIndices(layerspec);
			for (const auto& [name, type] : layerspec.fields)
			{
				if (type == FieldType::Emp)
					throw InvalidArg(std::format("Top layer field '{}' has empty type.", name));
				// BindFieldについては、ユーザーが明示していない限り書き換える。
				BindField_impl(l, name, { name }, false, false);
			}
		};
		// top layerのBindLayerは呼ぶ意味がない。
		// デフォルトの空配列のままでよいし、ユーザーが指定しているなら書き換える必要はない。
		bind(m_top_layer, -1_layer);
		for (auto&&[i, layer] : views::Enumerate(m_layers))
		{
			if (layer.route.empty())
				layer.route = { std::format("layer{}", i) };
			bind(layer, (LayerType)i);
		}
	}

	//const std::vector<std::pair<std::string, FieldType>>& TopFields() const { return m_top_layer.fields; }
	//const std::vector<detail::FieldBinding>& TopBindings() const { return m_top_layer.bindings; }
	//const std::vector<detail::LayerSpec>& Layers() const { return m_layers; }
	inline const detail::LayerSpec& GetLayerSpec(LayerType layer) const
	{
		assert(layer >= -1);
		return layer == -1 ? m_top_layer : m_layers[static_cast<size_t>(layer)];
	}
	inline LayerType MaxLayer() const { return m_layers.empty() ? -1_layer : LayerType(m_layers.size()) - 1_layer; }

	template <class Container>
	void Apply(Container& container) const
	{
		container.SetTopLayer(m_top_layer.fields);
		for (const auto& layer : m_layers)
			container.AddLayer(layer.fields);
		container.VerifyStructure();
	}

	inline static bool InferObjectFields(const rapidjson::Value& obj, const InferOptions& opt, Schema& schema,
									 LayerType layer, std::vector<std::string>& route)
	{
		if (!obj.IsObject())
			return true;
		auto& layerspec = [&]() -> auto& { return layer < 0 ? schema.m_top_layer : schema.m_layers[layer]; }();

		bool result = true;
		for (auto it = obj.MemberBegin(); it != obj.MemberEnd(); ++it)
		{
			std::string_view key(it->name.GetString(), it->name.GetStringLength());
			const auto& value = it->value;
			route.emplace_back(key);
			if (value.IsObject() && opt.flatten_objects)
			{
				InferObjectFields(value, opt, schema, layer, route);
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
					auto& next_layer_spec = schema.m_layers[size_t(next_layer)];
					next_layer_spec.route = route;
					std::vector<std::string> child_route;
					for (auto it = value.Begin(); it != value.End(); ++it)
					{
						//もし戻り値がfalseの場合、next_layerの中に何らかのarrayが含まれており、かつ配列が空だったことを意味する。
						//この場合、この配列が下層要素になりうるのか、それともarray<scalar>なのかを識別できない。
						//よって、要素を１つ進めて再度構造推定を行う。
						if (InferObjectFields(*it, opt, schema, next_layer, child_route)) break;
					}
				}
				else
				{
					//detail::EnsureField(fields, bindings, prefix.empty() ? key : JoinPath(prefix, key, opt.nested_name_separator), FieldType::Str);
					adapt::PrintWarning("scalar array is not supported. Field '{}' will be ignored.",
										detail::JoinPath(route, opt.nested_name_separator));
				}
			}
			else
			{
				//オブジェクトでも配列でもないので、フィールドとして扱う。
				detail::EnsureField(layerspec, route, detail::InferFieldType(value));
			}
			route.pop_back();
		}
		return result;
	}

private:
	detail::LayerSpec m_top_layer;
	std::vector<detail::LayerSpec> m_layers;
};

namespace detail
{


/*inline std::vector<const rapidjson::Value*> ResolveMany(const rapidjson::Value& root, const std::vector<std::string>& path)
{
	std::vector<const rapidjson::Value*> current{ &root };
	//const auto tokens = SplitPath(path);
	if (path.empty())
		return current;

	for (const auto& token : path)
	{
		std::vector<const rapidjson::Value*> next;
		if (token == "[]")
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
}*/

inline const rapidjson::Value& GetChild(const rapidjson::Value& parent, const std::vector<std::string>& path)
{
	const rapidjson::Value* current = &parent;
	for (const std::string& key : path)
	{
		if (!current->IsObject())
			throw InvalidArg("not an object");
		current = &(*current)[key.c_str()];
	}
	return *current;
}

inline const rapidjson::Value& GetLowerLayer(const rapidjson::Value& parent, const std::vector<std::string>& path)
{
	const rapidjson::Value* current = &parent;
	for (const std::string& key : path)
	{
		if (!current->IsObject()) throw InvalidArg("not an object");
		current = &(*current)[key.c_str()];
	}
	if (!current->IsArray()) throw InvalidArg("not an array");
	return *current;
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
		{
			std::string_view s(value.GetString(), value.GetStringLength());
			double out{};
			auto [ptr, ec] = std::from_chars(s.data(), s.data() + s.size(), out);
			if (ec == std::errc{} && ptr == s.data() + s.size())
				return static_cast<T>(out);
		}
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

template <any_container Container, class ElementRefT>
void PopulateElement(Container& container, ElementRefT& ref, const Schema& schema, LayerType layer,
					 const rapidjson::Value& node, const ImportOptions& opt)
{
	const LayerSpec& layerspec = schema.GetLayerSpec(layer);

	for (size_t i = 0; i < layerspec.fields.size(); ++i)
	{
		const auto& binding = layerspec.bindings[i];
		const rapidjson::Value* src = nullptr;
		try
		{
			src = &GetChild(node, binding.path);
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
			throw InvalidArg(std::format("Missing JSON field '{}'.", layerspec.fields[i].first));
		}
		if (src->IsNull() && opt.skip_null_fields)
			continue;

		auto ph = container.GetPlaceholder(layer, static_cast<uint16_t>(i));
#define CODE(ttype, ftype, vtype) \
case FieldType::ttype: ref[ph].template as<FieldType::ttype>() = ConvertJsonScalar<vtype>(*src, opt); break;
		switch (layerspec.fields[i].second)
		{
		ADAPT_FOR_EACH_TYPE(CODE)
		default: throw MismatchType("Unsupported field type in JSON import.");
		}
		#undef CODE
	}

	if (layer >= schema.MaxLayer()) return;

	LayerType child_layer = layer + 1_layer;
	const LayerSpec& child_spec = schema.GetLayerSpec(child_layer);
	const rapidjson::Value* children = nullptr;
	try
	{
		children = &GetLowerLayer(node, child_spec.route);
	}
	catch (...)
	{
		if (opt.skip_missing_fields) return;
		throw;
	}

	if (children->Empty()) return;

	ref.Reserve(static_cast<BindexType>(children->Size()));
	for (const auto& child : children->GetArray())
	{
		ref.PushDefaultElement();
		auto child_ref = ref.Back();
		PopulateElement(container, child_ref, schema, child_layer, child, opt);
	}
}

template <class Allocator, class ElementRefT>
rapidjson::Value MakeFieldValue(Allocator& alloc, const DTree& tree, const ElementRefT& ref, LayerType layer, uint16_t index, FieldType type)
{
	auto ph = tree.GetPlaceholder(layer, index);
	auto field = ref[ph];
	switch (type)
	{
	case FieldType::I08: return rapidjson::Value(field.template as<int8_t>());
	case FieldType::I16: return rapidjson::Value(field.template as<int16_t>());
	case FieldType::I32: return rapidjson::Value(field.template as<int32_t>());
	case FieldType::I64: return rapidjson::Value(field.template as<int64_t>());
	case FieldType::F32: return rapidjson::Value(field.template as<float>());
	case FieldType::F64: return rapidjson::Value(field.template as<double>());
	case FieldType::Str:
	{
		auto s = field.template as<std::string>();
		return rapidjson::Value(s.c_str(), static_cast<rapidjson::SizeType>(s.size()), alloc);
	}
	default:
		return rapidjson::Value(rapidjson::kNullType);
	}
}

template <class Iterator, class Allocator>
void AddValueAtPath(rapidjson::Value& object, Iterator path_begin, Iterator path_end, rapidjson::Value&& value, Allocator& alloc)
{
	if (!object.IsObject())
		throw InvalidArg("JSON export target must be an object.");
	if (path_begin == path_end)
		throw InvalidArg("JSON export path must not be empty.");

	rapidjson::Value* current = &object;
	Iterator leaf_it = path_end;
	--leaf_it;
	for (Iterator it = path_begin; it != leaf_it; ++it)
	{
		const auto& key = *it;

		if (!current->HasMember(key.c_str()))
		{
			rapidjson::Value name(key.c_str(), static_cast<rapidjson::SizeType>(key.size()), alloc);
			rapidjson::Value child(rapidjson::kObjectType);
			current->AddMember(name, child, alloc);
		}

		auto member = current->FindMember(key.c_str());
		if (member == current->MemberEnd())
			throw InvalidArg("Failed to create nested JSON object.");
		if (!member->value.IsObject())
			throw InvalidArg("JSON export path collides with a non-object value.");
		current = &member->value;
	}

	const auto& leaf = *leaf_it;

	rapidjson::Value name(leaf.c_str(), static_cast<rapidjson::SizeType>(leaf.size()), alloc);
	if (auto member = current->FindMember(leaf.c_str()); member != current->MemberEnd())
	{
		member->value = std::move(value);
	}
	else
	{
		current->AddMember(name, value, alloc);
	}
}
template <any_container Container, class ElementRefT>
rapidjson::Value ExportElement(const Container& t, const ElementRefT& ref, const Schema& schema, LayerType layer, rapidjson::Document::AllocatorType& alloc)
{
	rapidjson::Value object(rapidjson::kObjectType);
	const auto& layerspec = schema.GetLayerSpec(layer);
	for (size_t i = 0; i < layerspec.fields.size(); ++i)
	{
		AddValueAtPath(object,
			layerspec.bindings[i].path.begin(),
			layerspec.bindings[i].path.end(),
			MakeFieldValue(alloc, t, ref, layer, static_cast<uint16_t>(i), layerspec.fields[i].second),
			alloc);
	}

	if (layer < schema.MaxLayer())
	{
		const LayerType child_layer = layer + 1_layer;
		const auto& child_spec = schema.GetLayerSpec(child_layer);
		rapidjson::Value children(rapidjson::kArrayType);
		auto lower_elements = ref.GetLowerElements();
		for (const auto& child_ref : lower_elements)
			children.PushBack(ExportElement(t, child_ref, schema, child_layer, alloc), alloc);

		AddValueAtPath(object, child_spec.route.begin(), child_spec.route.end(), std::move(children), alloc);
	}
	return object;
}

} // namespace detail

inline Schema InferSchema(const rapidjson::Value& root, const InferOptions& opt = {})
{
	if (!root.IsObject())
		throw InvalidArg("InferSchema expects a JSON object root.");
	Schema schema;
	std::vector<std::string> route;
	Schema::InferObjectFields(root, opt, schema, -1_layer, route);
	schema.VerifyStructure();
	return schema;
}
inline Schema InferSchema(const rapidjson::Document& doc, const InferOptions& opt = {})
{
	return InferSchema(static_cast<const rapidjson::Value&>(doc), opt);
}

inline DTree ImportJson(const rapidjson::Value& root, const Schema& schema, const ImportOptions& opt = {})
{
	DTree tree;
	schema.Apply(tree);
	DTree::ElementRef top_ref = tree.GetTopElement();
	detail::PopulateElement(tree, top_ref, schema, -1_layer, root, opt);
	return tree;
}
inline DTree ImportJson(const rapidjson::Document& doc, const Schema& schema, const ImportOptions& opt = {})
{
	return ImportJson(static_cast<const rapidjson::Value&>(doc), schema, opt);
}
inline DTree ImportJson(const rapidjson::Document& doc)
{
	Schema schema = InferSchema(doc);
	return ImportJson(doc, schema);
}
inline DTree ImportJson(const rapidjson::Value& root)
{
	Schema schema = InferSchema(root);
	return ImportJson(root, schema);
}

template <any_container Container>
Schema InferSchema(const Container& container, const InferOptions& opt = {})
{
	Schema schema;
	schema.SetTopLayer(container.GetFieldInfosIn(-1_layer));
	LayerType max_layer = container.GetMaxLayer();
	for (LayerType layer = 0_layer; layer <= max_layer; ++layer)
	{
		schema.AddLayer(container.GetFieldInfosIn(layer));
	}
	schema.VerifyStructure();
	return schema;
}

inline rapidjson::Document ExportJson(const DTree& tree, const Schema& schema)
{
	rapidjson::Document doc;
	auto& alloc = doc.GetAllocator();
	doc.CopyFrom(detail::ExportElement(tree, tree.GetTopElement(), schema, -1_layer, alloc), alloc);
	rapidjson::StringBuffer buffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	doc.Accept(writer);
	return doc;
}
inline rapidjson::Document ExportJson(const DTree& tree)
{
	Schema schema = InferSchema(tree);
	return ExportJson(tree, schema);
}

} // namespace adapt::json

#endif

#endif
