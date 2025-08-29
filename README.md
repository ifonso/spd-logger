# Logger de Aplicações (Produtor x Consumidor)

> O objetivo deste projeto é fornecer um exemplo do padrão produtor x consumidor usando multithreading em C++ para a disciplina de Sistemas Paralelos e Distribuídos.

Em uma aplicação, é comum nos depararmos com um cenário em que diferentes partes do sistema geram logs de execução com diferentes níveis de importância, como erros, avisos ou eventos. Normalmente, é essencial persistir esses dados em um sistema externo ou localmente em arquivos temporários, a fim de investigar possíveis problemas na aplicação.

Para esse cenário, teremos produtores de logs, simulando diferentes partes do sistema, e consumidores, que observam os logs e os persistem localmente em um arquivo temporário.

- Produtores: módulos independentes da aplicação que geram mensagens de log.
    - Cada log segue um formato estruturado em **JSON**.
    - Existem três tipos de log: **INFO**, **WARNING** e **ERROR**.

- Consumidor: responsável por ler os logs da fila (buffer) e gravá-los no arquivo central.
    - Apenas um consumidor pode acessar o arquivo por vez, para garantir consistência dos dados.

**Exemplo de log**
```json
{
  "timestamp": "2025-08-26T16:32:01Z",
  "level": "ERROR",
  "producer_id": 3,
  "message": "Falha ao acessar banco de dados",
  "thread": "Thread-Produtor-3"
}
```

### Funcionamento

1. Sempre que um produtor gera uma mensagem de log, ela é colocada em um buffer compartilhado (fila).
2. O consumidor (ou consumidores) monitora esse buffer e, assim que um log está disponível, o remove da fila e escreve no arquivo de log.
3. Caso haja múltiplos consumidores, apenas um pode escrever no arquivo por vez. Enquanto um consumidor escreve, os demais aguardam.
4. Assim que o consumidor libera o arquivo, o próximo log da fila pode ser processado.
5. Uma vez que um log é consumido e gravado, ele é removido do buffer.

### Restrições

- O buffer possui capacidade limitada (exemplo: apenas 1 log por vez).
- Se o buffer estiver cheio, os produtores devem aguardar até que haja espaço para inserir novos logs.
- Se o buffer estiver vazio, os consumidores aguardam até que um novo log seja produzido.
- Enquanto um produtor escreve no buffer, outro deve esperar (exclusão mútua).

## Compilação

Este projeto utiliza CMake como sistema de build.

### Linux

Crie a pasta de build:
```bash
mkdir build && cd build
```

Gere os arquivos de build com o CMake:
```bash
cmake ..
```

Compile o projeto:
```bash
make
```

### Windows

Crie a pasta de build e entre nela:
```powershell
mkdir build
cd build
```

Gere os arquivos de build com o CMake (assumimos que você esteja usando MinGW):
```powershell
cmake .. -G "MinGW Makefiles"
```

Compile o projeto:
```powershell
mingw32-make
```
