#include <MACE/MACE.h>
#include <iostream>
#include <mjpgclient.h>
#include <udpclient.hpp>
#include <Copernicus.h>

using namespace mc;

gfx::Group group = gfx::Group();

gfx::Group turretBase = gfx::Group();

gfx::ColorAttachment cameraTexture;

gfx::Text turretBaseText, driveGear;
gfx::Text gearIn;
gfx::Text flywheelRPMText, targetRPMText;
gfx::Text floorIntakeText, topIntakeText;
gfx::Image turretBaseAngle, floorIntake, topIntake, camera;

gfx::ProgressBar flywheelRPM, targetRPM;

namespace titan {
	void setTurretAngle(const float angle) {
		turretBaseAngle.getTransformation().rotation[2] = angle * (math::pi() / 180.0f);
		turretBaseText.setText(std::to_wstring(static_cast<int>(angle)));
	}

	void setHighGear(const bool highGear) {
		if (highGear) {
			driveGear.setText(L"H");
			driveGear.setTexture(Colors::RED);
		}
		else {
			driveGear.setText(L"L");
			driveGear.setTexture(Colors::GREEN);
		}
	}

	void setFloorIntake(const bool intaking) {
		if (intaking) {
			floorIntake.setTexture(Colors::GREEN);
			floorIntakeText.setText(L"FLOOR INTAKING");
		}
		else {
			floorIntake.setTexture(Colors::RED);
			floorIntakeText.setText(L"NOT INTAKING\nON FLOOR");
		}
	}

