///**************************************************************************
//* @File: camera_alg.h
//* @Description: 查找相机算法定义
//* @Copyright: Copyright (c) 2016
//* @Company:
//* @WebSite:
//* @author 王冠
//* @Revision History
//*
//* <pre>
//* ----------------------------------------------------------------------
//*   Ver     Date       Who             Comments
//*  ----- ----------  --------  ---------------------------------------
//*   1.0  2016/04/21    王冠           初始化创建
//* ----------------------------------------------------------------------
//* </pre>
//**************************************************************************/

#ifndef _CAMERA_ALG_H_
#define _CAMERA_ALG_H_

#include <vector>
#include <map>
#include <cmath>
#include "camera_info_config.hpp"
#include "boost/shared_ptr.hpp"
#include "boost/make_shared.hpp"
#include "ZGXAPI.h"
#include "log_utils.h"
//add by sunhan 2013-5-2
#include "HIKIPCamera.h"


namespace zchx {
namespace camera_ctrl{


struct screen_param {
    int handle;
    int screen_id;
    int split_screen_id;
    std::string cam_ip;
    int cam_port;
    std::string cam_usrname;
    std::string cam_pass;

};


struct  cam_dist_t {
    double distance;
    boost::shared_ptr<camera_info_config> cam_info;
};



class camera_alg {
public:
    camera_alg();
    ~camera_alg();

    screen_param gen_turnto_param(camera_info_config& conf, std::string ip, int port);
    ptz_turnto_param_t gen_turnto_param(camera_info_config& conf, double dest_lat, double dest_lon);

    camera_info_config find_camera_info(double lat, double lon, const std::vector<camera_info_config>& cam_vec);
    //add by sun han
    camera_info_config find_camera_info_by_id(int id , const std::vector<camera_info_config>& cam_vec);
    bool do_turnto_action(camera_info_config& conf, ptz_turnto_param_t& turn_ptz_param, int cam_dotime);

	//add by sunhan camera_info_config 到 CAMERA_INFO 数据转换
	void  switchToCAMERA_INFO(camera_info_config& conf ,CAMERA_INFO * pInfo);
    bool  do_stop_hiki_action(camera_info_config& conf);

	bool  cmaera_is_valid(double lat, double lon , camera_info_config& conf);

    camera_info_config selected_cam_info;
    double dest_lat;
    double dest_lon;
    int track_num;
private:
    bool ptz_approach(CTS_PTZ current, CTS_PTZ aim);
private:
    double calc_distance(double point_posx, double point_posy, double cam_posx, double cam_posy);
    double to_rad(double deg);

    camera_info_config choice_cam();

    std::vector<cam_dist_t> distance_map;



};

bool comp(const cam_dist_t& cam_dist1, const cam_dist_t& cam_dist2);
}}




#endif
