#ifndef ADAPT_PLOT_AXES_H
#define ADAPT_PLOT_AXES_H

#include <OpenADAPT/Utility/Macros.h>
#include <OpenADAPT/Plot/Core.h>

namespace adapt
{

namespace plot_detail
{

#define DEF_AXIS(AXIS, axis)\
template <class Canvas_>\
class Axis##AXIS\
{\
	Canvas_* GetDerived() { return static_cast<Canvas_*>(this); }\
	const Canvas_* GetDerived() const { return static_cast<const Canvas_*>(this); }\
public:\
	void Set##AXIS##Label(const std::string& label) { GetDerived()->SetLabel(axis, label); }\
	void Set##AXIS##Range(double min, double max) { GetDerived()->SetRange(axis, min, max); }\
	void Set##AXIS##RangeMin(double min) { GetDerived()->SetRangeMin(axis, min); }\
	void Set##AXIS##RangeMax(double max) { GetDerived()->SetRangeMax(axis, max); }\
	void SetLog##AXIS(double base = 10) { GetDerived()->SetLog(axis, base); }\
	void SetFormat##AXIS(const std::string& fmt) { GetDerived()->SetFormat(axis, fmt); }\
	void Set##AXIS##DataTime(const std::string& fmt = std::string()) { GetDerived()->SetDataTime(axis, fmt); }\
	template <class ...Args>\
	void Set##AXIS##Tics(Args&& ...args) { GetDerived()->SetTics(axis, std::forward<Args>(args)...); }\
	void Set##AXIS##TicsRotate(double ang) { GetDerived()->SetTicsRotate(axis, ang); }\
	void Set##AXIS##LabelFont(std::string_view font, double size = 0.0) { GetDerived()->SetFont(axis"label", font, size); }\
	void Set##AXIS##TicsFont(std::string_view font, double size = 0.0) { GetDerived()->SetFont(axis"tics", font, size); }\
	int SetLabelFont(std::string_view font, double size = 0.0) { Set##AXIS##LabelFont(font, size); return 0; }\
	int SetTicsFont(std::string_view font, double size = 0.0) { Set##AXIS##TicsFont(font, size); return 0; }\
};

DEF_AXIS(X, "x")
DEF_AXIS(X2, "x2")
DEF_AXIS(Y, "y")
DEF_AXIS(Y2, "y2")
DEF_AXIS(Z, "z")
DEF_AXIS(Z2, "z2")
DEF_AXIS(CB, "cb")

#undef DEF_AXIS

/*template <class Derived, class Axes, class Indices = std::make_index_sequence<Axes::size>>
class AxesMethods;
template <class Derived, template <class> class ...Axes, size_t ...Indices>
class AxesMethods<Derived, UnarguedList<Axes...>, std::index_sequence<Indices...>> : public Axes<Derived>...
{
	template <size_t I>
	const auto& GetAxis() const { return static_cast<GetType_t<I, Axes<Derived>...>>(this); }
public:
	void SetLabelFont(std::string_view font, double size = 0.0)
	{
		int x = (Axes<Derived>::SetLabelFont(font, size) + ...);
	}
};
template <class Derived>
using Axes2D = AxesMethods<Derived, UnarguedList<AxisX, AxisY, AxisX2, AxisY2, AxisCB>>;*/

}

}

#endif