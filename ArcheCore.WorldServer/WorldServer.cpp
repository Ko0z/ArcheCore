#include "WorldServer.h"
#include "StreamServer.h"
#include "Managers/CharacterManager.h"
#include "Managers/SqLiteManager.h"
#include "Managers/FactionManager.h"
#include "Managers/ItemManager.h"
#include "Managers/OpCodeManager.h"
#include "Managers/SkillManager.h"
#include "Managers/ZoneManager.h"
#include "Managers/LocalizationManager.h"
#include <spdlog/spdlog.h>
#include <random>

#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>

int main(int argc, char* argv[])
{
	spdlog::info("ArcheCore server spinning up...");

	// JSON Config file
	std::filesystem::path path = std::filesystem::current_path();
	std::filesystem::path full_path = std::filesystem::absolute(path);
	// Will make the path string to platform specific format?
	std::filesystem::path preferred_path = full_path.make_preferred();
	std::string real_path_string = preferred_path.string();

	std::replace(real_path_string.begin(), real_path_string.end(), '\\', '/');
	std::string config_string = "/Server.cfg";

	std::string config_path = real_path_string + config_string;

	spdlog::info("World Server Config Path: {}", config_path);

	// JSON stuff
	std::ifstream f((config_path));
	nlohmann::json jsonData = nlohmann::json::parse(f);
	if (jsonData.empty()) {
		std::cout << "JSON ERROR: data is empty!" << std::endl;
		std::cin.get();
		return 0;
	}
	// ----------------------->

	OpCodes::Instance().BuildOpCodeMap();

	WorldManager& worldManager = WorldManager::Instance();
	worldManager.Initialize();

	/* Initialize all systems and prepare everything */
	if (SqLiteManager::Instance().Initialize() == false) 
		return 0;
	
	LocalizationManager::Instance().Initialize();
	FactionManager::Instance().Initialize();
	ItemManager::Instance().Initialize();
	SkillManager::Instance().Initialize();
	CharacterManager::Instance().Load();
	ZoneManager::Instance().Initialize();

	int worldServerPort = jsonData["GAME-SERVER-PORT"];
	int streamServerPort = jsonData["STREAM-SERVER-PORT"];
	
	/* Start the actual server, that listen for packets etc */
	WorldServer worldServer(worldServerPort);
	StreamServer streamServer(streamServerPort);
	
	// Initialize the clock
	auto clock = std::chrono::high_resolution_clock();

	// Running server on 10 FPS for testing, prob want to run it a bit more frequent..
	const double FPS = 10;
	const double MIN_FPS = ((double)1 / (double)4);
	double t = 0;
	//const double dt = 1 / FPS;	///		1/10 fps = 100 ms, 100.000 ns
	double dt = (static_cast<double>(1) / FPS);

	std::cout << std::endl;
	spdlog::info("Server started with fixedDeltaTime = {}\n", dt);

	double accumulator = 0;

	auto currentTime = clock.now();

	int tickSinceStart = 0;
	int correctedTicks = 0;
	std::chrono::time_point startTime = clock.now();

	//std::random_device rd;
	//std::mt19937 gen(rd());
	//std::uniform_int_distribution<> dis(1000, 1200);

	if (worldServer.Start() && streamServer.Start())
	{

		startTime = clock.now();

		// This is an attempt to have the server run on a fixed time step. 
		while (1)
		{
			auto newTime = clock.now();

			std::chrono::duration<double> elapsed_seconds = newTime - currentTime;
			double frameTime = std::chrono::duration_cast<std::chrono::duration<double>>(elapsed_seconds).count();

			// If we cant even do 4 fps we start throttling.
			if (frameTime > MIN_FPS)
			{
				//std::cout << "Hope to not see this." << std::endl;
				//spdlog::error("CORRECTION");
				frameTime = MIN_FPS;
			}

			currentTime = newTime;
			accumulator += frameTime;

			while (accumulator >= dt)
			{
				double _fps = (static_cast<double>(1) / frameTime);
				int intFps = (int)std::round(_fps);
				
				//std::cout << std::endl;
				tickSinceStart++;
				//std::cout << "[World] - Tick " << tickSinceStart << " Fps: " << intFps << std::endl;

				// measure time now and see if updates per second is correct
				auto timeNow = clock.now();
				std::chrono::duration<double> timeSinceStart = timeNow - startTime;
				double doubleTimeSinceStart = std::chrono::duration_cast<std::chrono::duration<double>>(timeSinceStart).count();
				int calcTicks = (int)std::round(doubleTimeSinceStart / dt);

				if (tickSinceStart != calcTicks) {
					//spdlog::error("TICKS WRONG");
					//std::cout << "tickSinceStart: " << tickSinceStart << " calcTicks: " << calcTicks << " correctedTicks: " << correctedTicks << std::endl;
					tickSinceStart = calcTicks;
					correctedTicks++;
				}

				worldServer.Update(-1, false);
				streamServer.Update(-1, false);
				worldManager.Update((float)dt);

				
				t += dt;
				accumulator -= dt;
			}
			
			// if we have more than 20 ms left, do sleep until atleast 10 ms left.
			auto timeLeft = dt - accumulator;
			if (timeLeft > (double)0.02) {
				double durationLeft = timeLeft - 0.01;
				auto sleepTime = std::chrono::duration_cast<std::chrono::steady_clock::duration>(std::chrono::duration<double>(durationLeft));
				//std::cout << "Sleep: " << sleepTime << '\n';
				const auto start = std::chrono::high_resolution_clock::now();
				std::this_thread::sleep_for(sleepTime);
				const auto end = std::chrono::high_resolution_clock::now();
				const std::chrono::duration<double, std::milli> elapsed = end - start;

				//std::cout << "Sleep: " << elapsed << '\n';
			}
		}
	}
	
	return 0;
}

void WorldServer::UpdateAllWorlds()
{
	// For the main world and all its entities

	// For the instanced worlds, like dungeons etc..
}
