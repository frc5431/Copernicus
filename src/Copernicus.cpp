#include <MACE/MACE.h>
#include <iostream>
#include <cpprest/http_client.h>
#include <cpprest/filestream.h>
#include <cpprest/rawptrstream.h>

using namespace utility;                    // Common utilities like string conversions
using namespace web;                        // Common features like URIs.
using namespace web::http;                  // Common HTTP functionality
using namespace web::http::client;          // HTTP client features
using namespace concurrency::streams;       // Asynchronous streams

int main(int argc, char** argv){
    char* bufferPtr = new char[1000];

    rawptr_buffer<char> buffer = rawptr_buffer<char>(bufferPtr,1000);

   // Create http_client to send the request.
    http_client client(U("http://webcam.st-malo.com/axis-cgi/mjpg/video.cgi"));

    // Build request URI and start the request.
    uri_builder builder(U(""));

    // Open stream to output file.
    pplx::task<void> requestTask = client.request(methods::GET, builder.to_string())

    // Handle response headers arriving.
    .then([&](http_response response)
    {
        if(response.status_code() != 200){
            throw mc::InitializationError("Recieved status code of "+response.status_code());
        }

        //i like head
        http_headers head = response.headers();
        std::string head_find = head["Content-Type"];

        const char* toFind = "boundary=";

        size_t pos = head_find.find(toFind);

        if(pos == std::string::npos) {
            throw mc::InitializationError("Invalid mjpeg stream header");
        }

        //the stickbug
        std::string boundary = head_find.substr(pos + sizeof(toFind)+1);

        return response.body().read_line(buffer);


        // Write response body into the file.
      //  return response.body().read_to_end(fileStream->streambuf());
    })

    .then([&](size_t size){
        for( mc::Index i = 0;i<size;++i){
            std::cout<<bufferPtr[i];
        }
        std::cout<<std::endl;
    });

    // Wait for all the outstanding I/O to complete and handle any exceptions
    try
    {
        requestTask.wait();
    }
    catch (const std::exception &e)
    {
        printf("Error exception:%s\n", e.what());
    }

    return 0;
}
