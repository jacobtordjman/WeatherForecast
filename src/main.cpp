//_________________________________________________Weather Forecast Project_________________________________________________________


// JACOB TURDJMAN - 208489484
// NIV MATITYAHU  - 209349364


//________________________________________________________Includes_________________________________________________________________

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <set>
#include <mutex>
#include <thread>
#include <atomic>
#include <json.hpp>
#include <httplib.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "imgui.h"
#include <imgui/backend/imgui_impl_glfw.h>
#include <imgui/backend/imgui_impl_opengl3.h>

//________________________________________________________Initalize_________________________________________________________________


// Constants: These define constant values used throughout the program.
const std::string api_key = "c327c78fff819312582e37d08f06ca87"; // API key for OpenWeatherMap
const std::string base_url = "http://api.openweathermap.org/data/2.5/weather"; // Base URL for OpenWeatherMap API
const std::string favorites_file = "favorites.txt"; // File name for storing favorite cities

// Struct Definition: Defines a data structure to hold information about a city.
struct City {
    std::string name;           // City name
    double lon;                 // Longitude
    double lat;                 // Latitude
    bool selected;              // Whether the city is selected
    nlohmann::json weatherData; // Weather data in JSON format
};

// Initial List of Cities: A vector holding some pre-defined cities.
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
std::mutex weatherDataMutex;         // Mutex to ensure thread-safe access to weather data
std::atomic<int> threadsFinished(0); // Atomic counter to keep track of finished threads

//________________________________________________________Functions_________________________________________________________________

// Function to Fetch Weather Data for a City
void fetchWeatherDataForCity(City& city) {
    // Create an HTTP client for the OpenWeatherMap API
    httplib::Client cli("http://api.openweathermap.org");
    // Formulate the URL for the API request
    std::string url = "/data/2.5/weather?lat=" + std::to_string(city.lat) + "&lon=" + std::to_string(city.lon) + "&appid=" + api_key;

    // Make the GET request
    auto res = cli.Get(url.c_str());
    if (res && res->status == 200) {
        // Parse the JSON response and update the city's weather data
        std::lock_guard<std::mutex> lock(weatherDataMutex); // Ensure thread-safe access
        city.weatherData = nlohmann::json::parse(res->body);
    }
    else {
        //std::cout << "respose: " << res->status <<std::endl;
        std::cerr << "Failed to fetch weather data for " << city.name << std::endl;
    }
    threadsFinished++; // Increment the counter for finished threads
}

