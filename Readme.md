# WeatherForecast

WeatherForecast is a graphical application that fetches and displays weather data for various cities. The application uses the OpenWeatherMap API to retrieve current weather data and displays it using ImGui with OpenGL and GLFW.

## Features

- Select cities to fetch current weather data.
- Display weather information including temperature, humidity, wind speed, sunrise, and sunset times.
- Mark favorite cities and filter to show only favorite cities.
- Add new cities by name and validate them using the OpenWeatherMap API.
- Multi-threaded fetching of weather data to enhance performance.

## Prerequisites

- **C++ Compiler**: Ensure you have a C++ compiler installed (e.g., GCC, Clang, or MSVC).
- **CMake**: Make sure CMake is installed on your system.
- **GLFW**: Install the GLFW library.
- **GLEW**: Install the GLEW library.
- **OpenWeatherMap API Key**: Register and obtain an API key from [OpenWeatherMap](https://home.openweathermap.org/users/sign_up).

## Installation

### Clone the Repository

```bash
git clone https://github.com/yourusername/WeatherForecast.git
cd WeatherForecast
```

### Install Dependencies

You dont need to install dependencies, just run the solution file in VS community


### Create API Key File

1. Register on [OpenWeatherMap](https://home.openweathermap.org/users/sign_up) and create an API key.
2. Create a file named `key.txt` in the `assets` directory.
3. Paste your API key into the `key.txt` file.

## Running the Application

After building the project, you can run the application:

```bash
./WeatherForecast
```

## Usage

1. **Select Cities**: Use the checkboxes to select the cities for which you want to fetch weather data.
2. **Fetch Weather Data**: Click the "Fetch Weather Data" button to retrieve and display the weather information.
3. **Manage Favorites**:
   - Add to Favorites: Select cities and click "Add to Favorites".
   - Remove from Favorites: Select cities and click "Remove from Favorites".
   - Show Favorites: Toggle between showing all cities and only favorite cities by clicking "Show Favorites" or "Show All".
4. **Add New City**: Enter a city name in the input field and click "Add City" to add a new city to the list.
5. **Mark City**: Enter a city name in the input field and click "Mark" to select a city by name.

## Contributing

Contributions are welcome! Please fork the repository and submit a pull request for any features, enhancements, or bug fixes.

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.

## Acknowledgments

- [OpenWeatherMap](https://openweathermap.org/) for providing the weather data API.
- [GLFW](https://www.glfw.org/) for window management and input.
- [GLEW](http://glew.sourceforge.net/) for managing OpenGL extensions.
- [ImGui](https://github.com/ocornut/imgui) for the graphical user interface.
