Certainly! Here's the `README.md` for your project:

```markdown
# Weather Forecast Project

## Table of Contents
- [Introduction](#introduction)
- [Features](#features)
- [Requirements](#requirements)
- [Installation](#installation)
- [Usage](#usage)
- [File Structure](#file-structure)
- [Contributing](#contributing)
- [License](#license)
- [Demonstration](#demonstration)

## Introduction
The Weather Forecast project is a C++ application that allows users to fetch and display weather data for various cities using the OpenWeatherMap API. The application features a graphical user interface built with ImGui and uses GLFW and GLEW for window and OpenGL context management.

## Features
- Fetch current weather data for selected cities.
- Add and remove favorite cities.
- Display detailed weather information including temperature, humidity, wind speed, sunrise, and sunset times.
- User-friendly graphical interface.

## Requirements
- C++ compiler with C++11 support
- CMake
- GLFW
- GLEW
- ImGui
- nlohmann/json library
- httplib library

## Installation

### Clone the Repository
```sh
git clone https://github.com/yourusername/WeatherForecast.git
cd WeatherForecast
```

### Build the Project
1. **Create a build directory**:
    ```sh
    mkdir build
    cd build
    ```

2. **Run CMake**:
    ```sh
    cmake ..
    ```

3. **Build the project**:
    ```sh
    make
    ```

### Setup API Key
1. **Create a file named `key.txt` inside the `assets` directory**:
    ```sh
    echo "your_api_key" > assets/key.txt
    ```

2. **Replace `your_api_key` with your actual OpenWeatherMap API key**.

## Usage
1. **Run the executable**:
    ```sh
    ./WeatherForecast
    ```

2. **Select cities and fetch weather data**.

3. **Use the controls to add/remove favorite cities and view weather details**.

## File Structure
```
WeatherForecast/
├── assets/                     # Directory for assets (e.g., API key file)
├── include/                    # Header files
│   └── WeatherForecast.h
├── src/                        # Source files
│   ├── WeatherForecast.cpp
│   └── main.cpp
├── CMakeLists.txt              # CMake build script
├── Readme.md                   # Project README
├── WeatherForecast.sln         # Visual Studio solution file
├── .gitignore                  # Git ignore file
└── .gitkeep                    # Git keep file for empty directories
```

## Demonstration


https://github.com/user-attachments/assets/0adf48c7-c789-4901-8c8b-60c470ac59d2


```
```
## Contributing
```

Contributions are welcome! Please follow these steps:
1. Fork the repository.
2. Create a new branch: `git checkout -b feature/your-feature-name`.
3. Make your changes.
4. Commit your changes: `git commit -m 'Add some feature'`.
5. Push to the branch: `git push origin feature/your-feature-name`.
6. Open a pull request.
```
## License
This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.
