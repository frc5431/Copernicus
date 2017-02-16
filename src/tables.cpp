#include "../include/tables.hpp"

//Standard includes
#include <memory>
#include <functional>

//NetworkTables include
//#include <networktables/NetworkTable.h>

//Boost includes
#include <boost/lexical_cast.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/chrono.hpp>
#include <boost/thread/mutex.hpp>

#include "../include/udpServer.hpp"
//#include "rapidjson/document.h"

#define PERCEPTION_TABLES_LOGTAG "TABLES"
#define PERCEPTION_TABLES_NAME "vision"
#define PERCEPTION_TABLES_PORT 5000

/*
NetworkTable::SetIPAddress("10.54.31.23");
*/

namespace Table {
	int x_val;
	int y_val;
	int dist_val;
	float horz_angle;
	float vert_angle;

	std::shared_ptr<NetworkTable> table;
	//boost::asio::io_service io;
	//boost::asio::ip::udp::socket socket(io);
	//boost::asio::ip::udp::endpoint endpoint(boost::asio::ip::address::from_string("127.0.0.1"),PERCEPTION_TABLES_PORT);

	//udp_server server;
	//void send(){	
		//std::array<int,5> turretAngle = {x_val,y_val,dist_val,horz_angle,vert_angle};
		//socket.open(boost::asio::ip::udp::v4());
		//socket.send_to(boost::asio::buffer(turretAngle),endpoint);
	//}

	void init() {
		/*NetworkTable::SetClientMode();
		NetworkTable::SetTeam(5431);

		std::cout << "hi";

		table = NetworkTable::GetTable("vision");*/

		boost::thread serveThread(boost::bind(&udp_server::serverInit, &server));

	//	socket = new boost::asio::ip::udp::socket(io, boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), PERCEPTION_TABLES_PORT));
	}

	void updateTarget(processing::Target final_target) {

		if(final_target.found) {
			x_val = final_target.x;
			y_val = final_target.y;
			dist_val = final_target.distance;
			horz_angle = final_target.horz_angle;
			vert_angle = final_target.vert_angle;
		} else {
			horz_angle = 666;
		}

		server.setTurretAngle(horz_angle);
		server.createJson();

		/*table->SetUpdateRate(0.01);
		table->PutNumber("x", x_val);
		table->PutNumber("y", y_val);
		table->PutNumber("distance", dist_val);
		table->PutNumber("horz_angle", table->GetNumber("horz_angle", 0.0) + horz_angle);
		table->PutNumber("vert_angle", vert_angle);*/
	}

	template<typename T>
	void _Logger(T toLog, bool err) {
		Logger::Log(PERCEPTION_LOG_TAG, toLog, err);
	}

}
