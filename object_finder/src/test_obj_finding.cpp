#include <fstream>

#include <boost/foreach.hpp>

#include "obj_finding.h"
#include "sphere_finding.h"

using namespace std;
using namespace Eigen;

void write(const vector<int> &dbg_image, int width, int height, string filename) {
    ofstream f(filename.c_str());
    int max_val = 0; for(int i = 0; i < width*height; i++) max_val = max(max_val, dbg_image[i]);
    f << "P2 " << width << " " << height << " " << max_val << "\n";
    for(int Y = 0; Y < height; Y++) {
        for(int X = 0; X < width; X++) {
            f << dbg_image[Y * width + X] << " ";
        }
        f << "\n";
    }
    f.close();
    cout << "Wrote to " << filename << endl;
}

int main() {
    int width=640, height=480;
    sensor_msgs::CameraInfo camera_info;
    sensor_msgs::Image image;
    image.width = camera_info.width = width;
    image.height = camera_info.height = height;
    image.encoding = "rgb8";
    image.data.resize(3 * width * height);
    
    double tmp[12] = {167.819926235456, 0.0, 349.5, 0.0, 0.0, 167.819926235456, 199.5, 0.0, 0.0, 0.0, 1.0, 0.0};
    for(int i = 0; i < 12; i++) camera_info.P[i] = tmp[i];
    
    TaggedImage img(image, camera_info, Affine3d::Identity());
    
    //write(dbg_image, width, height, "tmp.pgm");
    
    RenderBuffer rb(img);
    
    Obj obj = Obj("shooter.obj");
    BOOST_FOREACH(const Component &component, obj.components) {
        component.draw(rb, rb.new_region(), Vector3d(-1.5, 0, 2), Quaterniond(.5, .5, -.5, .5));
    }
    sphere_draw(rb, rb.new_region(), Vector3d(0, 0, 2), .3);
    sphere_draw(rb, rb.new_region(), Vector3d(1.5, 0, 2), .2);
    sphere_draw(rb, rb.new_region(), Vector3d(1.5, 0, 2), .3);
    
    vector<int> dbg_image(width*height, 0);
    rb.draw_debug_regions(dbg_image);
    
    vector<ResultWithArea> results = rb.get_results();
    
    BOOST_FOREACH(const Result &result, results) {
        cout << "region area: " << result.count << endl;
    }
    
    write(dbg_image, width, height, "out.pgm");
}
