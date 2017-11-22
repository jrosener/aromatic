#include "wind.h"
#include <cmath>

Wind::Wind()
{

}

void Wind::set_components_u_v(const float &u_ms, const float &v_ms)
{
    // Calculate wind direction.
    if (v_ms > 0.0)
    {
        this->direction = ((180 / M_PI) * std::atan(u_ms / v_ms) + 180);
    }
    if ((u_ms < 0.0) && (v_ms < 0.0))
    {
        this->direction = ((180 / M_PI) * std::atan(u_ms / v_ms) + 0);
    }
    if ((u_ms > 0.0) && (v_ms < 0.0))
    {
        this->direction = ((180 / M_PI) * std::atan(u_ms / v_ms) + 360);
    }

    // Calculate wind speed.
    this->speed = std::sqrt(std::pow(u_ms, 2) + std::pow(v_ms, 2));

    this->defined = true;
    return;
}

int Wind::get_speed_as_knot()
{
    return std::round(this->speed * 1.94384);
}

int Wind::get_direction_as_deg()
{
    return this->direction;
}

std::string Wind::get_direction_as_str()
{
    std::string str = "";

    // Transformation based on: http://wx.gmu.edu/dev/clim301/lectures/wind/wind-uv.html
    if ((this->direction >= 0)  && (this->direction <= 22))  str = "N";
    if ((this->direction > 22)  && (this->direction <= 67))  str = "NE";
    if ((this->direction > 67)  && (this->direction <= 112)) str = "E";
    if ((this->direction > 112) && (this->direction <= 157)) str = "SE";
    if ((this->direction > 157) && (this->direction <= 202)) str = "S";
    if ((this->direction > 202) && (this->direction <= 247)) str = "SO";
    if ((this->direction > 247) && (this->direction <= 292)) str = "O";
    if ((this->direction > 292) && (this->direction <= 337)) str = "NO";
    if ((this->direction > 337) && (this->direction <= 360)) str = "N";

    return str;
}

std::string Wind::get_direction_as_arrow()
{
    std::string dir = this->get_direction_as_str();

    if (dir == "N")  return "\u2193";
    if (dir == "NO") return "\u2198";
    if (dir == "O")  return "\u2192";
    if (dir == "SO") return "\u2197";
    if (dir == "S")  return "\u2191";
    if (dir == "SE") return "\u2196";
    if (dir == "E")  return "\u2190";
    if (dir == "NE") return "\u2199";

    return "";
}

bool Wind::is_defined()
{
    return this->defined;
}

void Wind::set_defined(bool is_def)
{
    this->defined = is_def;
}
