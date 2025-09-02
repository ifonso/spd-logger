#include <iostream>
#include <string>

#include "file_writer.hpp"
#include "buffer.hpp"
#include "producer.hpp"
#include "consumer.hpp"

int main() {
    MessageBuffer messageBuffer(3);
    FileWriter fileWriter("logs.json");

    // Instanciando producers e consumers
    Producer<MessageBuffer> producerOne(messageBuffer, 1);
    Producer<MessageBuffer> producerTwo(messageBuffer, 2);

    Consumer<MessageBuffer> consumerOne(messageBuffer, 1, fileWriter);

    // Iniciando producers e consumers
    producerOne.start();
    producerTwo.start();
    consumerOne.start();

    // Deixa sistema rodar por 10 segundos
    std::this_thread::sleep_for(std::chrono::seconds(10));

    std::cout << "\n=== Iniciando Shutdown ===" << std::endl;

    messageBuffer.shutdown();

    producerOne.stop();
    producerTwo.stop();
    consumerOne.stop();

    fileWriter.flush();

    std::cout << "=== Sistema Finalizado ===" << std::endl;

    return 0;
}
