#include <mjpgclient.h>
#include <iostream>
#include <networktables/NetworkTable.h>
#include <tables/ITableListener.h>
#include <perception.hpp>
#include <thread>
#include <Serial.hpp>
#include <MACE/MACE.h>

#define StayStill 0
#define DriveForward 1
#define RedLeft 2 
#define RedMiddle 3
#define RedRight 4

#define STRINGIFY(macro) #macro 

using namespace mc;

gfx::Group group = gfx::Group();

gfx::Text gearIn;
gfx::Text intakeRunning, intakeOff;
gfx::Image intake, camera;

gfx::ColorAttachment green, red, yellow;

os::Serial ser;

//MjpgClient client("http://10.54.31.25", 80, "mjpg/video.mjpg"); //Get cap line

std::shared_ptr<NetworkTable> table;

int currentAuton = StayStill;

gfx::Text autonText;


void getNextAuton() {
	currentAuton++;

	if( currentAuton >= 5 ) {
		currentAuton = 0;
	}

	if( currentAuton == StayStill ) {
		autonText.setText(L"Stay Still");
	} else if( currentAuton == DriveForward ) {
		autonText.setText(L"Drive Forward");
	} else if( currentAuton == RedLeft ) {
		autonText.setText(L"Red Left");
	} else if( currentAuton == RedMiddle ) {
		autonText.setText(L"Red Middle");
	} else if( currentAuton == RedRight ) {
		autonText.setText(L"Red Right");
	}

	table->PutNumber("autonSelect", currentAuton);
}

class AutonSelectButton: public gfx::Image {
public:
	bool clicked = false;

	void onInit() override {
		gfx::Image::onInit();
	}
	void onUpdate() override {
		gfx::Image::onUpdate();
	}//screw oliver
	void onRender() override {
		gfx::Image::onRender();
	}
	void onClean() override {
		gfx::Image::onClean();
	}

	void onDestroy() override {
		gfx::Image::onDestroy();
	};

	void onHover() override {
		gfx::Image::onHover();

		if(	os::Input::isKeyDown(os::Input::MOUSE_LEFT) ) {
			clicked = true;
		}

		if( clicked && os::Input::isKeyReleased(os::Input::MOUSE_LEFT) ) {
			clicked = false;
			
			getNextAuton();
		}
	}
};

AutonSelectButton autonSelect = AutonSelectButton();

void attemptSerialWrite(const std::string message) {
	if( ser.isConnected() && ser.isValid() ) {
		ser.write(message);
	} else {
		try {
			ser.init("//./COM10", 115200);
		} catch( ... ) {
			//ignore
		}
	}
}

namespace titan {
	void setTopIntake(const bool intaking, const bool reverse) {
		if (intaking) {
			if( reverse ) {
				//intake.setTexture(yellow);
				//intakeText.setText(L"INTAKING REVERSE");
			} else {
				//intake.setTexture(green);
				//intakeText.setText(L"INTAKING");
				intakeRunning.setProperty(gfx::Entity::DISABLED, false);
				intakeOff.setProperty(gfx::Entity::DISABLED, true);
			}
		}
		else {
			//intake.setTexture(red);
			//intakeText.setText(L"NOT INTAKING");
			intakeRunning.setProperty(gfx::Entity::DISABLED, true);
			intakeOff.setProperty(gfx::Entity::DISABLED, false);
		}
	}

	void setGearIn(const bool gear) {
		if (gear) {
			attemptSerialWrite("g");
			gearIn.setOpacity(1.0f);
		}
		else {
			attemptSerialWrite("o");

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

class TableListener: public ITableListener {
public:
	virtual void ValueChanged(ITable* source, llvm::StringRef key,
					  std::shared_ptr<nt::Value> value, bool isNew) override {
		if( key == "gearIn" ) {
			titan::setGearIn(value->GetBoolean());
		} else if( key == "intake" ) {
			titan::setTopIntake(value->GetBoolean(), false);
		} else if( key == "timeLeft" ) {
			if( value->GetDouble() > 120 ) {
				attemptSerialWrite("c");
			} else {
				attemptSerialWrite("s");
			}
		}
	}
};

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
	gearIn = gfx::Text("GEAR", f);
	gearIn.setTexture(Colors::GREEN);
	gearIn.setHorizontalAlign(gfx::HorizontalAlign::RIGHT);
	group.addChild(gearIn);
	
	f.setSize(24);
	autonText = gfx::Text(STRINGIFY(StayStill), f);
	autonText.setTexture(Colors::BLACK);
	autonText.setHorizontalAlign(gfx::HorizontalAlign::LEFT);
	group.addChild(autonText);

	autonSelect.setTexture(Colors::GRAY);
	autonSelect.setX(-0.9);
	autonSelect.setY(-0.2);
	autonSelect.setWidth(0.1f);
	autonSelect.setHeight(0.1f);
	group.addChild(autonSelect);

	red = gfx::ColorAttachment();
	red.init();
	red.load(Colors::RED);

	green = gfx::ColorAttachment();
	green.init();
	green.load(Colors::GREEN);

	yellow = gfx::ColorAttachment();
	yellow.init();
	yellow.load(Colors::YELLOW);

	intake = gfx::Image();
	intake.setTexture(red);
	intake.setWidth(1.0f);
	intake.setHeight(0.1f);
	intake.setY(0.9f);
	group.addChild(intake);

	intakeOff = gfx::Text(L"NOT INTAKING", f);
	intakeOff.setTexture(Colors::BLACK);
	intake.addChild(intakeOff);

	intakeRunning = gfx::Text(L"INTAKING", f);
	intakeRunning.setTexture(Colors::GREEN);
	intakeRunning.setProperty(gfx::Entity::DISABLED, true);
	intake.addChild(intakeRunning);
}

int main(int argc, char** argv) {
	try {
		NetworkTable::SetClientMode();
		NetworkTable::SetIPAddress("roborio-5431-frc.local");
		NetworkTable::SetTeam(5431);
		table = NetworkTable::GetTable("copernicus");

		TableListener listener = TableListener();
		table->AddTableListener(&listener);

		os::WindowModule win = os::WindowModule(720, 720, "Copernicus");
		win.setCreationCallback(&create);
		win.setFPS(30);
		win.setResizable(true);
		win.addChild(group);

		MACE::addModule(win);

		MACE::init();

		std::thread percepThread = std::thread([] () {
			cv::VideoCapture cap("http://roborio-5431-frc.local:1181/stream.mjpg");

			while( MACE::isRunning() ) {
				if( !cap.isOpened() ) {
					cap.open("http://roborio-5431-frc.local:1181/stream.mjpg");

					mc::os::wait(1000);
				} else {
					cap >> frame;

					mc::os::wait(33);

					perception::OnTurretFrame(frame);
				}
			}
		});

		while (MACE::isRunning()) {
			MACE::update();

			//titan::setGearIn(table->GetBoolean("gearIn", false));

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
