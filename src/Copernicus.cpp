#include <MACE/MACE.h>
#include <iostream>
#include <mjpgclient.h>

using namespace mc;

gfx::Group group = gfx::Group();

gfx::Group turretBase = gfx::Group();

gfx::Text turretBaseDegrees;
gfx::Image turretBaseAngle;

float angle = 0;

class TurretBaseComponent: public gfx::Component {
	void init(gfx::Entity* e) {};
	bool update(gfx::Entity* e) {
		if( os::Input::isKeyDown(os::Input::UP) ) {
            ++angle;
			e->getTransformation().rotation[2] = angle * (math::pi() / 180.0f);
            turretBaseDegrees.setText(std::to_wstring(static_cast<int>(angle))+L"°");
		} else if( os::Input::isKeyDown(os::Input::DOWN) ) {
            --angle;
			e->getTransformation().rotation[2] = angle * (math::pi() / 180.0f);
			turretBaseDegrees.setText(std::to_wstring(static_cast<int>(angle))+L"°");
		}
		return false;
	};
	void render(gfx::Entity* e) {};
	void destroy(gfx::Entity* e) {};
};

TurretBaseComponent comp = TurretBaseComponent();

void create(){
    turretBaseAngle = gfx::Image(RESOURCE_FOLDER + std::string("/turretBase-overlay.png"));
    turretBaseAngle.addComponent(comp);

    turretBase.addChild(turretBaseAngle);

    gfx::Font f = gfx::Font::loadFont(RESOURCE_FOLDER + std::string("/consola.ttf"));
    f.setSize(24);
    turretBaseDegrees = gfx::Text(L"0°", f);
    turretBaseDegrees.setHorizontalAlign(gfx::HorizontalAlign::LEFT);
    turretBaseDegrees.setVerticalAlign(gfx::VerticalAlign::TOP);
    turretBase.addChild(turretBaseDegrees);

    turretBase.setX(-0.8f);
    turretBase.setY(-0.8f);
    turretBase.setWidth(0.4f);
    turretBase.setHeight(0.4f);

    group.addChild(turretBase);
}

const char ip[] = "http://webcam.st-malo.com"; //Ip base of stream
int port = 80; //Port of stream
const char name[] = "axis-cgi/mjpg/video.cgi"; //extension

MjpgClient client(ip, port, name); //Get cap line

int main(int argc, char** argv){
    os::WindowModule win = os::WindowModule(500, 500, "Copernicus");
    win.setCreationCallback(&create);

    win.addChild(group);

    MACE::addModule(win);

    MACE::init();

    client.setDiscDim(640, 480); //Set disconnected image size

    while(MACE::isRunning()){
        MACE::update();

        cv::Mat curframe = client.getFrameMat(); //Test Mat (Use: getFrame for byte string)

        std::cout << curframe.size() << std::endl;

        std::this_thread::sleep_for(std::chrono::milliseconds(33));
    }
    MACE::destroy();

    return 0;
}
