/* 
 * File:   Map.cpp
 * Author: yuanxin8
 * 
 * Created on February 19, 2017, 8:29 PM
 */

#include "Map.h"
constexpr double DEG_TO_RAD = 0.017453292519943295769236907684886;
constexpr double EARTH_RADIUS_IN_METERS = 6372797.560856;
bool sortDescendArea(const feature_info& a, const feature_info& b);

Map::Map() {
}

Map::~Map() {
}

void Map::clear() {
    // stores map data from API into vectors and rtrees
    street_segments_list.clear();
    seg_travel_time_list.clear();
    street_name_list.clear();
    street_street_segments.clear();
    intersection_street_segments.clear();
    street_intersections.clear();

    street_name_street_id.clear();

    rtree_poi.clear();
    rtree_intersection.clear();

    intersection_list.clear();
    feature_area_list.clear();
    poi_list.clear();
    start_level.clear();
    block_level.clear();
    street_level.clear();
    search_level.clear();
    park_path.clear();
}

bool Map::load_layer2_map(std::string map_name) {
    if (!loadStreetsDatabaseBIN(map_name)) {
        return false;
    }
    // aquire basic const information from API
    total_number_street = getNumberOfStreets();
    total_number_street_segment = getNumberOfStreetSegments();
    total_number_intersection = getNumberOfIntersections();
    total_number_poi = getNumberOfPointsOfInterest();
    total_number_feature = getNumberOfFeatures();

    // create pointer to vector to allocate vector with size found in run-time
    std::vector<street_seg_info> *temp_street_segments_list
            = new std::vector<street_seg_info>(total_number_street_segment);
    std::vector<std::vector<unsigned>> *temp_street_street_segments
            = new std::vector<std::vector<unsigned>>(total_number_street);
    std::vector<std::vector<unsigned>> *temp_street_intersections
            = new std::vector<std::vector<unsigned>>(total_number_street);
    std::vector<std::string> *temp_street_name_list
            = new std::vector<std::string>(total_number_street);

    /***************************************************************************
                            intersection iteration
     ***************************************************************************/
    intersection_list.resize(total_number_intersection);
    for (unsigned current_int = 0; current_int < total_number_intersection; current_int++) {
        LatLon ll = getIntersectionPosition(current_int);

        // fill in intersection_list
        intersection_list[current_int].position = ll;
        intersection_list[current_int].name = getIntersectionName(current_int);
    // compute average latitude of the map
    average_latitude = 0.5 * (maximum_latitude + minimum_latitude) * DEG_TO_RAD;

        // store intersection street segment
        unsigned connected_seg_count = getIntersectionStreetSegmentCount(current_int);
        std::vector<unsigned> connected_seg_list;
        for (unsigned current_seg = 0; current_seg < connected_seg_count; current_seg++) {
            connected_seg_list.push_back(getIntersectionStreetSegment(current_int, current_seg));
        }
        intersection_street_segments.push_back(connected_seg_list);

        if (current_int == 0) {
            // setup all max/min lat/lon to the first intersection in the city
            maximum_latitude = ll.lat();
            minimum_latitude = maximum_latitude;
            maximum_longitude = ll.lon();
            minimum_longitude = maximum_longitude;
        } else {
            //find maximum and minimum lat and lon of the map according to intersection positions
            maximum_latitude = std::max(maximum_latitude, ll.lat());
            minimum_latitude = std::min(minimum_latitude, ll.lat());

            maximum_longitude = std::max(maximum_longitude, ll.lon());
            minimum_longitude = std::min(minimum_longitude, ll.lon());
        }
    }
    // compute average latitude of the map
    average_latitude = 0.5 * (maximum_latitude + minimum_latitude) * DEG_TO_RAD;

    // build rtree for intersections
    for (unsigned current_int = 0; current_int < total_number_intersection; current_int++) {
        LatLon ll = intersection_list[current_int].position;
        rtree_intersection.insert(std::make_pair(point(longitude_to_cartesian(ll.lon()), latitude_to_cartesian(ll.lat())), current_int));
    }

    /***************************************************************************
                             street segment iteration
     ***************************************************************************/
    //populate intersection street segments with the street segment IDs for each intersection.
    for (unsigned current_seg = 0; current_seg < total_number_street_segment; current_seg++) {

        // aquire street segment info from API
        StreetSegmentInfo api_seg_info = getStreetSegmentInfo(current_seg);

        // extract information of street segment and store into new defined structure
        street_seg_info my_seg_info;

        my_seg_info.from = api_seg_info.from;
        my_seg_info.to = api_seg_info.to;
        my_seg_info.oneWay = api_seg_info.oneWay;
        my_seg_info.speedLimit = api_seg_info.speedLimit;
        my_seg_info.street_index = api_seg_info.streetID;
        my_seg_info.street_name = getStreetName(api_seg_info.streetID);
        my_seg_info.way_OSMID = api_seg_info.wayOSMID;
        my_seg_info.curvePointCount = api_seg_info.curvePointCount;
        my_seg_info.isDrawn = false;

        //calculate street segment length
        LatLon start = getIntersectionPosition(my_seg_info.from);
        LatLon end = getIntersectionPosition(my_seg_info.to);

        if (api_seg_info.curvePointCount == 0){
            my_seg_info.length = find_distance_between_two_points(start, end);
            std::vector<LatLon> curve_point_latlon_list;
            street_segments_curvepoint.push_back(curve_point_latlon_list);
        }
        else {
            std::vector<LatLon> curve_point_latlon_list;
            for (unsigned current_curve_point = 0; current_curve_point < api_seg_info.curvePointCount; current_curve_point++) {
                curve_point_latlon_list.push_back(getStreetSegmentCurvePoint(current_seg, current_curve_point));
            }
            street_segments_curvepoint.push_back(curve_point_latlon_list);
            double length = 0.0;
            length += find_distance_between_two_points(start, curve_point_latlon_list[0]);

            for (unsigned i = 0; i < api_seg_info.curvePointCount - 1; i++) {
                LatLon p1 = curve_point_latlon_list[i];
                LatLon p2 = curve_point_latlon_list[i + 1];
                length += find_distance_between_two_points(p1, p2);
            }

            length += find_distance_between_two_points(curve_point_latlon_list[api_seg_info.curvePointCount - 1], end);
            my_seg_info.length = length;
        }

        seg_travel_time_list.push_back(my_seg_info.length / (my_seg_info.speedLimit / 3.6));

        // store street seg info
        temp_street_segments_list -> at(current_seg) = my_seg_info;

        // store street_street_segment
        temp_street_street_segments -> at(my_seg_info.street_index).push_back(current_seg);

        // store street name
        temp_street_name_list -> at(my_seg_info.street_index) = my_seg_info.street_name;

        // store street_intersection
        temp_street_intersections -> at(my_seg_info.street_index).push_back(my_seg_info.from);
        temp_street_intersections -> at(my_seg_info.street_index).push_back(my_seg_info.to);

    }// end street segments loop


    /***************************************************************************
                                    street iteration
     ***************************************************************************/

    // eliminate intersection duplicate and store street name street id  
    for (unsigned street_id = 0; street_id < total_number_street; street_id++) {
        std::set<unsigned> unique_int(temp_street_intersections
                -> at(street_id).begin(), temp_street_intersections -> at(street_id).end());
        temp_street_intersections -> at(street_id).assign(unique_int.begin(), unique_int.end());

        std::vector<unsigned> street_id_vector;
        if (street_name_street_id.find(temp_street_name_list
                -> at(street_id)) == street_name_street_id.end()) {
            for (unsigned current_street = street_id; current_street < total_number_street; current_street++) {
                if (temp_street_name_list -> at(street_id)
                        == temp_street_name_list -> at(current_street)) {
                    street_id_vector.push_back(current_street);
                }
            }
        }

        std::pair<std::string, std::vector<unsigned>> temp(temp_street_name_list
                -> at(street_id), street_id_vector);
        street_name_street_id.insert(temp);
    }

    /***************************************************************************
                            point of interest iteration
     ***************************************************************************/
    // build rtree for points of interest
    poi_list.resize(total_number_poi);
    std::vector<std::string> unique_poi_name;
    for (unsigned current_poi = 0; current_poi < total_number_poi; current_poi++) {
        LatLon ll = getPointOfInterestPosition(current_poi);
        rtree_poi.insert(std::make_pair(point(longitude_to_cartesian(ll.lon()), latitude_to_cartesian(ll.lat())), current_poi));
        poi_list[current_poi].position = ll;
        std::string poi_name = getPointOfInterestName(current_poi);
        poi_list[current_poi].name = poi_name;
        poi_list[current_poi].type = getPointOfInterestType(current_poi);
        
        unique_poi_name.push_back(poi_list[current_poi].name);       
    }
    /***************************************************************************
                            point of interest name based iteration
     ***************************************************************************/
    std::sort(unique_poi_name.begin(), unique_poi_name.end());
    std::vector<std::string>::iterator it = std::unique(unique_poi_name.begin(), unique_poi_name.end());
    unique_poi_name.resize(std::distance(unique_poi_name.begin(),it));
    
    for (unsigned current_interest_name_index = 0; current_interest_name_index < unique_poi_name.size(); current_interest_name_index++) {
        std::vector<unsigned> temp_poi_name;
        for (unsigned current_poi = 0; current_poi < total_number_poi; current_poi++){
            if (poi_list[current_poi].name.compare(unique_poi_name[current_interest_name_index]) == 0){
                temp_poi_name.push_back(current_poi);
            }
        }
        std::pair<std::string, std::vector<unsigned>> temp(unique_poi_name[current_interest_name_index], temp_poi_name);
        poi_name_poi_ids.insert(temp);
    }
       
    /***************************************************************************
                                    feature iteration
     ***************************************************************************/
    feature_area_list.resize(total_number_feature);
    for (unsigned current_feature = 0; current_feature < total_number_feature; current_feature++) {
        feature_area_list[current_feature].id = current_feature;
        feature_area_list[current_feature].name = getFeatureName(current_feature);
        feature_area_list[current_feature].type = getFeatureType(current_feature);

        unsigned point_count = getFeaturePointCount(current_feature);
        feature_area_list[current_feature].vertex_count = point_count;
        feature_area_list[current_feature].vertex_list.resize(point_count);

        for (unsigned current_vertex = 0; current_vertex < point_count; current_vertex++) {
            feature_area_list[current_feature].vertex_list[current_vertex] =
                    latlon_to_cartesian(getFeaturePoint(current_feature, current_vertex));
        }

        if (feature_area_list[current_feature].vertex_list[0].x ==
                feature_area_list[current_feature].vertex_list[point_count - 1].x &&
                feature_area_list[current_feature].vertex_list[0].y ==
                feature_area_list[current_feature].vertex_list[point_count - 1].y) {
            feature_area_list[current_feature].closed = true;
            feature_area_list[current_feature].area = computeArea(point_count, feature_area_list[current_feature].vertex_list);
        } else if (feature_area_list[current_feature].type == Stream) {
            feature_area_list[current_feature].closed = false;
            feature_area_list[current_feature].area = 0;
        } else {
            feature_area_list[current_feature].closed = true;
            feature_area_list[current_feature].vertex_count = point_count + 1;
            feature_area_list[current_feature].vertex_list.push_back(feature_area_list[current_feature].vertex_list[point_count - 1]);
            feature_area_list[current_feature].area = computeArea(point_count + 1, feature_area_list[current_feature].vertex_list);
        }
    }
    std::sort(feature_area_list.begin(), feature_area_list.end(), sortDescendArea);

    // clear dynamically allocated variables
    street_segments_list = *temp_street_segments_list;
    street_street_segments = *temp_street_street_segments;
    street_intersections = *temp_street_intersections;
    street_name_list = *temp_street_name_list;
    delete temp_street_street_segments;
    delete temp_street_segments_list;
    delete temp_street_intersections;
    delete temp_street_name_list;

    return true;
}

