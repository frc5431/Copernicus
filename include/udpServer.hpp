#ifndef UDP_SERVER_H
#define UDP_SERVER_H


#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <boost/chrono.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/shared_ptr.hpp>
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <thread>
#include <chrono>
#include <iostream>

#define UDP_SLOCK boost::mutex::scoped_lock locks(udpLock)

using boost::asio::ip::udp;
using namespace std;
using namespace rapidjson;


class udp_server{
    public:
        udp_server();
        void setPressure(double);
        void setHighGear(bool);
        void setBottomIntake(bool);
        void setStream(bool);
        void setCrosshairOffset(double);
        void setTurretAngle(float);
        void setRPM(int);
        void setTopIntake(bool);
        void setLeftRPM(int);
        void setRightRPM(int);
        void setHoldsGear(int);
        void setMode(int);
        void setPowered(int);
        void createJson();
        void serverInit();
        void serverEnd();

	private:
		boost::asio::io_service io_service;
        thread serverThread;
        double pressure;
        bool highGear;
        bool bottomIntake;
        bool stream;
        double crosshairOffset;
        float turretAngle;
        int RPM;
        bool topIntake;
        int leftRPM;
        int rightRPM;
        bool holdsGear;
        int mode;
        bool powered;
        string finalString;
        rapidjson::Document document;
		boost::mutex udpLock;

		void start_recieve(){
            //cout<<"Waiting"<<endl;
            socket_.async_receive_from(
            boost::asio::buffer(recv_buffer_),remote_endpoint,
            boost::bind(&udp_server::handle_recieve,this,
            boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
			boost::this_thread::sleep_for(boost::chrono::milliseconds(30));
        }

        void handle_recieve(const boost::system::error_code& error,size_t){
            if(!error || error == boost::asio::error::message_size){
                boost::shared_ptr<string> message(new string (finalString));
                socket_.async_send_to(
                boost::asio::buffer(*message),remote_endpoint,
                boost::bind(&udp_server::handle_send,this,message,
                boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
                start_recieve();
            }
        }
        void handle_send(boost::shared_ptr<string>,
            const boost::system::error_code&, size_t){
        }

        udp::socket socket_;
        udp::endpoint remote_endpoint;
        array<char,1>recv_buffer_;

};

#endif


