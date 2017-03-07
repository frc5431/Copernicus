#include <mjpgclient.h>
#include <Copernicus.h>
#include <iostream>
#include <networktables/NetworkTable.h>
#include <perception.hpp>
#include <thread>
#include <Serial.hpp>
#include <MACE/MACE.h>

using namespace mc;

gfx::Group group = gfx::Group();

gfx::Text gearIn;
gfx::Text intakeText;
gfx::Image intake, camera;

//MjpgClient client("http://10.54.31.25", 80, "mjpg/video.mjpg"); //Get cap line

std::shared_ptr<NetworkTable> table;

namespace titan {
	void setTopIntake(const bool intaking, const bool reverse) {
		if (intaking) {
			if( reverse ) {
				intake.setTexture(Colors::YELLOW);
				intakeText.setText(L"INTAKING REVERSE");
			} else {
				intake.setTexture(Colors::GREEN);
				intakeText.setText(L"INTAKING");
			}
		}
		else {
			intake.setTexture(Colors::RED);
			intakeText.setText(L"INTAKING");
		}
	}

	void setGearIn(const bool gear) {
		if (gear) {
			gearIn.setOpacity(1.0f);
		}
		else {
			gearIn.setOpacity(0.0f);
		}
	}
}

cv::Mat frame;

//josef memes
class CameraPuller : public gfx::Entity2D {
public:
	gfx::ColorAttachment attachment;

	void onInit() override {
	}
	void onUpdate() override{
	}//screw oliver
	void onRender() override{
	}

	void onClean() override{
		if (!frame.empty()) {
			attachment.load(frame);
		} else {
			attachment.load(Colors::DARK_RED);
		}
	}

	void onDestroy() override{
	};
};

CameraPuller comp = CameraPuller();

void perception::OnTurretFrame(cv::Mat& m) {
	frame = m;
	comp.makeDirty();
}

void create() {
	camera = gfx::Image();
	camera.setWidth(1.0f);
	camera.setHeight(1.0f);
	group.addChild(camera);

	gfx::ColorAttachment c = gfx::ColorAttachment();
	c.init();
	c.load(Colors::DARK_RED);
	camera.setTexture(c);
	comp.attachment = c;
	group.addChild(comp);

	gfx::Font f = gfx::Font::loadFont(RESOURCE_FOLDER + std::string("/consola.ttf"));
	f.setSize(48);
	gearIn = gfx::Text("", f);
	gearIn.setTexture(Colors::GREEN);
	gearIn.setHorizontalAlign(gfx::HorizontalAlign::RIGHT);
	group.addChild(gearIn);

	intake = gfx::Image(Colors::WHITE);
	intake.setWidth(1.0f);
	intake.setHeight(0.1f);
	intake.setY(0.9f);
	group.addChild(intake);

	intakeText = gfx::Text(L"Loading..", f);
	intakeText.setTexture(Colors::BLACK);
	intake.addChild(intakeText);
}

int main(int argc, char** argv) {
	try {
		NetworkTable::SetClientMode();
		NetworkTable::SetTeam(5431);
		table = NetworkTable::GetTable("copernicus");

		os::WindowModule win = os::WindowModule(720, 720, "Copernicus");
		win.setCreationCallback(&create);
		win.setFPS(30);
		win.setResizable(true);
		win.addChild(group);

		MACE::addModule(win);

		MACE::init();

		std::thread percepThread = std::thread([] () {
			perception::startPerception();

			while( MACE::isRunning() ) {
				perception::pullLoop();
			}
		});

		while (MACE::isRunning()) {
			MACE::update();

			mc::os::wait(33);
		}

		MACE::destroy();

		percepThread.join();
	}
	catch (const std::exception& ex) {
		Error::handleError(ex);
	}

	return 0;
}
