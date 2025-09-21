import base64
from datetime import datetime
from cryptography.hazmat.primitives import hashes
from cryptography.hazmat.primitives.asymmetric import padding
from cryptography.hazmat.primitives import serialization

def generate_license_key(hardware_id: str, expiry_date: str, uses: int, private_key_path: str):
    """
    Gera uma chave de licença assinada para o aparelho de diagnóstico.

    Args:
        hardware_id (str): O ID de hardware único do aparelho do cliente.
        expiry_date (str): A data de validade no formato 'YYYY-MM-DD'.
        uses (int): O número de utilizações permitidas.
        private_key_path (str): O caminho para o ficheiro da chave privada PEM.

    Returns:
        str: A chave de licença final, codificada em Base64.
    """
    try:
        # 1. Carregar a chave privada
        with open(private_key_path, "rb") as key_file:
            private_key = serialization.load_pem_private_key(
                key_file.read(),
                password=None, # Assumindo que a chave não tem password
            )

        # 2. Formatar a data e os dados da licença
        # Remove os hífens da data para o formato YYYYMMDD
        expiry_date_formatted = expiry_date.replace('-', '')
        license_data_str = f"{hardware_id}:{expiry_date_formatted}:{uses}"
        license_data_bytes = license_data_str.encode('utf-8')

        # 3. Assinar os dados da licença (o hash dos dados) com a chave privada
        signature = private_key.sign(
            license_data_bytes,
            padding.PKCS1v15(), # Padding standard
            hashes.SHA256()
        )

        # 4. Concatenar os dados originais com a sua assinatura
        signed_license = license_data_bytes + signature

        # 5. Codificar o resultado final em Base64
        license_key = base64.b64encode(signed_license).decode('utf-8')

        return license_key

    except Exception as e:
        print(f"Ocorreu um erro: {e}")
        return None

if __name__ == "__main__":
    print("--- Gerador de Chaves de Licenca ---")
    
    # Pedir os dados ao utilizador
    hw_id = input("Insira o ID de Hardware do aparelho: ").strip().upper()
    exp_date_str = input("Insira a data de validade (YYYY-MM-DD): ").strip()
    num_uses = int(input("Insira o numero de utilizacoes: ").strip())
    
    private_key_file = "private_key.pem"

    # Gerar a chave
    final_key = generate_license_key(hw_id, exp_date_str, num_uses, private_key_file)

    if final_key:
        print("\n-----------------------------------------")
        print("Chave de Licenca Gerada com Sucesso:")
        print(final_key)
        print("-----------------------------------------")
