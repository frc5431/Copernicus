#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <thread>
#include <chrono>
#include <iostream>
#include <fstream>
#include <Copernicus.h>

class udp_server {
public:
	udp_server(boost::asio::io_service& io_service) :
		socket_(io_service, boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), 25565)) {
		start_send();
		start_recieve();
		//io_service.run();
	}
	void clientInit() {
		//thread clientThread(boost::bind(&asio::io_service::run, &io_service));
	}
	void clientEnd() {
		//clientThread.join();
	}
	std::ofstream ofs;
	std::ifstream ifs;
	rapidjson::Document document;
	double pressure;
	bool highGear;
	bool bottomIntake;
	bool stream;
	double crosshairOffset;
	double turretAngle;
	int RPM;
	bool topIntake;
	int leftRPM;
	int rightRPM;
	bool holdsGear;
	int mode;
	bool powered;
	std::string finalString;
	std::thread recieving;
	void SetSettings() {
		ifs.open("Settings.txt");
		std::string settingsJson;
		if (ifs.is_open()) {
			while (!ifs.eof()) {
				ifs >> settingsJson;
				document.Parse(settingsJson.c_str());
				if (document.IsObject()) {
					if (document.HasMember("pressure") && document.HasMember("highGear") && document.HasMember("bottomIntake") &&
						document.HasMember("stream") && document.HasMember("crosshairOffset") && document.HasMember("turretAngle") &&
						document.HasMember("RPM") && document.HasMember("topIntake") && document.HasMember("leftRPM") && document.HasMember("rightRPM") &&
						document.HasMember("holdsGear") && document.HasMember("mode") && document.HasMember("powered")) {
						if (document["pressure"].IsDouble() && document["highGear"].IsBool() && document["bottomIntake"].IsBool() &&
							document["stream"].IsBool() && document["crosshairOffset"].IsDouble() && document["turretAngle"].IsDouble() &&
							document["RPM"].IsInt() && document["topIntake"].IsBool() && document["leftRPM"].IsInt() && document["rightRPM"].IsInt() &&
							document["holdsGear"].IsBool() && document["mode"].IsInt() && document["powered"].IsBool()) {
							pressure = document["pressure"].GetDouble();
							highGear = document["highGear"].GetBool();
							bottomIntake = document["bottomIntake"].GetBool();
							stream = document["stream"].GetBool();
							crosshairOffset = document["crosshairOffset"].GetDouble();
							turretAngle = document["turretAngle"].GetDouble();
							RPM = document["RPM"].GetInt();
							topIntake = document["topIntake"].GetBool();
							leftRPM = document["leftRPM"].GetInt();
							rightRPM = document["rightRPM"].GetInt();
							holdsGear = document["holdsGear"].GetBool();
							mode = document["mode"].GetInt();
							powered = document["powered"].GetBool();
						}
					}
				}
			}
		}
	}
	boost::asio::io_service io_service;
private:
	void start_recieve() {
		socket_.async_receive_from(
			boost::asio::buffer(recv_buf), remote_endpoint,
			boost::bind(&udp_server::handle_recieve, this,
				boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
		std::string delimeter = "}";
		std::string firstString = recv_buf.data();
		finalString = firstString.substr(0, firstString.find(delimeter) + 1);
		document.Parse(finalString.c_str());
		if (document.IsObject()) {
			if (document.HasMember("pressure") && document.HasMember("highGear") && document.HasMember("bottomIntake") &&
				document.HasMember("stream") && document.HasMember("crosshairOffset") && document.HasMember("turretAngle") &&
				document.HasMember("RPM") && document.HasMember("topIntake") && document.HasMember("leftRPM") && document.HasMember("rightRPM") &&
				document.HasMember("holdsGear") && document.HasMember("mode") && document.HasMember("powered")) {
				if (document["pressure"].IsDouble() && document["highGear"].IsBool() && document["bottomIntake"].IsBool() &&
					document["stream"].IsBool() && document["crosshairOffset"].IsDouble() && document["turretAngle"].IsDouble() &&
					document["RPM"].IsInt() && document["topIntake"].IsBool() && document["leftRPM"].IsInt() && document["rightRPM"].IsInt() &&
					document["holdsGear"].IsBool() && document["mode"].IsInt() && document["powered"].IsBool()) {
					pressure = document["pressure"].GetDouble();
					highGear = document["highGear"].GetBool();
					bottomIntake = document["bottomIntake"].GetBool();
					stream = document["stream"].GetBool();
					crosshairOffset = document["crosshairOffset"].GetDouble();
					turretAngle = document["turretAngle"].GetDouble();
					RPM = document["RPM"].GetInt();
					topIntake = document["topIntake"].GetBool();
					leftRPM = document["leftRPM"].GetInt();
					rightRPM = document["rightRPM"].GetInt();
					holdsGear = document["holdsGear"].GetBool();
					mode = document["mode"].GetInt();
					powered = document["powered"].GetBool();

					titan::setFloorIntake(bottomIntake);
					titan::setFlywheelRPM(RPM);
					titan::setGearIn(holdsGear);
					titan::setHighGear(highGear);
					titan::setTopIntake(topIntake);
					titan::setTurretAngle(turretAngle);
					/*double pressure, bool highGear, bool bottomIntake,
					bool stream, double crosshairOffset, double turretAngle,
					int RPM, bool topIntake, int leftRPM, int rightRPM, bool holdsGear, int mode, bool powered*/
				}
			}
		}
	}
	void handle_recieve(const boost::system::error_code& error, size_t) {
		start_recieve();
	}
	void start_send() {
		boost::asio::ip::udp::resolver resolver(io_service);
		boost::asio::ip::udp::resolver::query query("10.54.31.23", "25566");
		boost::asio::ip::udp::endpoint reciever_endpoint = *resolver.resolve(query);
		boost::asio::ip::udp::socket socket(io_service);
		socket.open(boost::asio::ip::udp::v4());
		std::array<char, 1> send_buf = { 0 };
		boost::asio::ip::udp::endpoint sender_endpoint;
		boost::shared_ptr<std::string> message(new std::string("0"));
		socket_.async_send_to(
			boost::asio::buffer("0"), reciever_endpoint,
			boost::bind(&udp_server::handle_send, this,
				boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
	}
	void handle_send(const boost::system::error_code& error, size_t) {
		start_send();
	}
	boost::asio::ip::udp::socket socket_;
	boost::asio::ip::udp::endpoint remote_endpoint;
	std::array<char, 256>recv_buf;
	std::thread clientThread;
};