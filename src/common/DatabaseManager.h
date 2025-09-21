#pragma once

#include <vector>
#include <string>

class DatabaseManager {
public:
    bool begin();
    std::vector<std::string> getVehicleList();
    // Futuramente, pode ter uma função para obter detalhes de um veículo:
    // VehicleConfig getVehicleConfig(const std::string& vehicleName);
};

extern DatabaseManager databaseManager;