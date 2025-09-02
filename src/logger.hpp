#pragma once

#include <ctime>
#include <string>
#include <chrono>
#include <sstream>
#include <memory>
#include <iostream>
#include <iomanip>

/**
 * @brief Níveis de severidade para logs
 *
 * Define os diferentes tipos de mensagens que podem ser registradas.
 * Ordenados por severidade crescente.
 */
enum class LogLevel {
    INFO,     ///< Informações gerais do sistema
    WARNING,  ///< Avisos que não impedem funcionamento
    ERROR     ///< Erros que podem afetar funcionamento
};

/**
 * @brief Sistema de logging genérico com formatação JSON
 * @tparam LogBuffer Tipo do buffer que implementa método push(string), idealizado uma fila.
 *
 * Esta classe template permite usar diferentes tipos de buffer para
 * armazenamento dos logs (arquivo, console, memória, etc).
 */
template<typename LogBuffer>
class Logger {
public:
    /**
    * @brief Construtor que vincula o logger a um buffer específico
    * @param buffer Referência para o buffer onde logs serão enviados
    *
    * @note O buffer deve existir durante toda vida útil do logger
    * @warning Não copie o logger - ele mantém referência ao buffer original
    */
    explicit Logger(LogBuffer& buffer) : internal_buffer(buffer) {}

    /**
    * @brief Registra uma mensagem de log no buffer
    * @param message Texto da mensagem a ser registrada
    * @param level Nível de severidade do log
    * @param producer_id ID numérico do produtor/módulo que gerou o log
    * @return Em caso de sucesso, retorna um std::unique_ptr contendo a string de log
    * formatada em JSON. Em caso de falha (ex: buffer cheio), retorna `nullptr`.
    *
    * Gera um JSON formatado com timestamp automático e envia para o buffer.
    *
    * Formato de saída:
    * @code
    * {
    *   "timestamp": "2025-08-31T16:32:01.123Z",
    *   "level": "ERROR",
    *   "producer_id": 3,
    *   "message": "Mensagem do erro"
    * }
    * @endcode
    */
    std::unique_ptr<std::string> log(const std::string& message, LogLevel level, int producer_id) {
        std::string formatted = generate_formatted_json_log(message, producer_id, level);

        if (internal_buffer.push(formatted)) {
            return std::unique_ptr<std::string>(new std::string(formatted));
        } else {
            return nullptr;
        }
    }

private:
    LogBuffer& internal_buffer;

    std::string level_to_string(LogLevel level) const {
        switch (level) {
            case LogLevel::INFO: return "INFO";
            case LogLevel::WARNING: return "WARNING";
            case LogLevel::ERROR: return "ERROR";
        }

        return "UNKNOWN";
    };

    std::string escape_json_string(const std::string& str) const {
        std::string escaped;

        for (char c : str) {
            switch (c) {
                case '"': escaped += "\\\""; break;
                case '\\': escaped += "\\\\"; break;
                case '\b': escaped += "\\b"; break;
                case '\f': escaped += "\\f"; break;
                case '\n': escaped += "\\n"; break;
                case '\r': escaped += "\\r"; break;
                case '\t': escaped += "\\t"; break;
                default: escaped += c; break;
            }
        }
        return escaped;
    };

    std::string get_current_timestamp() const {
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()) % 1000;

        std::stringstream ss;
        ss << std::put_time(std::gmtime(&time_t), "%Y-%m-%dT%H:%M:%S");
        ss << "." << std::setfill('0') << std::setw(3) << ms.count() << "Z";
        return ss.str();
    }

    std::string generate_formatted_json_log(const std::string& message, int producer_id, LogLevel level) const {
        std::stringstream json;
        json << "{\n";
        json << "  \"timestamp\": \"" << escape_json_string(get_current_timestamp()) << "\",\n";
        json << "  \"level\": \"" << escape_json_string(level_to_string(level)) << "\",\n";
        json << "  \"producer_id\": " << producer_id << ",\n";
        json << "  \"message\": \"" << escape_json_string(message) << "\"\n";
        json << "},";

        return json.str();
    }
};
