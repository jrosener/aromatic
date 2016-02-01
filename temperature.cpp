#include "temperature.h"
#include <cmath>

Temperature::Temperature()
{

}

void Temperature::set(const float &kelvin)
{
    this->value = kelvin;
    this->defined = true;
}

int Temperature::get_as_celsius()
{
    return std::round(this->value - 273.15);
}

bool Temperature::is_defined()
{
    return this->defined;
}
