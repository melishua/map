#ifndef MAP_H
#define MAP_H
#include "StreetsDatabaseAPI.h"
#include "OSMDatabaseAPI.h"
#include <set>
#include <map>
#include <string>
#include <vector>
#include <algorithm>
#include "graphics_types.h"
#include <math.h>
#include <list>

#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/index/rtree.hpp>

namespace bg = boost::geometry;
namespace bgi = boost::geometry::index;

typedef bg::model::point<double, 2, bg::cs::cartesian> point;
typedef std::pair<point, unsigned> value;

struct street_seg_info {
    IntersectionIndex from, to;
    bool oneWay;
    double length;
    float speedLimit;
    StreetIndex street_index;
    std::string street_name;
    OSMID way_OSMID;
    unsigned curvePointCount;
    bool isDrawn;
};

struct intersection_info {
    LatLon position;
    std::string name;
};

struct feature_info {
    unsigned id;
    std::string name;
    FeatureType type;
    bool closed;
    double area;
    unsigned vertex_count;
    std::vector<t_point> vertex_list;

    bool operator()(int i, int j) {
        return (i < j);
    }
};

struct poi_info {
    LatLon position;
    std::string name;
    std::string type;
    /*
     hospital: pharmacy, clinic, doctors, hospital, dentist, veterinary
     restaurant: fast_food, cafe, restaurant, pub, bar
     bank: bank, atm, bureau_de_change
     fuel: fuel
     infrastructure: community_centre, social_facility, police, fire_station
     school: tutoring, kindergarten, college, school, childcare, university, tutor,library
     parking: parking
     entertainment: theatre, cinema, internet_cafe, nightclub, fitness_center, gym, banquet_hall, gymnasium, swimming_pool, stripclub
     shop: marketplace
     postal: post_office, post_box, ems_station
     hotel: hotel, motel,
     religion: place_of_worship
     transportation: bus_station, ferry_terminal
     */

};

struct path_bound {
    double top;
    double bot;
    double left;
    double right;
    path_bound (double _top,double _bot,double _left,double _right) {
        top = _top;
        bot = _bot;
        left = _left;
        right = _right;
    }
};

struct search_input_id {
    std::vector<unsigned> start;
    std::vector<unsigned> end;
    bool search_poi_flag = false;
};

enum POI_type {
    hospital,
    restaurant,
    bank,
    fuel,
    infrastructure,
    school,
    parking,
    entertainment,
    shop,
    postal,
    hotel,
    transportation,
    religion,
    nothing
};

class Map {
public:
    Map();
    virtual ~Map();
    void clear();
    bool load_layer2_map(std::string map_name);
    bool load_layer1_map(std::string map_name);
    double find_distance_between_two_points(LatLon point1, LatLon point2);
    t_point latlon_to_cartesian(LatLon ll);
    float longitude_to_cartesian(float lon);
    float latitude_to_cartesian(float lat);

    /***************************************************************************
                                 M1 variables
     ***************************************************************************/
    // stores map data from API into vectors and rtrees
    std::vector<street_seg_info> street_segments_list;
    std::vector<double> seg_travel_time_list;
    std::vector<std::string> street_name_list;
    std::vector<std::vector<unsigned>> street_street_segments;
    std::vector<std::vector<unsigned>> intersection_street_segments;
    std::vector<std::vector<unsigned>> street_intersections;
    std::vector<std::vector<LatLon>> street_segments_curvepoint;

    std::map<std::string, std::vector<unsigned>> street_name_street_id;
    std::map<std::string, std::vector<unsigned>> poi_name_poi_ids;

    bgi::rtree<value, bgi::quadratic < 16 >> rtree_poi;
    bgi::rtree<value, bgi::quadratic < 16 >> rtree_intersection;

    /***************************************************************************
                                M2 variables
     ***************************************************************************/
    double maximum_latitude;
    double minimum_latitude;
    double maximum_longitude;
    double minimum_longitude;
    double average_latitude;

    std::vector<intersection_info> intersection_list;
    std::vector<feature_info> feature_area_list;
    std::vector<poi_info> poi_list;
    std::vector<StreetSegmentIndex> start_level;
    std::vector<StreetSegmentIndex> block_level;
    std::vector<StreetSegmentIndex> street_level;
    std::vector<StreetSegmentIndex> search_level;
    std::vector<StreetSegmentIndex> park_path;

    /***************************************************************************
                                constant variables
     ***************************************************************************/
    unsigned total_number_street;
    unsigned total_number_street_segment;
    unsigned total_number_intersection;
    unsigned total_number_poi;
    unsigned total_number_feature;

private:
    //helper function of load layer1 map 
    std::string StreetToOSM(std::string name);
    std::vector<StreetSegmentIndex> FindTargetStreetSegment(OSMID target);
    std::vector<StreetSegmentIndex> FillStreetSegment();
    //helper function to compute area of features(polygon)
    double computeArea (unsigned vertex_count, std::vector<t_point>& vertex_list) const;
    
};

#endif /* MAP_H */

