#include "location.h"
#include <iostream>
#include <cmath>
#include <stdexcept>

using namespace std;
using namespace cv;

Location::Location(Motor *motor): motor(motor) { }

void Location::add_camera(VideoCapture *capture, Transform *trans) {
    v_capture.push_back(capture);
    v_vision.push_back(new Vision(cvRound(capture->get(CAP_PROP_FRAME_HEIGHT)),
                                  cvRound(capture->get(CAP_PROP_FRAME_WIDTH)), trans));
}

std::pair<cv::Vec2d, cv::Vec2d> Location::get_location() {
    vector<pair<Vec2d, Vec2d> > v_delta = motor->get_delta();
    for (auto it = v_delta.begin(); it != v_delta.end(); it++) {
        Vec2d off = it->first, delta = it->second;
        Vec2d n(direction[1], -direction[0]);
        position += off[0] * n + off[1] * direction;
        direction = delta[0] * n + delta[1] * direction;
        direction /= sqrt(direction.dot(direction));
    }
    try_vision_correct();
    return make_pair(position, direction);
}

void Location::set_current_location(cv::Vec2d position, cv::Vec2d direction) {
    this->position = position;
    double m = direction.dot(direction);
    if (m <= 0)
        throw invalid_argument("Location::set_current_location: direction == Vec2d(0.0, 0.0)");
    direction /= sqrt(m);
    this->direction = direction;
}

void Location::try_vision_correct() {
    for (size_t i = 0; i < v_capture.size(); i++) {
        Mat frame;
        *v_capture[i] >> frame;
        v_vision[i]->input(frame);
        Vec2f pos;
        Vec2f direct;
        double location_confidence;
        v_vision[i]->get_location(pos, direct, location_confidence);
        if (location_confidence > 0.5) {
            if (direct.dot(direction) < 0) {
                direct = -direct;
                pos = Vec2f(900 - pos[0], 2200 - pos[1]);
            }
            position = pos;
            direction = direct;
            break;
        }
    }
}

float Location::get_radius() {
}