	void setTopIntake(const bool intaking) {
		if (intaking) {
			topIntake.setTexture(Colors::GREEN);
			topIntakeText.setText(L"TOP INTAKING");
		}
		else {
			topIntake.setTexture(Colors::RED);
			topIntakeText.setText(L"NOT INTAKING\nON TOP");
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

	void setFlywheelRPM(const float rpm) {
		flywheelRPM.setProgress(rpm);
		flywheelRPMText.setText(L"Flywheel: "+std::to_wstring(static_cast<int>(rpm)));
	}

	void setTargetRPM(const float rpm) {
		targetRPM.setProgress(rpm);
		targetRPMText.setText(L"Target: "+std::to_wstring(static_cast<int>(rpm)));
	}
}

void create() {
	cameraTexture.init();
	camera.setTexture(cameraTexture);
	group.addChild(camera);

	turretBaseAngle = gfx::Image(RESOURCE_FOLDER + std::string("/turretBase-overlay.png"));
	turretBase.addChild(turretBaseAngle);

	gfx::Font f = gfx::Font::loadFont(RESOURCE_FOLDER + std::string("/consola.ttf"));
	f.setSize(16);
	turretBaseText = gfx::Text(L"0°", f);
	turretBaseText.setTexture(Colors::GREEN);
	turretBaseText.setHorizontalAlign(gfx::HorizontalAlign::LEFT);
	turretBaseText.setVerticalAlign(gfx::VerticalAlign::TOP);
	turretBase.addChild(turretBaseText);

	f.setSize(64);
	driveGear = gfx::Text(L"Loading...", f);
	driveGear.setVerticalAlign(gfx::VerticalAlign::TOP);
	driveGear.setHorizontalAlign(gfx::HorizontalAlign::LEFT);
	group.addChild(driveGear);

	turretBase.setX(-0.8f);
	turretBase.setY(-0.8f);
	turretBase.setWidth(0.2f);
	turretBase.setHeight(0.2f);
	turretBase.setProperty(gfx::Entity::MAINTAIN_WIDTH, true);
	turretBase.setProperty(gfx::Entity::MAINTAIN_HEIGHT, true);

	group.addChild(turretBase);

	f.setSize(48);
	gearIn = gfx::Text(L"GEAR", f);
	gearIn.setTexture(Colors::GREEN);
	gearIn.setHorizontalAlign(gfx::HorizontalAlign::RIGHT);
	group.addChild(gearIn);

	floorIntake = gfx::Image(Colors::WHITE);
	floorIntake.setWidth(0.6f);
	floorIntake.setHeight(0.1f);
	floorIntake.setY(-0.9f);
	group.addChild(floorIntake);

	f.setSize(32);
	floorIntakeText = gfx::Text(L"Loading...", f);
	floorIntakeText.setTexture(Colors::WHITE);
	floorIntake.addChild(floorIntakeText);

	topIntake = gfx::Image(Colors::WHITE);
	topIntake.setWidth(0.6f);
	topIntake.setHeight(0.1f);
	topIntake.setY(0.9f);
	group.addChild(topIntake);

	topIntakeText = gfx::Text(L"Loading..", f);
	topIntakeText.setTexture(Colors::WHITE);
	topIntake.addChild(topIntakeText);

	gfx::ColorAttachment selectionTexture = gfx::ColorAttachment(RESOURCE_FOLDER + std::string("/progressBar.png"));

	flywheelRPM = gfx::ProgressBar(0, 4000, 0);
	flywheelRPM.setBackgroundTexture(Colors::LIGHT_GRAY);
	flywheelRPM.setForegroundTexture(Colors::GREEN);
	flywheelRPM.setSelectionTexture(selectionTexture);
	flywheelRPM.setWidth(0.4f);
	flywheelRPM.setHeight(0.1f);
	flywheelRPM.setX(-0.6f);
	flywheelRPM.setY(0.1f);
	group.addChild(flywheelRPM);

	flywheelRPMText = gfx::Text(L"Loading...", f);
	flywheelRPMText.setHorizontalAlign(gfx::HorizontalAlign::LEFT);
	flywheelRPM.addChild(flywheelRPMText);

	targetRPM = gfx::ProgressBar(0, 4000, 0);
	targetRPM.setBackgroundTexture(Colors::LIGHT_GRAY);
	targetRPM.setForegroundTexture(Colors::GREEN);
	targetRPM.setSelectionTexture(selectionTexture);
	targetRPM.setWidth(0.4f);
	targetRPM.setHeight(0.1f);
	targetRPM.setX(-0.6f);
	targetRPM.setY(-0.1f);
	group.addChild(targetRPM);

	targetRPMText = gfx::Text(L"Loading...", f);
	targetRPMText.setHorizontalAlign(gfx::HorizontalAlign::LEFT);
	targetRPM.addChild(targetRPMText);

	titan::setTurretAngle(0.0f);
	titan::setHighGear(false);
	titan::setFloorIntake(false);
	titan::setTopIntake(false);
	titan::setGearIn(true);
	titan::setFlywheelRPM(2000);
	titan::setTargetRPM(1200);
}

MjpgClient client("http://10.54.31.25", 80, "mjpg/video.mjpg"); //Get cap line

int main(int argc, char** argv) {
	try {
		/*std::thread async = std::thread([]() {
			boost::asio::io_service io_service;
			udp_server udp_Server(io_service);
			io_service.run();
		});*/
		os::WindowModule win = os::WindowModule(720, 720, "Copernicus");
		win.setCreationCallback(&create);
		win.setFPS(30);
		win.setResizable(true);
		win.addChild(group);

		MACE::addModule(win);

		MACE::init();

		//  client.setDiscDim(640, 480); //Set disconnected image size

		while (MACE::isRunning()) {
			MACE::update();


			cv::Mat curframe = client.getFrameMat(); //Test Mat (Use: getFrame for byte string)

			if (!curframe.empty() && curframe.size().width > 0 && curframe.size().height > 0) {
				std::cout << curframe.size() << std::endl;
		//		cameraTexture.setData(curframe.data, curframe.size().width, curframe.size().height, GL_BGR, GL_UNSIGNED_BYTE, GL_UNSIGNED_BYTE);
			}

			std::this_thread::sleep_for(std::chrono::milliseconds(33));
		}
		MACE::destroy();

		//async.detach();
	}
	catch (const std::exception& ex) {
		Exception::handleException(ex);
	}

	return 0;
}