float Map::longitude_to_cartesian(float lon) {
    return lon * cos(average_latitude) * DEG_TO_RAD;
}

float Map::latitude_to_cartesian(float lat) {
    return lat * DEG_TO_RAD;
}

t_point Map::latlon_to_cartesian(LatLon ll) {
    float x = ll.lon() * cos(average_latitude) * DEG_TO_RAD;
    float y = ll.lat() * DEG_TO_RAD;
    t_point cartesian(x, y);
    return cartesian;
}

double Map::find_distance_between_two_points(LatLon point1, LatLon point2) {
    double lat_avg = (point1.lat() + point2.lat()) * DEG_TO_RAD / 2.0;
    double x1 = DEG_TO_RAD * point1.lon() * cos(lat_avg);
    double x2 = DEG_TO_RAD * point2.lon() * cos(lat_avg);
    return EARTH_RADIUS_IN_METERS * sqrt(pow((point2.lat() - point1.lat()) * DEG_TO_RAD, 2) + pow(x2 - x1, 2));
}

bool Map::load_layer1_map(std::string map_name) {
    map_name = StreetToOSM(map_name);

    if (!loadOSMDatabaseBIN(map_name)) {
        return false;
    }

    unsigned total_number_ways = getNumberOfWays();

    for (unsigned i = 0; i < total_number_ways; i++) {
        const OSMWay* example = getWayByIndex(i);
        for (unsigned j = 0; j < getTagCount(example); ++j) {
            std::string Key, Value;
            std::tie(Key, Value) = getTagPair(example, j);
            if (Key == "highway") {
                if (Value == "motorway") {
                    OSMID target = example->id();
                    std::vector<StreetSegmentIndex> temp_level = FindTargetStreetSegment(target);
                    start_level.insert(std::end(start_level), std::begin(temp_level), std::end(temp_level));
                } else if (Value == "primary" || Value == "trunk") {
                    OSMID target = example->id();
                    std::vector<StreetSegmentIndex> temp_level = FindTargetStreetSegment(target);
                    block_level.insert(std::end(block_level), std::begin(temp_level), std::end(temp_level));
                } else if (Value == "secondary") {
                    OSMID target = example->id();
                    std::vector<StreetSegmentIndex> temp_level = FindTargetStreetSegment(target);
                    street_level.insert(std::end(street_level), std::begin(temp_level), std::end(temp_level));
                } else if (Value == "residential") {
                    OSMID target = example->id();
                    std::vector<StreetSegmentIndex> temp_level = FindTargetStreetSegment(target);
                    search_level.insert(std::end(search_level), std::begin(temp_level), std::end(temp_level));
                }
            }
        }
    }
    std::vector<StreetSegmentIndex> temp_level = FillStreetSegment();
    park_path.insert(std::end(park_path), std::begin(temp_level), std::end(temp_level));
    return true;
}

