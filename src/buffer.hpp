#pragma once

#include <queue>
#include <string>
#include <mutex>
#include <atomic>
#include <condition_variable>

class MessageBuffer {
public:
    /**
     * @brief Construtor que define capacidade máxima do buffer
     * @param capacity Número máximo de mensagens que o buffer pode armazenar
     * @throws std::invalid_argument se capacity for 0
     */
    explicit MessageBuffer(size_t capacity) : max_capacity(capacity) {
        if (capacity == 0) {
            throw std::invalid_argument("Capacidade do buffer deve ser maior que zero");
        }
    }

    ~MessageBuffer() {
        shutdown();
    }

    /**
     * @brief Adiciona mensagem ao buffer (operação de Producer)
     * @param message Mensagem a ser adicionada
     * @return true se mensagem foi adicionada, false se buffer foi fechado
     *
     * Bloqueia se buffer estiver cheio até haver espaço disponível.
     * Retorna false apenas se shutdown() foi chamado durante a espera.
     */
    bool push(const std::string& message) {
        std::unique_lock<std::mutex> lock(mutex);

        // Espera até haver espaço ou buffer ser fechado
        not_full.wait(lock, [this]() {
            return buffer.size() < max_capacity || is_shutdown;
        });

        // Se buffer foi fechado durante a espera
        if (is_shutdown) {
            return false;
        }

        // Adiciona mensagem e notifica consumers
        buffer.push(message);
        not_empty.notify_one();

        return true;
    }

    /**
     * @brief Remove mensagem do buffer (operação de Consumer)
     * @param message Referência onde mensagem será armazenada
     * @return true se mensagem foi removida, false se buffer foi fechado
     *
     * Bloqueia se buffer estiver vazio até haver mensagem disponível.
     * Retorna false apenas se shutdown() foi chamado e buffer está vazio.
     */
    bool pop(std::string& message) {
        std::unique_lock<std::mutex> lock(mutex);

        // Espera até haver mensagem ou buffer ser fechado
        not_empty.wait(lock, [this]() {
            return !buffer.empty() || is_shutdown;
        });

        // Se buffer foi fechado e está vazio
        if (is_shutdown && buffer.empty()) {
            return false;
        }

        // Remove mensagem e notifica producers
        message = buffer.front();
        buffer.pop();
        not_full.notify_one();

        return true;
    }

    /**
     * @brief Verifica se buffer está cheio
     * @return true se buffer está cheio
     * @note Esta informação pode mudar imediatamente após o retorno
     */
    bool full() const {
        std::lock_guard<std::mutex> lock(mutex);
        return buffer.size() >= max_capacity;
    }

    /**
     * @brief Verifica se buffer está vazio
     * @return true se buffer está vazio
     * @note Esta informação pode mudar imediatamente após o retorno
     */
    bool empty() const {
        std::lock_guard<std::mutex> lock(mutex);
        return buffer.empty();
    }

    /**
     * @brief Obtém número atual de mensagens no buffer
     * @return Quantidade de mensagens armazenadas
     * @note Esta informação pode mudar imediatamente após o retorno
     */
    size_t size() const {
        std::lock_guard<std::mutex> lock(mutex);
        return buffer.size();
    }

    /**
     * @brief Obtém capacidade máxima do buffer
     * @return Número máximo de mensagens que podem ser armazenadas
     */
    size_t capacity() const {
        return max_capacity;
    }

    /**
     * @brief Inicia processo de shutdown do buffer
     *
     * Sinaliza para todas as threads esperando que o buffer está sendo fechado.
     * Threads em push() irão retornar false.
     * Threads em pop() irão processar mensagens restantes e então retornar false.
     */
    void shutdown() {
        {
            std::lock_guard<std::mutex> lock(mutex);
            is_shutdown = true;
        }

        // Notifica todas as threads esperando
        not_empty.notify_all();
        not_full.notify_all();
    }

private:
    mutable std::mutex mutex;                    ///< Mutex para acesso thread-safe

    std::condition_variable not_empty;           ///< Condição para consumers esperando
    std::condition_variable not_full;            ///< Condição para producers esperando
    std::queue<std::string> buffer;              ///< Fila FIFO interna

    const size_t max_capacity;                   ///< Capacidade máxima do buffer
    std::atomic<bool> is_shutdown;               ///< Flag de shutdown thread-safe

    // Desabilita cópia para evitar problemas com mutex
    MessageBuffer(const MessageBuffer&) = delete;
    MessageBuffer& operator=(const MessageBuffer&) = delete;
};
