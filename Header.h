// Declarações do programa de agência bancária
#include <iostream>
#include <Windows.h>
using namespace std;
// Definições
#define PAUSA							system("pause")
#define LIMPAR_TELA						system("cls")
#define QTDE_HASH						200					// quantidade de hash code
#define EXTENSAO_NOME					40					// extensão do nome do cliente
#define ATIVO							'A'
#define INATIVO							'I'
// opções do menu
#define CADASTRAR_CLIENTE				'C'
#define EXCLUIR_CLIENTE					'E'
#define MOSTRAR_CLIENTE					'M'
#define SAIR_DO_PROGRAMA				'S'
#define CAMINHO_ARQUIVO					"..\\Clientes.cad"
// struct do cliente
typedef struct tagCLIENTE
{
	char cAgenciaConta[12],				// agencia e conta do cliente
		cAtivo,							// 'A' - ativo e 'I' - inativo
		szNome[EXTENSAO_NOME + 1];		// nome do cliente
	double dSaldo;						// saldo do cliente
} CLIENTE;
// struct da chave de busca do cliente - vai ser a lista ligada
typedef struct tagCHAVE
{
	char cAgenciaConta[12],				// agencia e conta do cliente
		cAtivo;							// 'A' - ativo e 'I' - inativo
	int nPosicional;					// posição do cliente dentro do cadastro
	tagCHAVE *ptrProximo;				// ponteiro para o próximo cliente da lista ligada
} CHAVE;

// struct do Hash Code considerando que as inserções serão sempre no último da lista
typedef struct tagHASH
{
	CHAVE *ptrPrimeira,					// ponteiro para a primeira chave da lista
		*ptrUltima;						// ponteiro para a última
} HASH;

//
// classe clAgencia
//
class clAgencia
{
	// atributos private
private:
	// vetor de hash --->Hash Table
	HASH vetHash[QTDE_HASH];			// vetor de hash ponto de partida para cada lista ligada
	int nPosProximo;					// posicional do próximo cliente
	CHAVE stChave;						// para conter uma chave de busca
	CLIENTE stCliente;					// para conter os dados de um cliente
	FILE *fdCadastro;					// file descriptor do cadastro de clientes
	//
	// métodos privados
	//
	// Método que calcula o hash code
	// Parâmetros:
	//		Entrada: char *ptrAgConta - ponteiro para um string que contém a agencia e
	//									conta do cliente
	int CalcularHashCode(char *ptrAgConta)
	{
		int i,							// indice genérico
			nSomatoria = 0,				// somatória do char * peso
			nPesoInicial = 33;			// char indice zero * 33, indice 1 * 32...somar na somatoria
		// fazer a somatória do char[i] * peso
		for(i = 0; i < strlen(ptrAgConta) ; i++, nPesoInicial--)
			nSomatoria += *(ptrAgConta + i) * nPesoInicial;
		return (nSomatoria % QTDE_HASH); // retorna o hash code
	}

	// Método que insere uma chave na lista lista do hash code correspondente
	//	Parâmetros:
	//		Entrada: int nHash - hash code
	//				 int nPosicional - posicional do cliente dentro do cadastro
	//				 char *ptrAgConta - ponteiro do string que contém agência e conta
	//		Retorno: bool - true inserção ok
	//						false - não tem memória disponível
	bool InsereChaveNoHashTable(int nHash, int nPosicional, char *ptrAgConta)
	{
		if(vetHash[nHash].ptrPrimeira == NULL)		// primeira chave da lista?
			vetHash[nHash].ptrUltima =
				vetHash[nHash].ptrPrimeira = new CHAVE; // aloca memória e liga no hash code
		else
			vetHash[nHash].ptrUltima =
			vetHash[nHash].ptrUltima->ptrProximo = new CHAVE; // aloca memória na prox.ultima chave
		if(vetHash[nHash].ptrUltima == NULL)			// não tem memória?
			return false;								// retorna em condição de erro
		vetHash[nHash].ptrUltima->cAtivo = ATIVO;		// cliente ativo
		strcpy_s(vetHash[nHash].ptrUltima->cAgenciaConta, 11, ptrAgConta); // copiar Ag e conta
		vetHash[nHash].ptrUltima->nPosicional = nPosicional; // posicional no cadastro
		vetHash[nHash].ptrUltima->ptrProximo = NULL;	// indica o fim da lista ligada
	}

