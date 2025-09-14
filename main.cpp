#include <iostream>
#include <cpr/cpr.h>

int main() {

    std::string target_url = "https://example.com";
    std::cout << "Attempting to fetch URL: " << target_url << std::endl;

    cpr::Response response = cpr::Get(cpr::Url{target_url});

    if(response.status_code == 200){
        
        std::cout << "Request successful! Status code: " << response.status_code << std::endl;
        
        std::cout << "--- Begin HTML Content ---" << std::endl;

        std::cout << response.text << std::endl;

        std::cout << "--- End HTML Content ---" << std::endl;
    }else{

        std::cout << "Failed to fetch the URL"<< response.status_code << std::endl;
    }

    if(response.error){
        std::cout << "Error: " << response.error.message << std::endl;
    }

    return 0;
}