#ifndef WIND_H
#define WIND_H

#include <string>

class Wind
{
 private:
   float speed = 0.0; // Speed in m/s.
   int direction = 0; // Direction in degrees [0 , 360]
   bool defined = false;

 public:
    Wind();
    void set_components_u_v(const float &u_ms, const float &v_ms);
    int get_speed_as_knot();
    int get_direction_as_deg();
    std::string get_direction_as_str();
    std::string get_direction_as_arrow();
    bool is_defined();
    void set_defined(bool is_def);
};

#endif // WIND_H
