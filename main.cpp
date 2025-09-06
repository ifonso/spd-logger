// Author: Afonso Lucas
//
// # Referências:
//
// The Producer Consumer Pattern - https://jenkov.com/tutorials/java-concurrency/producer-consumer.html
// Producer Consumer Patterns - https://sworthodoxy.blogspot.com/2020/05/producer-consumer-patterns.html
// Conditional Variables - https://en.cppreference.com/w/cpp/thread/condition_variable.html
// Mutex - https://en.cppreference.com/w/cpp/thread/mutex.html
// Doxygen - https://www.doxygen.nl/manual/docblocks.html

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
