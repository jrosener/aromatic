#ifndef TEMPERATURE_H
#define TEMPERATURE_H


class Temperature
{
 private:
    float value = 0.0; // Temperature in Kelvin.
    bool defined = false;

 public:
    Temperature();
    void set(const float &kelvin);
    int get_as_celsius();
    bool is_defined();
};

#endif // TEMPERATURE_H
