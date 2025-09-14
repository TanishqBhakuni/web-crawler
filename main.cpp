#include <iostream>
#include <cpr/cpr.h>

int main() {

    std::string target_url = "https://indianexpress.com/article/business/companies/infosys-buyback-shareholders-benefit-10247117/";
    std::cout << "Attempting to fetch URL: " << target_url << std::endl;

    cpr::Response response = cpr::Get(cpr::Url{target_url});

    if(response.status_code == 200){
        std::cout << "Successfully fetched the URL"<<response.status_code << std::endl;
    }else{
        std::cout << "Failed to fetch the URL"<<response.status_code << std::endl;
    }
    return 0;
}