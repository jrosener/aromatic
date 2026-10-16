#include "wind.h"
#include <cmath>
#include <sstream>

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

// 16-point compass rose, each sector is 22.5\u00b0 wide and centered on its direction
// (ex: N = [348.75\u00b0, 11.25\u00b0[, NNE = [11.25\u00b0, 33.75\u00b0[,...).
static const char *COMPASS_POINTS[16] = {"N", "NNE", "NE", "ENE", "E", "ESE", "SE", "SSE",
                                         "S", "SSO", "SO", "OSO", "O", "ONO", "NO", "NNO"};

static int get_compass_sector(int direction)
{
    return static_cast<int>(std::floor((direction + 11.25) / 22.5)) % 16;
}

std::string Wind::get_direction_as_str()
{
    if ((this->direction < 0) || (this->direction > 360))
        return "";

    return COMPASS_POINTS[get_compass_sector(this->direction)];
}

std::string Wind::get_direction_as_arrow()
{
    if ((this->direction < 0) || (this->direction > 360))
        return "";

    // Unicode only has arrows for the 8 main directions, so rotate a down arrow
    // (wind coming from N) by the angle of the compass sector: the arrow shows
    // where the wind blows to.
    std::stringstream angle;
    angle << get_compass_sector(this->direction) * 22.5;

    return "<span class=\"wind_arrow\" style=\"transform: rotate(" + angle.str() + "deg)\">\u2193</span>";
}

bool Wind::is_defined()
{
    return this->defined;
}

void Wind::set_defined(bool is_def)
{
    this->defined = is_def;
}