	// Método private que lê um cliente posicional do cadastro
	//	Parâmetros:
	//		Entrada: int nPosicional - posicional do cliente no cadastro
	//				 CLIENTE *ptrStCliente - ponteiro da struct para receber os dados
	//										 lidos
	//		Retorno: bool - true - leitura OK
	//						false - erro de seek ou erro de leitura
	bool LerClientePosicional(int nPosicional, CLIENTE *ptrStCliente)
	{
		if(fseek(fdCadastro, nPosicional * sizeof(CLIENTE), SEEK_SET) != 0)		// erro de seek ?
			return false;
		if(fread_s(ptrStCliente, sizeof(CLIENTE), sizeof(CLIENTE), 1, fdCadastro) == 0)	// erro de leitura ?
			return false;
		return true;
	}

	// Método private que grava um cliente posicional no cadastro
	//	Parâmetros:
	//		Entrada: int nPosicional - posicional do cliente no cadastro
	//				 CLIENTE *ptrStCliente - ponteiro da struct que contém os dados
	//										 do cliente a serem gravados
	//		Retorno: bool - true - gravação OK
	//						false - erro de seek ou erro de gravação
	bool GravarClientePosicional(int nPosicional, CLIENTE *ptrStCliente)
	{
		if(fseek(fdCadastro, nPosicional * sizeof(CLIENTE), SEEK_SET) != 0)		// erro de seek ?
			return false;
		if(fwrite(ptrStCliente, sizeof(CLIENTE), 1, fdCadastro) == 0)			// erro de gravação
			return false;
		return true;
	}

	// atributos e métodos públicos
public:
	// construtor da classe
	// Parâmetros:
	//	Entrada: bool *ptrStatus - false - indica que houve erro na construção
	//							   true - construção foi ok
	clAgencia(bool *ptrStatus)
	{
		int i;							// indice genérico
		for(i = 0; i < QTDE_HASH; i++)	// loop para inicializar o hash table
			vetHash[i].ptrPrimeira = NULL;	// hash code com sua lista vazia
		nPosProximo = 0;				// posicional inicial
		// abrir o arquivo de clientes em modo r+b - precisa existir
		if(fopen_s(&fdCadastro, CAMINHO_ARQUIVO, "r+b") != 0)	// erro de abertura?
		{	// arquivo não existe - vamos abrir em modo w+b - novo ou vazio
			if(fopen_s(&fdCadastro, CAMINHO_ARQUIVO, "w+b") != 0) // erro irrecuperável
			{
				cout << "Erro irrecuperável de abertura!" << endl;
				PAUSA;
				*ptrStatus = false;
				return;									// sai do construtor
			}
		}
		else
		{	// o cadastro de cliente existe - montar as listas ligadas nos hash code
			while(true)
			{
				if(fread_s(&stCliente, sizeof(CLIENTE), sizeof(CLIENTE), 1, fdCadastro) == 0) // fim?
				{	// fim do arquivo
					*ptrStatus = true;					// indica tudo bem
					return;								// sai do construtor
				}
				// verificar se o cliente está inativo pois não irá ter sua chave de busca
				// em memória
				if(stCliente.cAtivo != ATIVO)			// cliente não ativo
				{
					nPosProximo++;						// incrementa posicional para o próximo
					continue;							// vai buscar o próximo
				}
				// temos um cliente ativo
				// calcular o hash code 
				// fazer a inserção no hash table da chave de busca
				// incrementar para o próximo
				// continue
				if(!InsereChaveNoHashTable(CalcularHashCode(stCliente.cAgenciaConta),
					nPosProximo, stCliente.cAgenciaConta))	// não tem memória?
				{
					*ptrStatus = false;					// indica o erro
					return;								// cai fora do construtor
				}
				nPosProximo++;							// posiciona para o próximo
			} // while
		}
	} // construtor