std::string Map::StreetToOSM(std::string name) {
    for (unsigned i = 0; i < name.size(); i++) {
        if (name[i] == '.') {
            name[i + 1] = 'o';
            name[i + 2] = 's';
            name[i + 3] = 'm';
            name[i + 4] = '.';
            name[i + 5] = 'b';
            name[i + 6] = 'i';
            name[i + 7] = 'n';
            name[i + 8] = '\0';
            name[i + 9] = '\0';
            name[i + 10] = '\0';
            name[i + 11] = '\n';
            break;
        }
    }
    return name;
}

std::vector<StreetSegmentIndex> Map::FindTargetStreetSegment(OSMID target) {
    std::vector<StreetSegmentIndex> level;
    for (unsigned i = 0; i < total_number_street_segment; i++) {
        if (street_segments_list[i].way_OSMID == target) {
            level.push_back(i);
            street_segments_list[i].isDrawn = true;
        }
    }
    return level;
}

std::vector<StreetSegmentIndex> Map::FillStreetSegment() {
    std::vector<StreetSegmentIndex> level;
    for (unsigned i = 0; i < total_number_street_segment; i++) {
        if (street_segments_list[i].isDrawn == false) {
            level.push_back(i);
        }
    }
    return level;
}

double Map::computeArea(unsigned vertex_count, std::vector<t_point>& vertex_list) const {
    double area = 0.0;
    int endIndex;

    for (unsigned i = 0; i < vertex_count; i++) {
        endIndex = (i + 1) % vertex_count;
        area = area + EARTH_RADIUS_IN_METERS * (vertex_list[i].x + vertex_list[endIndex].x)
                * EARTH_RADIUS_IN_METERS * (vertex_list[endIndex].y - vertex_list[i].y);
    }

    return 0.5 * fabs(area);
}

bool sortDescendArea(const feature_info& a, const feature_info& b) {
    return a.area > b.area;
}
