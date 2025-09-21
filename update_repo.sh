#!/bin/bash

# Este script automatiza o envio de alterações locais para o repositório no GitHub.
# Uso: ./update_repo.sh "A sua mensagem de commit aqui"

# Faz com que o script pare imediatamente se um comando falhar
set -e

# 1. Verifica se uma mensagem de commit foi fornecida como argumento
if [ -z "$1" ]; then
    echo "Erro: É necessário fornecer uma mensagem para o commit."
    echo "Exemplo de uso: ./update_repo.sh \"Adicionada nova funcionalidade de diagnóstico\""
    exit 1
fi

echo "--> Passo 1: A adicionar todos os ficheiros modificados..."
git add .

echo "--> Passo 2: A criar o commit com a mensagem: \"$1\""
git commit -m "$1"

echo "--> Passo 3: A enviar as alterações para o GitHub..."
git push origin main

echo -e "\nSucesso! O seu repositório no GitHub foi atualizado."
