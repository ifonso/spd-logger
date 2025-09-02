#pragma once

#include <fstream>
#include <mutex>
#include <string>
#include <stdexcept>
#include <iostream>

class FileWriter {
public:
    /**
     * @brief Obtém instância para um arquivo específico
     * @param filename Nome do arquivo de log (padrão: "logs.json")
     * @return Referência para instância única do arquivo
     * @throws std::runtime_error se não conseguir abrir o arquivo
     */
    explicit FileWriter(const std::string& filename) : filename(filename) {
        file.open(filename, std::ios::app);
        if (!file.is_open()) {
            throw std::runtime_error("Não foi possível abrir o arquivo de log: " + filename);
        }

        std::cout << "FileWriter criado para arquivo: " << filename << std::endl;
    }

    ~FileWriter() {
        if (file.is_open()) {
            file.flush();
            file.close();
        }
    }

    /**
     * @brief Adiciona linha JSON ao arquivo de log
     * @param json_line String contendo JSON válido da mensagem de log
     *
     * Operação thread-safe que adiciona a linha ao arquivo e força flush.
     * Cada linha JSON é escrita individualmente para formar um arquivo
     * JSONL (JSON Lines) válido.
     */
    void append(const std::string& json_line) {
        std::lock_guard<std::mutex> lock(write_mutex);

        // Verifica se arquivo ainda está aberto
        if (!file.is_open()) {
            throw std::runtime_error("Arquivo de log foi fechado inesperadamente");
        }

        file << json_line << std::endl;
        file.flush(); // Garante que dados sejam escritos imediatamente
    }

    /**
     * @brief Verifica se o arquivo está aberto e operacional
     * @return true se arquivo está aberto para escrita
     */
    bool is_open() const {
        std::lock_guard<std::mutex> lock(write_mutex);
        return file.is_open();
    }

    /**
     * @brief Obtém o nome do arquivo associado a esta instância
     * @return Nome do arquivo de log
     */
    const std::string& get_filename() const {
        return filename;
    }

    /**
     * @brief Força flush do buffer interno do arquivo
     *
     * Útil para garantir que todos os dados sejam escritos em disco
     * em momentos críticos (como antes de shutdown).
     */
    void flush() {
        std::lock_guard<std::mutex> lock(write_mutex);
        if (file.is_open()) {
            file.flush();
        }
    }

    /**
     * @brief Fecha o arquivo explicitamente
     *
     * Útil para finalização controlada. Após chamada, tentativas de
     * escrita irão falhar com exceção.
     */
    void close() {
        std::lock_guard<std::mutex> lock(write_mutex);
        if (file.is_open()) {
            file.flush();
            file.close();
            std::cout << "Arquivo de log fechado: " << filename << std::endl;
        }
    }

private:
    mutable std::mutex write_mutex;     ///< Mutex para operações thread-safe

    std::ofstream file;                 ///< Stream de saída para arquivo
    std::string filename;               ///< Nome do arquivo associado



    // Desabilita cópia para evitar problemas com mutex
    FileWriter(const FileWriter&) = delete;
    FileWriter& operator=(const FileWriter&) = delete;
};
