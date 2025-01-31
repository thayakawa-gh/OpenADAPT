#ifndef ADAPT_PLOT_AXES_H
#define ADAPT_PLOT_AXES_H

#include <OpenADAPT/Plot/Core.h>

namespace adapt
{

namespace plot_detail
{

#define DEF_AXIS(AXIS, axis)\
template <class Canvas_>\
class Axis##AXIS : public Canvas_\
{\
public:\
	using Canvas_::Canvas_;\
	void Set##AXIS##Label(const std::string& label) { this->SetLabel(axis, label); }\
	void Set##AXIS##Range(double min, double max) { this->SetRange(axis, min, max); }\
	void Set##AXIS##RangeMin(double min) { this->SetRangeMin(axis, min); }\
	void Set##AXIS##RangeMax(double max) { this->SetRangeMax(axis, max); }\
	void SetLog##AXIS(double base = 10) { this->SetLog(axis, base); }\
	void SetFormat##AXIS(const std::string& fmt) { this->SetFormat(axis, fmt); }\
	void Set##AXIS##DataTime(const std::string& fmt = std::string()) { this->SetDataTime(axis, fmt); }\
	template <class ...Args>\
	void Set##AXIS##Tics(Args&& ...args) { this->SetTics(axis, std::forward<Args>(args)...); }\
	void Set##AXIS##TicsRotate(double ang) { this->SetTicsRotate(axis, ang); }\
};

DEF_AXIS(X, "x")
DEF_AXIS(X2, "x2")
DEF_AXIS(Y, "y")
DEF_AXIS(Y2, "y2")
DEF_AXIS(Z, "z")
DEF_AXIS(Z2, "z2")
DEF_AXIS(CB, "cb")

#undef DEF_AXIS

template <class Canvas_>
struct Axis2D : public AxisX<AxisX2<AxisY<AxisY2<AxisCB<Canvas_>>>>>
{
	using Base = AxisX<AxisX2<AxisY<AxisY2<AxisCB<Canvas_>>>>>;
	using Base::Base;
};
template <class Canvas_>
struct Axis3D : public AxisZ<Axis2D<Canvas_>>
{
	using Base = AxisZ<Axis2D<Canvas_>>;
	using Base::Base;
};

}

}

#endif