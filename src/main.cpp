// main.cpp

#include "WeatherForecast.h"

/**
 * @brief Main function initializes the GUI, handles the main loop, and cleans up resources.
 *
 * Initializes the GLFW library and creates a window with an OpenGL context.
 * Sets up ImGui for creating a graphical user interface and handles the main loop
 * where user inputs are processed, weather data is fetched, and the UI is updated.
 * Cleans up resources before exiting.
 *
 * @return int Returns 0 on successful execution, -1 on failure.
 */
int main(int, char**) {
    // Initialize GLFW library
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    // Create a GLFW window with OpenGL context
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Weather Forecast", NULL, NULL);
    if (window == NULL) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window); // Make the OpenGL context current
    glfwSwapInterval(1); // Enable vsync

    // Initialize GLEW library
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

    // Initialize ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsLight();

    // Setup ImGui binding for GLFW and OpenGL
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    // Read API key from file
    api_key = readApiKeyFromFile("assets/key.txt");

    // Variables to manage application state
    bool fetchWeather = false;
    bool showFavoritesOnly = false;
    std::vector<std::thread> threads;
    std::set<std::string> favorites;
    loadFavorites(cities, favorites); // Load favorite cities from file
    char cityNameBuffer[128] = ""; // Buffer for new city input
    char markCityBuffer[128] = ""; // Buffer for marking city input

    // Main application loop
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents(); // Process all pending events

        // Start a new ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Retrieve the size of the GLFW window
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);

        // Fit ImGui window to GLFW window size
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImVec2(static_cast<float>(display_w), static_cast<float>(display_h)));

        // Create ImGui window
        ImGui::Begin("Weather Forecast", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

        // Left side: City selection
        ImGui::BeginChild("City Selection", ImVec2(ImGui::GetContentRegionAvail().x * 0.6f, 0), true);
        ImGui::Text("Select cities to fetch weather data:");
        for (auto& city : cities) {
            if (!showFavoritesOnly || (showFavoritesOnly && favorites.find(city.name) != favorites.end())) {
                ImGui::Checkbox(city.name.c_str(), &city.selected);
            }
        }
        ImGui::EndChild();

        // Right side: Controls
        ImGui::SameLine();
        ImGui::BeginChild("Controls", ImVec2(0, 0), true);

        ImVec2 buttonSize = ImVec2(ImGui::GetContentRegionAvail().x, 0);

        // Button to fetch weather data for selected cities
        if (ImGui::Button("Fetch Weather Data", buttonSize)) {
            fetchWeather = true;
            threadsFinished = 0;
            for (auto& city : cities) {
                city.weatherData = nullptr; // Clear previous weather data
            }
            for (auto& city : cities) {
                if (city.selected) {
                    threads.emplace_back(fetchWeatherDataForCity, std::ref(city)); // Fetch weather data in separate threads
                }
            }
            uncheckAllCities(cities); // Uncheck all cities after fetching data
        }

        // Button to add selected cities to favorites
        if (ImGui::Button("Add to Favorites", buttonSize)) {
            addFavorites(cities, favorites);
            uncheckAllCities(cities); // Uncheck all cities after adding to favorites
        }

        // Button to remove selected cities from favorites
        if (ImGui::Button("Remove from Favorites", buttonSize)) {
            removeFavorites(cities, favorites);
            uncheckAllCities(cities); // Uncheck all cities after removing from favorites
        }

        // Button to toggle between showing all cities and favorite cities only
        if (ImGui::Button(showFavoritesOnly ? "Show All" : "Show Favorites", buttonSize)) {
            showFavoritesOnly = !showFavoritesOnly;
            uncheckAllCities(cities); // Uncheck all cities after toggling view
        }

        // Input field to add a new city
        ImGui::PushItemWidth(buttonSize.x - ImGui::CalcTextSize("Add City").x - ImGui::GetStyle().ItemSpacing.x - 10);
        ImGui::InputText("##CityName", cityNameBuffer, sizeof(cityNameBuffer));
        ImGui::PopItemWidth();
        ImGui::SameLine();
        if (ImGui::Button("Add City", ImVec2(ImGui::GetContentRegionAvail().x, 0))) {
            std::string cityName(cityNameBuffer);
            double lon, lat;
            if (validateCity(cityName, lon, lat)) {
                cities.push_back({ cityName, lon, lat, false, nullptr });
            }
            cityNameBuffer[0] = '\0'; // Clear the input field
        }

        // Input field to mark a city as selected
        ImGui::PushItemWidth(buttonSize.x - ImGui::CalcTextSize("Mark").x - ImGui::GetStyle().ItemSpacing.x - 10);
        ImGui::InputText("##MarkCityName", markCityBuffer, sizeof(markCityBuffer));
        ImGui::PopItemWidth();
        ImGui::SameLine();
        if (ImGui::Button("Mark", ImVec2(ImGui::GetContentRegionAvail().x, 0))) {
            std::string cityName(markCityBuffer);
            markCity(cityName, cities);
            markCityBuffer[0] = '\0'; // Clear the input field
        }

        ImGui::EndChild(); // End the controls child window

        // Handle fetching weather data in background threads
        if (fetchWeather) {
            if (threadsFinished == threads.size()) {
                for (auto& thread : threads) {
                    if (thread.joinable()) {
                        thread.join(); // Wait for all threads to finish
                    }
                }
                threads.clear();
                fetchWeather = false;
            }
        }

        // Display weather data for cities
        for (auto& city : cities) {
            if (!city.weatherData.is_null()) {
                ImGui::Text("%s:", city.name.c_str());
                ImGui::Text("Weather: %s", city.weatherData["weather"][0]["description"].get<std::string>().c_str());
                ImGui::Text("Temperature: %.2f°C", city.weatherData["main"]["temp"].get<double>() - 273.15);
                ImGui::Text("Humidity: %d%%", city.weatherData["main"]["humidity"].get<int>());
                ImGui::Text("Wind Speed: %.2f m/s", city.weatherData["wind"]["speed"].get<double>());
                ImGui::Text("Sunrise: %s", unixToHHMM(city.weatherData["sys"]["sunrise"].get<int>()).c_str());
                ImGui::Text("Sunset: %s", unixToHHMM(city.weatherData["sys"]["sunset"].get<int>()).c_str());
                ImGui::Separator();
            }
        }

        ImGui::End(); // End the main window

        // Render the ImGui frame
        ImGui::Render();
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window); // Swap front and back buffers
    }

    // Clean up and terminate the application
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