	// Método que verifica se o cliente já está cadastrado (se existe e ativo)
	//	Se o cliente existe e ativo o método irá copiar seus dados na struct indicada pelo
	//		invocador do método
	//	Parâmetros:
	//		Entrada: char *ptrAgConta - ponteiro para o string que contém agencia e conta
	//				 CLIENTE *ptrStCliente - ponteiro de uma struct do invocador do método
	//										 que receberá os dados do cliente ativo
	//		Retorno: bool - true - o cliente existe e seus dados foram copiados para o
	//								invocador do método
	//						false - o cliente não existe ou está inativo
	bool VerificaSeExiste(char *ptrAgConta, CLIENTE *ptrStCliente)
	{
		CHAVE *ptrCorrente;
		int nHash = CalcularHashCode(ptrAgConta);

		for(ptrCorrente = vetHash[nHash].ptrPrimeira;
			ptrCorrente;
			ptrCorrente = ptrCorrente->ptrProximo)
		{
			if(ptrCorrente->cAtivo == 'I')		// cliente inativo ?
				continue;
			if(strcmp(ptrCorrente->cAgenciaConta, ptrAgConta) == 0)		// códigos iguais?
			{	// encontrou ... agora vamos ler o cliente
				if(!LerClientePosicional(ptrCorrente->nPosicional, ptrStCliente))	// erro de leitura ?
					return false;
				return true;
			}
		}
		return false;
	}

	// Método público que cadastra um novo cliente
	//	Parâmetros:
	//		Entrada: CLIENTE *ptrStCliente - ponteiro de uma struct que contém
	//										os dados do novo cliente
	//		Retorno: bool - true - indica que foi cadastrado OK
	//						false - houve algum erro no cadastramento do novo cliente
	bool CadastraNovoCliente(CLIENTE *ptrStCliente)
	{
		// 1. Gravar no posicional
		// 2. Inserir no Hash Table a nova chave sabendo o posicional
		// 3. Incrementar o posicional
		int nHash = CalcularHashCode(ptrStCliente->cAgenciaConta);

		ptrStCliente->cAtivo = 'A';
		if(!GravarClientePosicional(nPosProximo, ptrStCliente))	// erro de gravação ?
			return false;
		if(!InsereChaveNoHashTable(nHash, nPosProximo, ptrStCliente->cAgenciaConta)) // erro de inserção ?
			return false;
		nPosProximo++;
		cout << "Cliente cadastrado com sucesso !" << endl;
		return true;
	}

	// Método público que exclui cliente de maneira lógica
	//	Parâmetros:
	//		Entrada: char *ptrAgConta - ponteiro de um string que contém o código do cliente
	//		Retorno: bool - true - indica que a exclusão foi OK
	//						false - indica que houve algum erro na exclusão do cliente
	bool ExcluirCliente(char *ptrAgConta)
	{
		// 1. Varrer a lista ligada do Hash code calculado até encontrar a chave
		// 2. O cliente existe colocar na CHAVE que está inativo
		// 3. Ler posicional fazer o update para inativo e regravar posicional
		int nHash = CalcularHashCode(ptrAgConta);
		CHAVE *ptrCorrente;

		for(ptrCorrente = vetHash[nHash].ptrPrimeira;
			ptrCorrente;
			ptrCorrente = ptrCorrente->ptrProximo)
		{
			if(strcmp(ptrCorrente->cAgenciaConta, ptrAgConta) == 0)	// cliente que precuro ?
			{
				ptrCorrente->cAtivo = 'I';
				if(!GravarClientePosicional(ptrCorrente->nPosicional, &stCliente))
					return false;
				cout << "Cliente Excluido com sucesso !" << endl;
				return true;
			}
		}

	}

	// Método público que fecha o cadastro de clientes
	void FecharArquivo()
	{
		fclose(fdCadastro);
	}
};









