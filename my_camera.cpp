#include "my_camera.h"

void my_camera::create_window(cv::String name)
{
	cv::namedWindow(name);
	this->win_name = name;
}

void my_camera::destroy_window()
{
	cv::destroyWindow(this->win_name);
}

void my_camera::live_show()
{
	cv::Mat frame, flipped_frame;
	this->camera_capture = new cv::VideoCapture();
	this->camera_capture->open(0);
	if (!this->camera_capture->isOpened()) {
		std::cout << "Some error occured when opening camera!!" << std::endl;
		exit(1);
	}
	int64 begin = cv::getTickCount();
	double cpu_freq = cv::getTickFrequency();
	std::string title_tip = "press enter to exit";
	double pre_millsec = 0;
	bool is_paused = false;
	int64 pause_tm_step = 0;
	cv::Mat pause_img = cv::imread("pause.png");
	cv::Mat person_img = cv::imread("person.png");
	cv::Mat rec_img = cv::imread("rec.png");
	cv::setMouseCallback(this->win_name, &my_camera::on_mouse_click, reinterpret_cast<void*>(&(click_mat_info(&flipped_frame, &this->click_points))));
	while (true) {
		// pressing space key pause
		if (cv::waitKey(50) == 32) {
			if (!is_paused) {
				is_paused = true;
				pause_tm_step = cv::getTickCount();
				// show pause image at the center
				cv::Mat flipped_frame;
				cv::flip(frame, flipped_frame, 1);
				std::string title_time = (char*)"current elapse ";
				title_time += std::to_string(long((cv::getTickCount() - begin) / cpu_freq)) + " second(s)";
				cv::putText(flipped_frame, cv::String(title_tip), cv::Point(20, 10), \
					cv::HersheyFonts::FONT_HERSHEY_COMPLEX, 0.5, cv::Scalar(0, 0, 0, 0));
				cv::putText(flipped_frame, cv::String(title_time), cv::Point(20, 30), \
					cv::HersheyFonts::FONT_HERSHEY_COMPLEX, 0.5, cv::Scalar(0, 0, 0, 0));
				int height = flipped_frame.size().height, width = flipped_frame.size().width;
				int x_start = 120, y_start = 120;
				for (int i = x_start; i < pause_img.size().height + x_start; ++i) {
					for (int j = y_start; j < pause_img.size().width + y_start; ++j) {
						flipped_frame.at<cv::Vec3b>(i, j) = pause_img.at<cv::Vec3b>(i - x_start, j - y_start);
					}
				}
				// display person image
				x_start = 180;
				y_start = 400;
				for (int i = x_start; i < x_start + person_img.size().height; ++i) {
					for (int j = y_start; j < y_start + person_img.size().width; ++j) {
						flipped_frame.at<cv::Vec3b>(i, j) = person_img.at<cv::Vec3b>(i - x_start, j - y_start);
					}
				}
				cv::imshow(this->win_name, flipped_frame);
				continue;
			}
			else {
				is_paused = false;
				begin = cv::getTickCount() - (pause_tm_step - begin);
			}
		}
		if (is_paused) {
			continue;
		}
		this->camera_capture->read(frame);
		if (frame.empty()) {
			std::cout << "the frame captured is empty!!" << std::endl;
			break;
		}
		cv::flip(frame, flipped_frame, 1);
		std::string title_time = (char*)"current elapse ";
		title_time += std::to_string(long((cv::getTickCount() - begin) / cpu_freq)) + " second(s)";
		cv::putText(flipped_frame, cv::String(title_tip), cv::Point(20, 10), \
			cv::HersheyFonts::FONT_HERSHEY_COMPLEX, 0.5, cv::Scalar(0, 0, 0, 0));
		cv::putText(flipped_frame, cv::String(title_time), cv::Point(20, 30), \
			cv::HersheyFonts::FONT_HERSHEY_COMPLEX, 0.5, cv::Scalar(0, 0, 0, 0));
		// display the line by mouse
		if (this->click_points.size() >= 2) {
			for (int i = 0; i < this->click_points.size() - 1; ++i) {
				cv::line(flipped_frame, this->click_points[i], this->click_points[i + 1], cv::Scalar(0, 0, 255, 0));
			}
		}
		// display person image
		int x_start = 180, y_start = 400;
		for (int i = x_start; i < x_start + person_img.size().height; ++i) {
			for (int j = y_start; j < y_start + person_img.size().width; ++j) {
				flipped_frame.at<cv::Vec3b>(i, j) = person_img.at<cv::Vec3b>(i - x_start, j - y_start);
			}
		}
		//display rec image
		x_start = 50;
		y_start = 300;
		for (int i = x_start; i < x_start + rec_img.size().height; i++) {
			for (int j = y_start; j < y_start + rec_img.size().width; j++) {
				flipped_frame.at<cv::Vec3b>(i, j) = rec_img.at<cv::Vec3b>(i - x_start, j - y_start);
			}
		}
		cv::imshow(this->win_name, flipped_frame);
		double elapsed_millsec = (cv::getTickCount() - begin) / cpu_freq;
		// record frames
		if ((elapsed_millsec - pre_millsec) > 0.03) {
			this->recorded_frames.push_back(flipped_frame);
			pre_millsec = elapsed_millsec;
		}
		// pressing enter key, exit
		if (cv::waitKey(50) == 13) {
			this->frame_w = flipped_frame.size().width;
			this->frame_h = flipped_frame.size().height;
			break;
		}
	}
	this->camera_capture->release();
}

void my_camera::save_frames_as_video(const char* file_name)
{
	cv::VideoWriter* writer = new cv::VideoWriter(cv::String(file_name), \
		cv::VideoWriter::fourcc('D', 'I', 'V', 'X'), 24, cv::Size(this->frame_w, this->frame_h));
	if (!writer->isOpened()) {
		std::cout << "The video file fail to open!!" << std::endl;
		exit(1);
	}
	for (cv::Mat frame : this->recorded_frames) {
		writer->write(frame);
	}
	writer->release();
	delete writer;
}

void my_camera::on_mouse_click(int event, int x, int y, int flags, void* params)
{
	click_mat_info* click_info = reinterpret_cast<click_mat_info*>(params);
	cv::Mat* in_img = click_info->img_mat;
	std::vector<cv::Point>* points = click_info->past_points;
	if (event == CV_EVENT_LBUTTONDOWN) {
		std::cout << "the mouse position is: (" << x << ", " << y << ")" << std::endl;
		points->push_back(cv::Point(x, y));
	}
}

click_mat_info::click_mat_info()
{
	this->img_mat = nullptr;
	this->past_points = nullptr;
}

click_mat_info::click_mat_info(cv::Mat* _mat, std::vector<cv::Point>* _points)
{
	this->img_mat = _mat;
	this->past_points = _points;
}
