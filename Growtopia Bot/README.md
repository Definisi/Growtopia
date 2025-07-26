# Growtopia Bot

A modern C++17 bot for Growtopia with comprehensive features including networking, HTTP API, Discord integration, and automated gameplay.

## Features

- **Multi-client Support**: Manage multiple bot instances simultaneously
- **HTTP API**: RESTful API for bot control and monitoring
- **Discord Integration**: Rich Presence and webhook support
- **Lua Scripting**: Extensible automation through Lua scripts
- **Item Database**: Complete item management system
- **World Navigation**: Advanced pathfinding and movement
- **Inventory Management**: Automated item handling
- **Event System**: Comprehensive event handling and callbacks
- **Logging System**: Thread-safe logging with multiple levels
- **Modern C++**: Clean, maintainable code following best practices

## Requirements

### System Requirements
- Windows 10/11 (64-bit)
- Visual Studio 2019 or later (with C++17 support)
- CMake 3.15 or later
- Git

### Dependencies
- **ENet**: Networking library
- **cpprest**: HTTP client/server library
- **nlohmann/json**: JSON parsing
- **Lua**: Scripting engine
- **Discord RPC**: Discord Rich Presence

## Building

### Prerequisites

1. Install Visual Studio with C++ development tools
2. Install CMake from [cmake.org](https://cmake.org/download/)
3. Clone the repository:
   ```bash
   git clone <repository-url>
   cd "Growtopia Bot"
   ```

### Build Steps

1. **Configure the project:**
   ```bash
   mkdir build
   cd build
   cmake .. -G "Visual Studio 16 2019" -A x64
   ```

2. **Build the project:**
   ```bash
   cmake --build . --config Release
   ```

   Or open the generated solution file in Visual Studio and build from there.

3. **Install (optional):**
   ```bash
   cmake --install . --config Release
   ```

### Build Configurations

- **Debug**: Full debugging information, no optimizations
- **Release**: Optimized build for production use
- **RelWithDebInfo**: Optimized with debug information
- **MinSizeRel**: Optimized for minimum size

## Usage

### Basic Setup

1. **Prepare data files:**
   - Place `items.dat` in the `data/` directory
   - Configure settings in `data/config/`

2. **Run the bot:**
   ```bash
   ./GrowtopiaBot.exe
   ```

3. **Access the HTTP API:**
   - Default endpoint: `http://localhost:8080`
   - API documentation available at `/api/docs`

### Configuration

The bot uses various configuration files:

- **Network Settings**: Configure in `src/core/constants.hpp`
- **Logging**: Adjust log levels and output in the Logger class
- **Discord**: Set up Discord application ID for Rich Presence

### API Endpoints

- `GET /api/status` - Get bot status
- `POST /api/clients/add` - Add new client
- `POST /api/clients/{id}/connect` - Connect client
- `GET /api/clients/{id}/inventory` - Get client inventory
- `POST /api/clients/{id}/move` - Move client

## Development

### Code Structure

```
src/
├── core/           # Core utilities (constants, enums, logger)
├── api/            # Lua API bindings
├── client/         # Bot client implementation
├── server/         # HTTP server
├── utils/          # Utility functions
├── item/           # Item database and management
├── player/         # Player-related functionality
├── world/          # World and tile management
├── events/         # Event system
├── lapi/           # Lua API implementation
├── gui/            # GUI components (if any)
└── vendor/         # Third-party libraries
```

### Coding Standards

- **C++17 Standard**: Modern C++ features and best practices
- **Google Style Guide**: Code formatting with clang-format
- **Static Analysis**: clang-tidy for code quality
- **Documentation**: Doxygen-style comments
- **Error Handling**: RAII and exception safety
- **Thread Safety**: Proper synchronization where needed

### Code Quality Tools

1. **Format code:**
   ```bash
   cmake --build . --target format
   ```

2. **Run static analysis:**
   ```bash
   cmake --build . --target lint
   ```

3. **Generate documentation:**
   ```bash
   doxygen Doxyfile
   ```

### Contributing

1. Fork the repository
2. Create a feature branch
3. Follow the coding standards
4. Add tests for new functionality
5. Ensure all checks pass
6. Submit a pull request

## Architecture

### Core Components

- **Logger**: Thread-safe singleton logging system
- **Constants**: Centralized configuration values
- **Enums**: Type-safe enumeration classes
- **Client**: Main bot client with ENet networking
- **HttpServer**: RESTful API server using cpprest
- **DiscordManager**: Discord Rich Presence integration
- **ItemDatabase**: Item management and lookup
- **EventSystem**: Asynchronous event handling

### Design Patterns

- **Singleton**: Logger, ItemDatabase
- **Factory**: Client creation and management
- **Observer**: Event system
- **RAII**: Resource management
- **Strategy**: Different bot behaviors

## Troubleshooting

### Common Issues

1. **Build Errors:**
   - Ensure all dependencies are installed
   - Check CMake version compatibility
   - Verify Visual Studio C++17 support

2. **Runtime Errors:**
   - Check log files in `logs/` directory
   - Verify data files are present
   - Ensure network connectivity

3. **Performance Issues:**
   - Monitor CPU and memory usage
   - Check network latency
   - Review log levels (disable debug in production)

### Debug Mode

For debugging, build with Debug configuration:
```bash
cmake --build . --config Debug
```

This enables:
- Detailed logging
- Debug assertions
- Memory leak detection
- Performance profiling

## License

This project is licensed under the MIT License - see the LICENSE file for details.

## Acknowledgments

- ENet library for reliable UDP networking
- Microsoft cpprest for HTTP functionality
- nlohmann/json for JSON parsing
- Discord for Rich Presence API
- Lua for scripting capabilities

## Support

For support and questions:
- Check the documentation
- Review existing issues
- Create a new issue with detailed information
- Join our Discord community (if available)

---

**Note**: This bot is for educational purposes. Please respect the game's terms of service and use responsibly.