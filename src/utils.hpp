#pragma once

#include <array>


namespace utils {
    const std::array<const char*, 5> info_messages = {{
        "O serviço de aplicação foi iniciado com sucesso na porta 8080.",
        "O usuário 'joana.silva' efetuou login com sucesso no sistema.",
        "O processamento do lote de pagamentos #54321 foi concluído. 150 transações processadas.",
        "Nova mensagem recebida da fila 'user-registration'. Iniciando processamento.",
        "Backup do banco de dados agendado concluído com sucesso."
    }};

    const std::array<const char*, 5> warning_messages = {{
        "O uso da função 'calculateLegacyTax()' está obsoleto. Use 'calculateNewTax()' em seu lugar.",
        "A partição de disco /dev/sda1 está com 85% de sua capacidade utilizada.",
        "O tempo de resposta da API externa 'api.weather.com' excedeu 2000ms.",
        "A chave de API utilizada expirará em 3 dias.",
        "Tentativa de login não autenticada para o usuário 'admin' do IP 192.168.10.25."
    }};

    const std::array<const char*, 5> error_messages = {{
        "Falha na conexão com o banco de dados: tempo limite de conexão excedido.",
        "Objeto de referência nula encontrado na linha 42 de user_processor.py.",
        "Permissão negada ao tentar acessar o diretório /var/log/app.",
        "Falha na análise do arquivo de configuração 'settings.xml': XML malformado.",
        "Memória insuficiente para completar a operação de renderização de imagem."
    }};
}
