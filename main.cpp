#include <iostream>
#include <string>

#include "file_writer.hpp"
#include "buffer.hpp"
#include "producer.hpp"
#include "consumer.hpp"

// O que é produtor x consumidor? 1 buffer
// Qual o nosso problema?
// Buffer -> Nossa fila de logs JSON
// Producer -> Produtor/emissor de logs.
// Consumer -> Consome os logs do buffer e escreve no arquivo.


int main() {
    MessageBuffer messageBuffer(3);
    FileWriter fileWriter("logs.json");

    // Instanciando producers e consumers
    Producer<MessageBuffer> producerOne(messageBuffer, 1);
    Producer<MessageBuffer> producerTwo(messageBuffer, 2);

    Consumer<MessageBuffer, FileWriter> consumerOne(messageBuffer, fileWriter, 6);
    Consumer<MessageBuffer, FileWriter> consumerTwo(messageBuffer, fileWriter, 7);

    // Iniciando producers e consumers
    producerOne.start();
    producerTwo.start();
    consumerOne.start();
    consumerTwo.start();

    // Deixa sistema rodar por 10 segundos
    std::this_thread::sleep_for(std::chrono::seconds(10));

    std::cout << "\n=== Iniciando Shutdown ===" << std::endl;

    messageBuffer.shutdown();

    producerOne.stop();
    producerTwo.stop();
    consumerOne.stop();
    consumerTwo.stop();

    fileWriter.flush();

    std::cout << "=== Sistema Finalizado ===" << std::endl;

    return 0;
}
