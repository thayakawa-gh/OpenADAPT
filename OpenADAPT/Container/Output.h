#ifndef ADAPT_RANGE_OUTPUT_H
#define ADAPT_RANGE_OUTPUT_H

#include <iomanip>
#include <string_view>
#include <fstream>
#include <OpenADAPT/Utility/Exception.h>
#include <OpenADAPT/Container/Tree.h>
#include <OpenADAPT/Range/RangeAdapter.h>

namespace adapt
{

namespace detail
{

//左端のスペースを含む幅を返す。
template <FieldType Type>
consteval int GetDefaultWidth()
{
	using enum FieldType;
	if constexpr (Type == I08) return 5;
	else if constexpr (Type == I16) return 7;
	else if constexpr (Type == I32) return 12;
	else if constexpr (Type == I64) return 21;
	else if constexpr (Type == F32) return 10;
	else if constexpr (Type == F64) return 12;
	else if constexpr (Type == I16) return 12;
	else if constexpr (Type == I16) return 12;
	else if constexpr (Type == I16) return 12;
	else if constexpr (Type == I16) return 12;
	else if constexpr (Type == I16) return 12;
	else if constexpr (Type == I16) return 12;
	else if constexpr (Type == I16) return 12;
	else if constexpr (Type == I16) return 12;
}

inline void WriteAsText_print(std::ostream& out, int8_t i)
{
	out << std::setw(5) << (int)i;
}
inline void WriteAsText_print(std::ostream& out, int16_t i)
{
	out << std::setw(7) << i;
}
inline void WriteAsText_print(std::ostream& out, int32_t i)
{
	out << std::setw(12) << i;
}
inline void WriteAsText_print(std::ostream& out, int64_t i)
{
	out << std::setw(21) << i;
}

inline void WriteAsText_print(std::ostream& out, float f)
{
	//floatやdoubleの場合、幅をうまく整形する必要がある。
	//floatの精度はだいたい6~7桁。指数部は2桁なので符号と合わせて4文字。
	//小数点、マイナス符号を合わせると13文字あれば完全に表示できるが、切りが悪いので12文字とする。
	//12文字のとき、6桁の表示を保証する場合、fixedなら-0.000654321 ~ -123456789.0まではfixedで表せる。
	int x = (int)std::log10(std::abs(f));//絶対値は10^x乗以上。
	if (x >= -4 && x < 9)
	{
		//fixedで表示できる範囲。
		int prec = std::max(1, 7 - x - 1);//小数点より下の桁数。最低でも1桁は表示する。
		out << std::fixed << std::setw(13) << std::setprecision(prec) << f;
	}
	else
	{
		out << std::scientific << std::setw(13) << std::setprecision(5) << f;
	}
}
inline void WriteAsText_print(std::ostream& out, double f)
{
	//floatの精度はだいたい15桁。指数部は4桁なので符号と合わせて6文字。
	//小数点、マイナス符号を合わせると完全に表示するには23文字も必要。流石に長すぎるので16文字に切り詰める。
	//16文字のとき、10桁の表示を保証する場合、fixedなら-0.0009876543210 ~ -1234567890123.0まではfixedで表せる。
	int x = (int)std::log10(std::abs(f));//絶対値は10^x乗以上。
	if (x >= -4 && x < 13)
	{
		//fixedで表示できる範囲。
		int prec = std::max(1, 10 - x - 1);//小数点より下の桁数。最低でも1桁は表示する。
		out << std::fixed << std::setw(17) << std::setprecision(prec) << f;
	}
	else
	{
		out << std::scientific << std::setw(17) << std::setprecision(8) << f;
	}
}
inline void WriteAsText_print(std::ostream& out, std::complex<float> c)
{
	out << "(";
	WriteAsText_print(out, c.real());
	out << ",";
	WriteAsText_print(out, c.imag());
	out << ")";
}
inline void WriteAsText_print(std::ostream& out, std::complex<double> c)
{
	out << "(";
	WriteAsText_print(out, c.real());
	out << ",";
	WriteAsText_print(out, c.imag());
	out << ")";
}
inline void WriteAsText_print(std::ostream& out, std::string_view str)
{
	//substrだとutf-8の表示がおかしくなる場合がある。
	//そもそもsetwが正常に機能しないのだが、改善方法がわからない。
	//out << std::setw(17) << (str.size() <= 16 ? str : str.substr(0, 16));
	out << std::setw(17) << str;
}


inline void WriteAsText_print(std::ostream& out, const Bpos& bpos, int width = 11)
{
	LayerType layer = bpos.GetLayer();
	if (layer == -1_layer) out << "[]";
	else
	{
		out << "[" << std::setw(width) << bpos[0_layer];
		for (LayerType l = 1_layer; l <= layer; ++l) out << "," << std::setw(width) << bpos[l];
		out << "]";
	}
}
//widthにはBindexの最小幅を与える。デフォルトではuint32_tを全て表示できるよう11とする。
inline void WriteAsText_print(std::ostream& out, const JBpos& jbp, int width = 11)
{
	RankType rank = jbp.GetMaxRank();
	if (rank == -1_rank) out << "[]";
	else
	{
		out << "[" << std::setw(width) << jbp[0_rank];
		for (RankType r = 1_layer; r <= rank; ++r)
		{
			out << ",";
			WriteAsText_print(out, jbp[r], width);
		}
		out << "]";
	}
}

template <class Trav, typed_node_or_placeholder NP>
void WriteAsText_print(std::ostream& out, const Trav& trav, NP&& np, [[maybe_unused]] int name_len = 0)
{
	++name_len;
	std::ios_base::fmtflags f = out.flags();
	WriteAsText_print(out, np(trav));
	out.flags(f);
}

template <class Trav, rtti_node_or_placeholder NP>
void WriteAsText_print(std::ostream& out, const Trav& trav, NP&& np, [[maybe_unused]] int name_len = 0)
{
	//name_lenは現在使っていない。が、いずれヘッダ出力をするときに使いたい。
	++name_len;//スペース一個分追加。
	std::ios_base::fmtflags f = out.flags();
	using enum FieldType;
	switch (np.GetType())
	{
	case I08:
		WriteAsText_print(out, np(trav).i08());
		break;
	case I16:
		WriteAsText_print(out, np(trav).i16());
		break;
	case I32:
		WriteAsText_print(out, np(trav).i32());
		break;
	case I64:
		WriteAsText_print(out, np(trav).i64());
		break;
	case F32:
		WriteAsText_print(out, np(trav).f32());
		break;
	case F64:
		WriteAsText_print(out, np(trav).f64());
		break;
	case C32:
		WriteAsText_print(out, np(trav).c32());
		break;
	case C64:
		WriteAsText_print(out, np(trav).c64());
		break;
	case Str:
		WriteAsText_print(out, np(trav).str());
		break;
	case Jbp:
		WriteAsText_print(out, np(trav).jbp());
		break;
	case Emp:
		out << "      Unknown";
	}
	out.flags(f);
}

//ヘッダを出力しつつ、各NPの変数名の長さを返す。
/*template <class Trav, node_or_placeholder ...NPs>
auto WriteHeaderAsText(std::ostream& out, NPs&& ...nps)
{

}*/

template <class Trav, node_or_placeholder NP, node_or_placeholder ...NPs>
void WriteAsText_rec(std::ostream& out, const Trav& trav, NP&& np, NPs&& ...nps)
{
	WriteAsText_print(out, trav, np);
	WriteAsText_rec(out, trav, std::forward<NPs>(nps)...);
}
template <class Trav>
void WriteAsText_rec(std::ostream& out, const Trav&)
{
	out << "\n";
}

}

//move層の出力。
template <traversal_range Range, node_or_placeholder ...NPs>
void WriteAsText(std::ostream& out, Range&& range, NPs&& ...nps)
{
	Bpos pos;
	LayerType max = std::max({ nps.GetLayer()... });
	max = std::max(max, range.GetTravLayer());
	range.SetTravLayer(max);
	pos.Init(max);
	for (auto& trav : range)
	{
		try
		{
			std::ostringstream oss;
			trav.GetBpos(pos);
			detail::WriteAsText_print(oss, pos, 4);
			detail::WriteAsText_rec(oss, trav, std::forward<NPs>(nps)...);
			out << oss.str();
		}
		catch (const JointError&)
		{}
	}
}

namespace detail
{

template <class Range>
class OutputConsumer
{
public:

	template <traversal_range Range_, class Stream, node_or_placeholder ...NPs_>
		requires (std::convertible_to<Range_, Range>)
	void Exec(Range_&& range, Stream& ost, NPs_&& ...args)
	{
		WriteAsText(ost, std::forward<Range_>(range), std::forward<NPs_>(args)...);
	}
};

}

template <node_or_placeholder ...NPs>
RangeConsumer<detail::OutputConsumer, std::ostream&, NPs...> Show(NPs&& ...nps)
{
	return RangeConsumer<detail::OutputConsumer, std::ostream&, NPs...>(std::cout, std::forward<NPs>(nps)...);
}
template <node_or_placeholder ...NPs>
RangeConsumer<detail::OutputConsumer, std::ofstream, NPs...> Write(std::string_view filename, NPs&& ...nps)
{
	std::ofstream ost(filename.data());
	if (!ost) throw BadFile("file cannot open.");
	return RangeConsumer<detail::OutputConsumer, std::ofstream, NPs...>(std::move(ost), std::forward<NPs>(nps)...);
}


}

#endif
