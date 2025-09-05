#pragma once

#include <exception>
#include <thread>
#include <string>
#include <atomic>
#include <iostream>


template<typename LogBuffer, typename FileWriter>
class Consumer {
public:
    /**
     * @brief Construtor que inicializa consumer com buffer, ID e arquivo de saída
     * @param buffer Referência para buffer de onde logs serão consumidos
     * @param consumer_id ID único deste consumer
     * @param filename Nome do arquivo onde logs serão escritos (padrão: "log.json")
     */
    Consumer(LogBuffer& buffer, FileWriter& log_writer, int consumer_id)
        : buffer_ref(buffer),
          consumer_id(consumer_id),
          is_running(false),
          log_writer(log_writer) {}

    ~Consumer() {
        stop();
    }

    /**
     * @brief Inicia a rotina de consumo de logs
     *
     * Cria uma thread separada que consome mensagens do buffer e escreve no arquivo.
     * Não faz nada se já estiver rodando.
     */
    void start() {
        if (is_running.load()) {
            std::cout << "Consumer [" << consumer_id << "] já iniciado." << std::endl;
            return;
        }

        is_running.store(true);
        worker_thread = std::thread(&Consumer::writing_routine, this);

        std::cout << "Consumer [" << consumer_id << "] iniciado..." << std::endl;
    }

    /**
     * @brief Para a rotina de consumo
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

        std::cout << "Consumer [" << consumer_id << "] parado..." << std::endl;
    }

private:
    LogBuffer& buffer_ref;              ///< Referência ao buffer de logs
    FileWriter& log_writer;             ///< Referência ao escritor de arquivo

    int consumer_id;                    ///< ID único deste consumer

    std::thread worker_thread;          ///< Thread dedicada para consumo
    std::atomic<bool> is_running;       ///< Flag thread-safe para controle de execução

    /**
     * @brief Rotina principal executada na thread separada
     *
     * Loop que consome mensagens do buffer e escreve no arquivo até receber
     * sinal de parada. Trata exceções para evitar crash da thread.
     *
     * A rotina funciona de forma que:
     * - Tenta consumir mensagens enquanto o consumer estiver rodando
     * - Se buffer.pop() retorna false (buffer vazio), verifica se deve continuar
     * - Se is_running for false, para o loop
     * - Inclui pequeno delay para evitar busy-waiting excessivo
     */
    void writing_routine() {
        try {
            std::string message;

            while (is_running.load()) {
                if (buffer_ref.pop(message)) {
                    log_writer.append(message);

                    // Log no terminal sem quebrar a mensagem
                    std::ostringstream oss;
                    oss << "\n[CONSUMER " << consumer_id << "] processou: " << message;
                    std::cout << oss.str() << std::endl;

                } else {
                    std::this_thread::sleep_for(std::chrono::milliseconds(10));
                }
            }
        } catch (const std::exception& e) {
            std::cout
                << "Erro em consumer [" << consumer_id << "] -> "
                << e.what()
                << std::endl;
        }
    }
};
