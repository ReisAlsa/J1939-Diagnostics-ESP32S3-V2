#ifndef DIAGNOSTICS_DATA_H
#define DIAGNOSTICS_DATA_H

#include <vector>
#include <string>

// Estrutura para representar um Código de Erro de Diagnóstico (DTC)
struct DtcData {
    bool is_active;
    uint32_t spn;
    uint8_t fmi;
    std::string description;
};

// Estrutura para armazenar os dados de identificação de uma ECU
struct EcuIdData {
    std::string part_number;
    std::string serial_number;
    std::string software_version;
};

// Estrutura para armazenar um valor em tempo real (live data)
struct LiveValueData {
    std::string name;
    float value;
    std::string unit;
};

// Simula a leitura de DTCs de uma ECU.
std::vector<DtcData> read_dtcs_from_ecu(const std::string& ecu_name);

// Simula o envio do comando para apagar os DTCs de uma ECU.
bool clear_dtcs_from_ecu(const std::string& ecu_name);

// Simula a leitura dos dados de identificação de uma ECU.
EcuIdData read_id_from_ecu(const std::string& ecu_name);

// Simula a leitura de valores em tempo real de uma ECU.
std::vector<LiveValueData> read_live_values_from_ecu(const std::string& ecu_name);

#endif // DIAGNOSTICS_DATA_H