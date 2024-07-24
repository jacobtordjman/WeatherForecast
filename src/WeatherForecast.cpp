// WeatherForecast.cpp

#include "WeatherForecast.h"

// Constants: These define constant values used throughout the program.
const std::string base_url = "http://api.openweathermap.org/data/2.5/weather";
const std::string favorites_file = "favorites.txt";
std::string api_key;

// Initial List of Cities
std::vector<City> cities = {
    {"New York", -74.0060, 40.7128, false, nullptr},
    {"Los Angeles", -118.2437, 34.0522, false, nullptr},
    {"Tel Aviv", 34.7818, 32.0853, false, nullptr},
    {"Madrid", -3.7038, 40.4168, false, nullptr},
    {"Moscow", 37.6173, 55.7558, false, nullptr},
    {"London", -0.1276, 51.5074, false, nullptr},
    {"Paris", 2.3522, 48.8566, false, nullptr},
    {"Berlin", 13.4050, 52.5200, false, nullptr},
    {"Tokyo", 139.6917, 35.6895, false, nullptr},
    {"Sydney", 151.2093, -33.8688, false, nullptr},
    {"Bangkok", 100.5018, 13.7563, false, nullptr}
};

// Global Variables for Threading
std::mutex weatherDataMutex;
std::atomic<int> threadsFinished(0);

// Function to Read API Key from File
std::string readApiKeyFromFile(const std::string& filePath) {
    std::ifstream keyFile(filePath);
    std::string key;
    if (keyFile.is_open()) {
        std::getline(keyFile, key);
        keyFile.close();
    }
    else {
        std::cerr << "Unable to open API key file: " << filePath << std::endl;
        exit(EXIT_FAILURE); // Exit if API key cannot be read
    }
    return key;
}

// Function to Fetch Weather Data for a City
void fetchWeatherDataForCity(City& city) {
    httplib::Client cli("http://api.openweathermap.org");
    std::string url = "/data/2.5/weather?lat=" + std::to_string(city.lat) + "&lon=" + std::to_string(city.lon) + "&appid=" + api_key;

    auto res = cli.Get(url.c_str());
    if (res && res->status == 200) {
        std::lock_guard<std::mutex> lock(weatherDataMutex);
        city.weatherData = nlohmann::json::parse(res->body);
    }
    else {
        std::cerr << "Failed to fetch weather data for " << city.name << std::endl;
    }
    threadsFinished++;
}

// Function to Validate if a City Name is Valid
bool validateCity(const std::string& cityName, double& lon, double& lat) {
    httplib::Client cli("http://api.openweathermap.org");
    std::string url = "/geo/1.0/direct?q=" + cityName + "&limit=1&appid=" + api_key;

    auto res = cli.Get(url.c_str());
    if (res && res->status == 200) {
        auto data = nlohmann::json::parse(res->body);
        if (!data.empty()) {
            lon = data[0]["lon"];
            lat = data[0]["lat"];
            return true;
        }
    }
    return false;
}

// Function to Load Favorite Cities from a File
void loadFavorites(std::vector<City>& cities, std::set<std::string>& favorites) {
    std::ifstream infile(favorites_file);
    std::string city;
    while (std::getline(infile, city)) {
        double lon, lat;
        if (validateCity(city, lon, lat)) {
            favorites.insert(city);
            auto it = std::find_if(cities.begin(), cities.end(), [&city](const City& c) { return c.name == city; });
            if (it == cities.end())
                cities.push_back({ city, lon, lat, false, nullptr });
        }
    }
}

// Function to Save Favorite Cities to a File
void saveFavorites(const std::set<std::string>& favorites) {
    std::ofstream outfile(favorites_file);
    for (const auto& city : favorites) {
        outfile << city << std::endl;
    }
}

// Function to Add Selected Cities to Favorites
void addFavorites(const std::vector<City>& cities, std::set<std::string>& favorites) {
    for (const auto& city : cities) {
        if (city.selected && favorites.find(city.name) == favorites.end()) {
            favorites.insert(city.name);
        }
    }
    saveFavorites(favorites);
}

// Function to Remove Selected Cities from Favorites
void removeFavorites(const std::vector<City>& cities, std::set<std::string>& favorites) {
    for (const auto& city : cities) {
        if (city.selected) {
            favorites.erase(city.name);
        }
    }
    saveFavorites(favorites);
}

// Function to Filter and Return Only Favorite Cities
std::vector<City> filterFavorites(const std::vector<City>& cities, const std::set<std::string>& favorites) {
    std::vector<City> filteredCities;
    for (const auto& city : cities) {
        if (favorites.find(city.name) != favorites.end()) {
            filteredCities.push_back(city);
        }
    }
    return filteredCities;
}

// Function to Convert Unix Time to HH:MM Format
std::string unixToHHMM(int unixTime) {
    std::time_t t = unixTime;
    std::tm* tm = std::localtime(&t);
    char buffer[6];
    std::strftime(buffer, sizeof(buffer), "%H:%M", tm);
    return std::string(buffer);
}

// Function to Uncheck All Cities
void uncheckAllCities(std::vector<City>& cities) {
    for (auto& city : cities) {
        city.selected = false;
    }
}

// Function to Mark a City as Selected Based on Its Name
bool markCity(const std::string& cityName, std::vector<City>& cities) {
    for (auto& city : cities) {
        if (city.name == cityName) {
            city.selected = true;
            return true;
        }
    }
    return false;
}
