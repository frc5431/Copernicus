#include <mjpgclient.h>
#include <Copernicus.h>
#include <iostream>
#include <networktables/NetworkTable.h>
#include <perception.hpp>
#include <thread>
#include <MACE/MACE.h>

using namespace mc;

gfx::Group group = gfx::Group();

gfx::Group turretBase = gfx::Group();

gfx::Text turretBaseText, driveGear;
gfx::Text gearIn;
gfx::Text flywheelRPMText, targetRPMText;
gfx::Text floorIntakeText, topIntakeText;
gfx::Image turretBaseAngle, floorIntake, topIntake, camera;

gfx::ProgressBar flywheelRPM, targetRPM;

//MjpgClient client("http://10.54.31.25", 80, "mjpg/video.mjpg"); //Get cap line

std::shared_ptr<NetworkTable> table;

namespace titan {
	void setTurretAngle(const float angle) {
		turretBaseAngle.getTransformation().rotation[2] = angle;
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
			floorIntakeText.setText(L"INTAKING");
		}
		else {
			floorIntake.setTexture(Colors::RED);
			floorIntakeText.setText(L"NOT INTAKING");
		}
	}

	void setTopIntake(const bool intaking) {
		if (intaking) {
			topIntake.setTexture(Colors::GREEN);
			topIntakeText.setText(L"FEEDING");
		}
		else {
			topIntake.setTexture(Colors::RED);
			topIntakeText.setText(L"NOT FEEDING");
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
		flywheelRPMText.setText(L"Flywheel: " + std::to_wstring(static_cast<int>(rpm)));
	}

	void setTargetRPM(const float rpm) {
		targetRPM.setProgress(rpm);
		targetRPMText.setText(L"Target: " + std::to_wstring(static_cast<int>(rpm)));
	}
}

cv::Mat frame;

//josef memes
class CameraComponent : public gfx::Component {
public:
	void init(gfx::Entity* e) {
		gfx::Image* en = dynamic_cast<gfx::Image*>(e);
		gfx::ColorAttachment c = gfx::ColorAttachment();
		c.init();
		c.load(Colors::DARK_RED);
		en->setTexture(c);
	}
	bool update(gfx::Entity* e) {
		//e->makeDirty();
		return false;
	}//screw oliver
	void render(gfx::Entity* e) {
		/*
		titan::setFloorIntake(table->GetBoolean("floorIntake", false));
		titan::setFlywheelRPM(table->GetNumber("flywheelRPM", 0.0));
		titan::setGearIn(table->GetBoolean("holdsGear", false));
		titan::setHighGear(table->GetBoolean("highGear", false));
		titan::setTargetRPM(table->GetNumber("targetRPM", 0.0));
		titan::setTopIntake(table->GetBoolean("topIntake", false));
		titan::setTurretAngle(table->GetNumber("turretAngle", 0.0));
		*/
	}

	void clean(gfx::Entity* e){
		gfx::Image* en = dynamic_cast<gfx::Image*>(e);

		if (!frame.empty()) {
			gfx::ColorAttachment& c = en->getTexture();
			c.load(frame);

			en->setTexture(c);
		}
	}

	void destroy(gfx::Entity*) {
	};
};

void perception::OnTurretFrame(cv::Mat& m) {
	frame = m;
	camera.makeDirty();
}

CameraComponent comp;

void create() {
	camera = gfx::Image();
	camera.setWidth(1.0f);
	camera.setHeight(1.0f);
	camera.addComponent(comp);
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
}

int main(int argc, char** argv) {
	try {
		/*
		NetworkTable::SetClientMode();
		NetworkTable::SetTeam(5431);
		table = NetworkTable::GetTable("copernicus");*/

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

				mc::os::wait(33);
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
