#pragma once

#include "logger.hpp"
#include "utils.hpp"

#include <iostream>
#include <exception>
#include <memory>
#include <array>
#include <thread>
#include <atomic>
#include <random>
#include <chrono>
#include <string>

template<typename LogBuffer>
class Producer {
public:
    /**
     * @brief Construtor que inicializa producer com buffer e ID
     * @param buffer Referência para buffer onde logs serão armazenados
     * @param producer_id ID único deste producer
     */
    explicit Producer(LogBuffer& buffer, int producer_id)
        : logger(),
          buffer_ref(buffer),
          producer_id(producer_id),
          is_running(false),
          generator(std::random_device{}()) {}

    ~Producer() {
        stop();
    }

    /**
     * @brief Inicia a rotina de logging automático
     *
     * Cria uma thread separada que gera logs em intervalos de 3-10 segundos.
     * Não faz nada se já estiver rodando.
     */
    void start() {
        if (is_running.load()) {
            std::cout << "Tentativa de iniciar producer já em execução: Producer [" << producer_id << "]" << std::endl;
            return;
        }

        is_running.store(true);
        worker_thread = std::thread(&Producer::logging_routine, this);

        std::cout << "Producer [" << producer_id << "] iniciado..." << std::endl;
    }

    /**
     * @brief Para a rotina de logging
     *
     * Sinaliza para a thread parar e aguarda sua finalização.
     * Operação segura - pode ser chamada múltiplas vezes.
     */
    void stop() {
        if (!is_running.load()) {
            return;
        }

        is_running.store(false);

        if (worker_thread.joinable()) {
            worker_thread.join();
        }

        std::cout << "Producer [" << producer_id << "] parado..." << std::endl;
    }

private:
    Logger<LogBuffer> logger;           ///< Logger interno para gerar mensagens
    LogBuffer& buffer_ref;              ///< Referência ao buffer
    int producer_id;                    ///< ID único deste producer
    std::atomic<bool> is_running;       ///< Flag thread-safe para controle
    std::thread worker_thread;          ///< Thread dedicada para logging
    mutable std::mt19937 generator;     ///< Gerador de números aleatórios

    /**
     * @brief Gera intervalo aleatório entre 0 e 2 segundos
     * @return Duração em milissegundos para próximo log
     */
    std::chrono::milliseconds get_random_interval() const {
        std::uniform_int_distribution<int> distribution(0, 2000);
        return std::chrono::milliseconds(distribution(generator));
    }

    /**
     * @brief Seleciona mensagem aleatória baseada no nível
     * @param level Nível do log para filtrar mensagens apropriadas
     * @return Mensagem correspondente ao nível
     */
    std::string get_random_message(LogLevel level) const {
        const std::array<const char*, 5>* messages = get_messages_for_level(level);

        if (!messages) return "Mensagem de teste do producer " + std::to_string(producer_id);

        std::uniform_int_distribution<size_t> dist(0, messages->size() - 1);
        return std::string((*messages)[dist(generator)]);
    }

    /**
     * @brief Obtém array de mensagens para um nível específico
     * @param level Nível do log
     * @return Ponteiro para array de mensagens ou nullptr se inválido
     */
    const std::array<const char*, 5>* get_messages_for_level(LogLevel level) const {
        switch (level) {
            case LogLevel::INFO:
                return &utils::info_messages;
            case LogLevel::WARNING:
                return &utils::warning_messages;
            case LogLevel::ERROR:
                return &utils::error_messages;
        }

        return nullptr;
    }

    /**
     * @brief Seleciona nível de log aleatório com pesos realistas
     * @return Nível de log selecionado
     *
     * Distribuição: ~70% INFO, ~25% WARNING, ~5% ERROR
     */
    LogLevel get_random_log_level() const {
        // std::uniform_int_distribution<T> tem sobrecarga do operator()
        // template<class URNG>
        // result_type operator()(URNG& g);
        std::uniform_int_distribution<int> dist(1, 100);
        int random_int = dist(generator);

        if (random_int <= 70) {
            return LogLevel::INFO;
        } else if (random_int <= 95) {
            return LogLevel::WARNING;
        } else {
            return LogLevel::ERROR;
        }
    }

    /**
     * @brief Rotina principal executada na thread separada
     *
     * Loop que gera logs em intervalos aleatórios até receber sinal de parada.
     * Trata exceções para evitar crash da thread.
     */
    void logging_routine() {
        try {
            while (is_running.load()) {
                // Gera dados alearórios
                LogLevel level = get_random_log_level();
                std::string message = get_random_message(level);

                // Registra o log
                std::unique_ptr<std::string> log = logger.log(message, level, producer_id, buffer_ref);

                if (log) {
                    // Log no terminal sem quebrar a mensagem
                    std::ostringstream oss;
                    oss << "\n[PRODUCER " << producer_id <<  "] enviando: \n" << *log;
                    std::cout << oss.str() << std::endl;
                }

                // Aguarda intervalo aleatório
                std::chrono::milliseconds interval = get_random_interval();
                std::this_thread::sleep_for(interval);
            }
        } catch (const std::exception& e) {
            std::cout
                << "Erro em producer [" << producer_id << "] -> "
                << e.what()
                << std::endl;
        }
    }
};
