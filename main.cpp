#include "my_camera.h"

int main()
{
	// enter key 13, space key 32
	my_camera* camera = new my_camera();
	camera->create_window("camera");
	camera->live_show();
	camera->destroy_window();
	camera->save_frames_as_video("my_video.avi");
}