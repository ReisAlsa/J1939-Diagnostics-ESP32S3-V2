#!/bin/bash

# Este script atualiza o repositório local com as últimas alterações do GitHub.
# Ele deve ser executado na raiz do projeto.

set -e

echo "A verificar o estado do repositório..."

echo "A guardar alterações locais (se existirem)..."
git stash

echo "A puxar as últimas alterações do ramo 'main'..."
git pull origin main

echo "A tentar reaplicar alterações locais..."
git stash pop || echo "Aviso: Nenhuma alteração local para reaplicar ou ocorreram conflitos."

echo "Repositório atualizado com sucesso."
