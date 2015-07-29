#include <iostream>
#include <unistd.h>
#include "motor.h"
#include "location.h"

using namespace std;
using namespace cv;

int main() {
    Motor *motor = Motor::get_instance();
    VideoCapture capture1(1);
    Transform trans1(1);
    Location location(motor);
    location.add_camera(&capture1, &trans1);
    location.set_current_location(cv::Vec2d(0, 0), cv::Vec2d(0, 1));
    while (1) {
        Vec2d loc = location.get_location().first;
        cout << loc << endl;
        if (loc.dot(loc) > 1800 * 1800) {
            motor->stop();
            break;
        }
        motor->go(-2000, 0.4);
        usleep(50000);
    }
    Motor::destroy_instance();
    return 0;
}
