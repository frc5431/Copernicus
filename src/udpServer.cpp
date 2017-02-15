#include "../include/udpServer.hpp"

		udp_server::udp_server():
            socket_(io_service,udp::endpoint(udp::v4(),25566)){
                start_recieve();
        }

        void udp_server::setPressure(double paramPressure){UDP_SLOCK; pressure = paramPressure;}
        void udp_server::setHighGear(bool paramHighGear){UDP_SLOCK; highGear = paramHighGear;}
        void udp_server::setBottomIntake(bool paramBottomIntake){UDP_SLOCK; bottomIntake = paramBottomIntake;}
        void udp_server::setStream(bool paramStream){UDP_SLOCK; stream = paramStream;}
        void udp_server::setCrosshairOffset(double paramCrosshairOffset){UDP_SLOCK; crosshairOffset = paramCrosshairOffset;}
        void udp_server::setTurretAngle(float paramTurretAngle){UDP_SLOCK; turretAngle = paramTurretAngle;}
        void udp_server::setRPM(int paramRPM){UDP_SLOCK; RPM = paramRPM;}
        void udp_server::setTopIntake(bool paramTopIntake){UDP_SLOCK; topIntake = paramTopIntake;}
        void udp_server::setLeftRPM(int paramLeftRPM){UDP_SLOCK; leftRPM = paramLeftRPM;}
        void udp_server::setRightRPM(int paramRightRPM){UDP_SLOCK; rightRPM = paramRightRPM;}
        void udp_server::setHoldsGear(int paramHoldsGear){UDP_SLOCK; holdsGear = paramHoldsGear;}
        void udp_server::setMode(int paramMode){UDP_SLOCK; mode = paramMode;}
        void udp_server::setPowered(int paramPowered){UDP_SLOCK; powered = paramPowered;}

		void udp_server::createJson(){
		   UDP_SLOCK;
           StringBuffer message;
           Writer<StringBuffer> writer(message);
           writer.StartObject();
           writer.Key("pressure");
           writer.Double(pressure);
           writer.Key("highGear");
           writer.Bool(highGear);
           writer.Key("bottomIntake");
           writer.Bool(bottomIntake);
           writer.Key("stream");
           writer.Bool(stream);
           writer.Key("crosshairOffset");
           writer.Double(crosshairOffset);
           writer.Key("turretAngle");
           writer.Double(turretAngle);
           writer.Key("RPM");
           writer.Int(RPM);
           writer.Key("topIntake");
           writer.Bool(topIntake);
           writer.Key("leftRPM");
           writer.Int(leftRPM);
           writer.Key("rightRPM");
           writer.Int(rightRPM);
           writer.Key("holdsGear");
           writer.Bool(holdsGear);
           writer.Key("mode");
           writer.Int(mode);
           writer.Key("powered");
           writer.Bool(powered);
           writer.EndObject();
           finalString = message.GetString();
           }

        void udp_server::serverInit(){
            //thread serverThread(boost::bind(&asio::io_service::run,&io_service));
			io_service.run();
		}

        void udp_server::serverEnd(){
            serverThread.join();
        }

/*
int main(){
    udp_server udp_server;
    udp_server.setPressure(1.2);
    udp_server.setHighGear(true);
    udp_server.setBottomIntake(false);
    udp_server.setStream(true);
    udp_server.setCrosshairOffset(3.4);
    udp_server.setTurretAngle(5.6);
    udp_server.setRPM(7);
    udp_server.setTopIntake(false);
    udp_server.setLeftRPM(8);
    udp_server.setRightRPM(9);
    udp_server.setHoldsGear(true);
    udp_server.setMode(10);
    udp_server.setPowered(11);
    udp_server.createJson();
    udp_server.serverInit();
    while(true){}
    udp_server.serverEnd();
   double pressure, bool highGear, bool bottomIntake,
    bool stream, double crosshairOffset, double turretAngle,
    int RPM, bool topIntake, int leftRPM, int rightRPM, bool holdsGear, int mode, bool powered
}*/
