#pragma once
#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
#include <vector>

class my_camera
{
public:
	cv::String win_name;
	cv::VideoCapture* camera_capture;
	std::vector<cv::Mat> recorded_frames;
	int frame_w, frame_h;
	std::vector<cv::Point> click_points;

	void create_window(cv::String name);
	void destroy_window();
	void live_show();
	void save_frames_as_video(const char* file_name);
	static void on_mouse_click(int event, int x, int y, int flags, void* params);
};

struct click_mat_info {
	cv::Mat* img_mat;
	std::vector<cv::Point>* past_points;

	click_mat_info();
	click_mat_info(cv::Mat* _mat, std::vector<cv::Point>* _points);
};