// Function to Validate if a City Name is Valid
bool validateCity(const std::string& cityName, double& lon, double& lat) {
    // Create an HTTP client for the OpenWeatherMap API
    httplib::Client cli("http://api.openweathermap.org");
    // Formulate the URL for the API request
    std::string url = "/geo/1.0/direct?q=" + cityName + "&limit=1&appid=" + api_key;

    // Make the GET request
    auto res = cli.Get(url.c_str());
    if (res && res->status == 200) {
        // Parse the JSON response and extract the longitude and latitude
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
    std::ifstream infile(favorites_file); // Open the file for reading
    std::string city;
    while (std::getline(infile, city)) {
        double lon, lat;
        if (validateCity(city, lon, lat)) {
            favorites.insert(city); // Add to the set of favorite cities
            auto it = std::find_if(cities.begin(), cities.end(), [&city](const City& c) {return c.name == city; });
            if (it == cities.end())
                cities.push_back({ city, lon, lat, false, nullptr }); // Add to the list of cities
        }
    }
}

// Function to Save Favorite Cities to a File
void saveFavorites(const std::set<std::string>& favorites) {
    std::ofstream outfile(favorites_file); // Open the file for writing
    for (const auto& city : favorites) {
        outfile << city << std::endl; // Write each favorite city to the file
    }
}

// Function to Add Selected Cities to Favorites
void addFavorites(const std::vector<City>& cities, std::set<std::string>& favorites) {
    for (const auto& city : cities) {
        if (city.selected && favorites.find(city.name) == favorites.end()) {
            favorites.insert(city.name); // Add to the set of favorite cities
        }
    }
    saveFavorites(favorites); // Save the updated list to the file
}

// Function to Remove Selected Cities from Favorites
void removeFavorites(const std::vector<City>& cities, std::set<std::string>& favorites) {
    for (const auto& city : cities) {
        if (city.selected) {
            favorites.erase(city.name); // Remove from the set of favorite cities
        }
    }
    saveFavorites(favorites); // Save the updated list to the file
}

// Function to Filter and Return Only Favorite Cities
std::vector<City> filterFavorites(const std::vector<City>& cities, const std::set<std::string>& favorites) {
    std::vector<City> filteredCities;
    for (const auto& city : cities) {
        if (favorites.find(city.name) != favorites.end()) {
            filteredCities.push_back(city); // Add to the filtered list if it's a favorite
        }
    }
    return filteredCities;
}

// Function to Convert Unix Time to HH:MM Format
std::string unixToHHMM(int unixTime) {
    std::time_t t = unixTime;
    std::tm* tm = std::localtime(&t);
    char buffer[6];
    std::strftime(buffer, sizeof(buffer), "%H:%M", tm); // Format the time as HH:MM
    return std::string(buffer);
}

// Function to Uncheck All Cities
void uncheckAllCities(std::vector<City>& cities) {
    for (auto& city : cities) {
        city.selected = false; // Set selected to false for all cities
    }
}

// Function to Mark a City as Selected Based on Its Name
bool markCity(const std::string& cityName, std::vector<City>& cities) {
    for (auto& city : cities) {
        if (city.name == cityName) {
            city.selected = true; // Set selected to true if the name matches
            return true;
        }
    }
    return false;
}

//__________________________________________________________Main___________________________________________________________________

int main(int, char**) {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    // Create a Window with OpenGL Context
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Weather Forecast", NULL, NULL);
    if (window == NULL) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window); // Make the OpenGL context current
    glfwSwapInterval(1); // Enable vsync

    // Initialize GLEW
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

    // Initialize ImGui Context
    IMGUI_CHECKVERSION();  // Verifies that the header file version matches the compiled version of the ImGui library.
    ImGui::CreateContext();  // Creates an ImGui context.
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsLight();   // Sets the ImGui style to dark.

    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);  // Initializes ImGui for use with GLFW and OpenGL.
    ImGui_ImplOpenGL3_Init("#version 130");  // Initializes ImGui for use with OpenGL, specifying the GLSL version.

    // Flags and buffers for the main loop
    bool fetchWeather = false;
    bool showFavoritesOnly = false;
    std::vector<std::thread> threads;
    std::set<std::string> favorites;
    loadFavorites(cities, favorites); // Load favorite cities from file
    char cityNameBuffer[128] = ""; // Buffer for new city input
    char markCityBuffer[128] = ""; // Buffer for marking city input

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();  // Processes all pending events.

        // Start the ImGui frame
        ImGui_ImplOpenGL3_NewFrame();  // Starts a new ImGui frame for OpenGL.
        ImGui_ImplGlfw_NewFrame();  // Starts a new ImGui frame for GLFW.
        ImGui::NewFrame();  // Starts a new ImGui frame.

        // Retrieve the size of the GLFW window
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);  // Retrieves the size of the framebuffer of the specified window.

        // Fit ImGui window to GLFW window size
        ImGui::SetNextWindowPos(ImVec2(0, 0));  // Set the position of the next ImGui window to the top-left corner
        ImGui::SetNextWindowSize(ImVec2(static_cast<float>(display_w), static_cast<float>(display_h)));  // Set the size of the next ImGui window to match the GLFW window

        // Create ImGui window
        ImGui::Begin(" ", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);  // Begins a new ImGui window.

        // Left side: City selection
        ImGui::BeginChild("City Selection", ImVec2(ImGui::GetContentRegionAvail().x * 0.6f, 0), true);  // Begins a child window.
        ImGui::Text("Select cities to fetch weather data:");  // Displays text.
        for (auto& city : cities) {
            if (!showFavoritesOnly || (showFavoritesOnly && favorites.find(city.name) != favorites.end())) {
                ImGui::Checkbox(city.name.c_str(), &city.selected);  // Creates a checkbox for each city.
            }
        }
        ImGui::EndChild();  // Ends the child window.

        ImGui::SameLine();  // Places the next widget on the same line.
        ImGui::BeginChild("Controls", ImVec2(0, 0), true);  // Begins a child window for controls.

        ImVec2 buttonSize = ImVec2(ImGui::GetContentRegionAvail().x, 0);  // Define button size.

        if (ImGui::Button("Fetch Weather Data", buttonSize)) {  // Creates a button to fetch weather data.
            fetchWeather = true;
            threadsFinished = 0;
            for (auto& city : cities) {
                city.weatherData = nullptr;
            }
            for (auto& city : cities) {
                if (city.selected) {
                    threads.emplace_back(fetchWeatherDataForCity, std::ref(city));  // Fetch weather data for selected cities.
                }
            }
            uncheckAllCities(cities);  // Uncheck all cities after fetching data.
        }

        if (ImGui::Button("Add to Favorites", buttonSize)) {  // Creates a button to add selected cities to favorites.
            addFavorites(cities, favorites);
            uncheckAllCities(cities);  // Uncheck all cities after adding to favorites.
        }

        if (ImGui::Button("Remove from Favorites", buttonSize)) {  // Creates a button to remove selected cities from favorites.
            removeFavorites(cities, favorites);
            uncheckAllCities(cities);  // Uncheck all cities after removing from favorites.
        }

        if (ImGui::Button(showFavoritesOnly ? "Show All" : "Show Favorites", buttonSize)) {  // Creates a button to toggle between showing all cities and favorites.
            showFavoritesOnly = !showFavoritesOnly;
            uncheckAllCities(cities);  // Uncheck all cities after toggling view.
        }

        ImGui::PushItemWidth(buttonSize.x - ImGui::CalcTextSize("Add City").x - ImGui::GetStyle().ItemSpacing.x - 10);  // Sets the width of the next item.
        ImGui::InputText("##CityName", cityNameBuffer, sizeof(cityNameBuffer));  // Creates an input text box for new city input.
        ImGui::PopItemWidth();  // Reverts to the previous item width.
        ImGui::SameLine();  // Places the next widget on the same line.
        if (ImGui::Button("Add City", ImVec2(ImGui::GetContentRegionAvail().x, 0))) {  // Creates a button to add a new city.
            std::string cityName(cityNameBuffer);
            double lon, lat;
            if (validateCity(cityName, lon, lat)) {
                cities.push_back({ cityName, lon, lat, false, nullptr });
            }
            cityNameBuffer[0] = '\0'; // Clear the search bar
        }

        ImGui::PushItemWidth(buttonSize.x - ImGui::CalcTextSize("Mark").x - ImGui::GetStyle().ItemSpacing.x - 10);  // Sets the width of the next item.
        ImGui::InputText("##MarkCityName", markCityBuffer, sizeof(markCityBuffer));  // Creates an input text box for marking a city.
        ImGui::PopItemWidth();  // Reverts to the previous item width.
        ImGui::SameLine();  // Places the next widget on the same line.
        if (ImGui::Button("Mark", ImVec2(ImGui::GetContentRegionAvail().x, 0))) {  // Creates a button to mark a city.
            std::string cityName(markCityBuffer);
            markCity(cityName, cities);
            markCityBuffer[0] = '\0'; // Clear the search bar
        }

        ImGui::EndChild();  // Ends the child window for controls.

        // Fetching weather data in background threads
        if (fetchWeather) {
            if (threadsFinished == threads.size()) {
                for (auto& thread : threads) {
                    if (thread.joinable()) {
                        thread.join();  // Wait for all threads to finish.
                    }
                }
                threads.clear();
                fetchWeather = false;
            }
        }

        // Display weather data for cities
        for (auto& city : cities) {
            if (!city.weatherData.is_null()) {
                ImGui::Text("%s:", city.name.c_str());  // Displays city name.
                ImGui::Text("Weather: %s", city.weatherData["weather"][0]["description"].get<std::string>().c_str());  // Displays weather description.
                ImGui::Text("Temperature: %.2f°C", city.weatherData["main"]["temp"].get<double>() - 273.15);  // Displays temperature.
                ImGui::Text("Humidity: %d%%", city.weatherData["main"]["humidity"].get<int>());  // Displays humidity.
                ImGui::Text("Wind Speed: %.2f m/s", city.weatherData["wind"]["speed"].get<double>());  // Displays wind speed.
                ImGui::Text("Sunrise: %s", unixToHHMM(city.weatherData["sys"]["sunrise"].get<int>()).c_str());  // Displays sunrise time.
                ImGui::Text("Sunset: %s", unixToHHMM(city.weatherData["sys"]["sunset"].get<int>()).c_str());  // Displays sunset time.
                ImGui::Separator();  // Adds a separator line.
            }
        }

        ImGui::End();  // Ends the ImGui window.

        ImGui::Render();  // Renders the ImGui frame.
        glViewport(0, 0, display_w, display_h);  // Sets the viewport size.
        glClearColor(0.45f, 0.55f, 0.60f, 1.00f);  // Specifies the clear color for the color buffers.
        glClear(GL_COLOR_BUFFER_BIT);  // Clears buffers to preset values.
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());  // Renders ImGui draw data using OpenGL.

        glfwSwapBuffers(window);  // Swaps the front and back buffers of the window.
    }

    ImGui_ImplOpenGL3_Shutdown();  // Shuts down the ImGui OpenGL bindings.
    ImGui_ImplGlfw_Shutdown();  // Shuts down the ImGui GLFW bindings.
    ImGui::DestroyContext();  // Destroys the ImGui context.
    glfwDestroyWindow(window);  // Destroys the GLFW window.
    glfwTerminate();  // Cleans up and terminates the GLFW library.

    return 0;
}
