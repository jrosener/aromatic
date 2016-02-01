#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>

#include "meteo_forecast_printer.h"

Meteo_forecast_printer::Meteo_forecast_printer(const std::vector<Meteo_forecast> &forecasts)
{
    this->forecasts = forecasts;
}

std::string Meteo_forecast_printer::get_html()
{
    std::string html;

    html.append("<html>\n");
    html.append(" <head>\n");
    html.append("  <meta charset=\"utf-8\" />\n");
    html.append("  <style>\n");
    html.append("   body {\n");
    html.append("    font-family: sans;\n");
    html.append("   }\n");
    html.append("   h1 {\n");
    html.append("    font-size: 14px;\n");
    html.append("   }\n");
    html.append("   h2 {\n");
    html.append("    font-size: 10px;\n");
    html.append("   }\n");
    html.append("   table {\n");
    html.append("    font-size: 12px;\n");
    html.append("    border-collapse: collapse;\n");
    html.append("    width: 100%;\n");
    html.append("   }\n");
    html.append("   th {\n");
    html.append("     vertical-align: bottom;\n");
    html.append("   }\n");
    html.append("   th, td {\n");
    html.append("    text-align: center;\n");
    html.append("    padding: 8px;\n");
    html.append("    border-bottom: 1px solid #ddd;\n");
    html.append("   }\n");
    html.append("    th.separator, td.separator {\n");
    html.append("     border-left: 1px solid #ddd;\n");
    html.append("    }\n");
    html.append("   th {\n");
    html.append("    background-color: #4CAF50;\n");
    html.append("    color: white;\n");
    html.append("   }\n");
    html.append("   span.wind_ultra_light {\n");
    html.append("    background-color:#ffffff;\n");
    html.append("   }\n");
    html.append("   span.wind_light {\n");
    html.append("    background-color:#8ff7eb;\n");
    html.append("   }\n");
    html.append("   span.wind_medium {\n");
    html.append("    background-color:#54ea52;\n");
    html.append("   }\n");
    html.append("   span.wind_good {\n");
    html.append("    background-color:#fcfc3c;\n");
    html.append("   }\n");
    html.append("   span.wind_strong {\n");
    html.append("    background-color:#ea9233;\n");
    html.append("   }\n");
    html.append("  </style>\n");
    html.append(" </head>\n");
    html.append(" <body>\n");

    for (auto &forecast : this->forecasts)
    {
        // Title of forecast.
        std::stringstream ss;
        ss << std::fixed << std::setprecision(2) << forecast.get_latitude();
        std::string latitude = ss.str();
        std::stringstream ss2;
        ss2 << std::fixed << std::setprecision(2) << forecast.get_longitude();
        std::string longitude = ss2.str();
        html.append("  <h1>" + forecast.get_location_name() + " - " + forecast.get_start_date_as_str("%a %d %b %Y"));
        std::string map_url = "";
        if (forecast.get_country() == "FR")
        {
            map_url.append("http://tab.geoportail.fr/?c=");
            map_url.append(longitude + "," + latitude);
            map_url.append("&z=16&l0=GEOGRAPHICALGRIDSYSTEMS.MAPS.SCAN-EXPRESS.STANDARD:WMTS(1)&l1=GEOGRAPHICALGRIDSYSTEMS.MAPS:WMTS(1)&l2=ORTHOIMAGERY.ORTHOPHOTOS:WMTS(1)&l3=GEOGRAPHICALGRIDSYSTEMS.MAPS:WMTS(1)&permalink=yes");
        }
        else
        {
            map_url.append("https://www.google.fr/maps/@");
            map_url.append(latitude + "," + longitude + ",14z");
        }
        html.append(" (<a href=" + map_url + ">" + latitude + ", " + longitude + "</a>)</h1>\n");

        // Links to webcam/forecast/...
        if (forecast.get_urls().size() > 0)
        {
            html.append("<h2>");
            for (auto &link : forecast.get_urls())
            {
                html.append("| <a href=" + link.second + ">" + link.first + "</a> | ");
            }
            html.append("</h2>");
        }

        // Table with forecast for every hours.
        html.append("  <table>\n");

        // Days and hours.
        html.append("   <tr>\n");
        html.append("    <th></th>\n");
        std::string cur_day = "";
        for (unsigned int i = 0; i < forecast.get_max_offset(); i++)
        {
            if (cur_day != forecast.get_start_date_offset_as_str((unsigned int)i, "%a"))
            {
                cur_day = forecast.get_start_date_offset_as_str((unsigned int)i, "%a");
                html.append("    <th class=\"separator\">" + forecast.get_start_date_offset_as_str((unsigned int)i, "%a<br/>%Hh") + "</th>\n");
            }
            else
            {
                html.append("    <th>" + forecast.get_start_date_offset_as_str((unsigned int)i, "<br/>%Hh") + "</th>\n");
            }
        }
        html.append("   </tr>\n");

        // Wind.
        html.append("   <tr>\n");
        html.append("    <td>Wind<br/>(knots)</td>\n");
        for (unsigned int i = 0; i < forecast.get_max_offset(); i++)
        {
            if (forecast.get_wind(i).is_defined() == true)
            {
                unsigned int knots = forecast.get_wind(i).get_speed_as_knot();
                if (knots <= 5)                     html.append("    <td><span class=\"wind_ultra_light\">");
                if ((knots >  5)  && (knots <= 10)) html.append("    <td><span class=\"wind_light\">");
                if ((knots >  10) && (knots <= 15)) html.append("    <td><span class=\"wind_medium\">");
                if ((knots >  15) && (knots <= 20)) html.append("    <td><span class=\"wind_good\">");
                if (knots >  20)                    html.append("    <td><span class=\"wind_strong\">");
                html.append(forecast.get_wind(i).get_direction_as_arrow()
                             + std::to_string(knots)
//                           + "<br/>" + std::to_string(forecast.get_wind(i).get_direction_as_deg())
                             + "</span></td>\n");
            }
            else
            {
                html.append("    <td>--</td>\n");
            }
        }
        html.append("   </tr>\n");

        // Temperature.
        html.append("   <tr>\n");
        html.append("    <td>Temp.<br/>(°C)</td>\n");
        for (unsigned int i = 0; i < forecast.get_max_offset(); i++)
        {
            if (forecast.get_temperature(i).is_defined() == true)
                html.append("    <td>" + std::to_string(forecast.get_temperature(i).get_as_celsius()) + "</td>\n");
            else
                html.append("    <td>--</td>\n");
        }
        html.append("   </tr>\n");

        html.append("  </table>\n");
        html.append("  <br/>");
    }

    html.append(" </body>\n");
    html.append("</html>\n");

    return html;
}

std::string Meteo_forecast_printer::get_txt()
{
    std::string txt = "";

    for (auto &forecast : this->forecasts)
    {
        txt.append("\n");
        txt.append("[Forecast @ " + forecast.get_location_name() + "]\n");
        for (unsigned int i = 0; i < forecast.get_max_offset(); i++)
        {
            txt.append("  " + forecast.get_start_date_offset_as_str((unsigned int)i, "%a %d %b - %H:%M"));
            if (forecast.get_temperature(i).is_defined() == true)
                txt.append(" : Temp = " + std::to_string(forecast.get_temperature(i).get_as_celsius()) + "°C");
            else
                txt.append("--");
            if (forecast.get_wind(i).is_defined() == true)
            {
                txt.append(" / Wind = " + std::to_string(forecast.get_wind(i).get_speed_as_knot()) + "knots");
                txt.append(" (" + forecast.get_wind(i).get_direction_as_str() + ")");
            }
            else
            {
                txt.append(" / --");
            }
            txt.append("\n");
        }
        txt.append("\n");
    }

    return txt;
}
