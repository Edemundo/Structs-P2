// Prova P2 de Estrutura de dados - Turma da Noite - Prof. José Carlos Bortot
// FATEC - MC - JCB - 24/11/2016 - Versão 0.0
#include "Header.h"
//
// entry point do programa
//
void main(void)
{
	bool flgStatus;									// para receber os status da construção
	CLIENTE stCliente;								// struct para conter os dados de um cliente
	char cWork[200],								// area de trabalho para formatação do sprintf_s
		szNomeAluno[] = "Rubens Correa Pio",			// substituir o texto por seu nome
		cOpcao;										// para receber a opção do operador
	SYSTEMTIME stTime;								// para receber a data e hora do sistema

	setlocale(LC_ALL, "portuguese_brazil");
	// instanciar a classe e receber os status em flgStatus
	clAgencia objAgConta(&flgStatus);				// cria o objeto ag conta
	if(!flgStatus)									// problema na construção?
	{	// erro irrecuperável
		cout << "Impossível iniciar o programa de agência bancária!" << endl;
		PAUSA;
		return;										// volta ao Sistema Operacional
	}
	// Loop infinito
	while(true)
	{
		LIMPAR_TELA;
		GetLocalTime(&stTime);						// data e hora do sistema
		sprintf_s(cWork, 
			"\n\tProva P2 Estrutura de Dados - FATEC-MC-Turma da noite %02d/%02d/%04d\n\tAluno:%s",
			stTime.wDay, stTime.wMonth, stTime.wYear, szNomeAluno);
		cout << cWork << endl;
		cout << CADASTRAR_CLIENTE << " - Cadastrar novo cliente" << endl;
		cout << EXCLUIR_CLIENTE << " - Excluir cliente existente" << endl;
		cout << MOSTRAR_CLIENTE << " - Mostrar cliente cadastrado" << endl;
		cout << SAIR_DO_PROGRAMA << " - Sair do programa" << endl;
		cout << "\tSelecione: ";
		cin >> cOpcao;
		cOpcao = toupper(cOpcao);
		switch(cOpcao)								// avaliar a opção escolhida
		{
			case CADASTRAR_CLIENTE:
				do
				{
					cout << "Digite o código do cliente com até 12 carácteres(ou 0 para cancelar a ação): ";
					cin >> stCliente.cAgenciaConta;
					if(strcmp(stCliente.cAgenciaConta, "0") == 0)	// digitou 0 ?
						break;
				}while(strlen(stCliente.cAgenciaConta) > 12 || stCliente.cAgenciaConta == NULL);

				if(strcmp(stCliente.cAgenciaConta, "0") == 0)	// digitou 0 ?
						break;

				if(objAgConta.VerificaSeExiste(stCliente.cAgenciaConta, &stCliente))
				{
					cout << "Cliente: " << stCliente.cAgenciaConta << "  Nome: "<< stCliente.szNome << endl
						<< "Saldo: " << stCliente.dSaldo << endl
						<< "\tJá Está Cadastrado !" << endl;
					PAUSA;
					break;
				}
				// cliente não existe

				cin.ignore(1, EOF);
				cout << "Digite o nome do cliente: ";
				cin.getline(stCliente.szNome, EXTENSAO_NOME, '\n');
				cout << "Digite o saldo do cliente: ";
				cin >> stCliente.dSaldo;

				if(!objAgConta.CadastraNovoCliente(&stCliente))
				{
					cout << "Problema na inserção do cliente: " << stCliente.cAgenciaConta << endl
						<< "Nome: " << stCliente.szNome << endl;
				}
				PAUSA;
				break;
			case EXCLUIR_CLIENTE:
				do
				{
					cout << "Digite o código do cliente com até 12 carácteres(ou 0 para cancelar a ação): ";
					cin >> stCliente.cAgenciaConta;
					if(strcmp(stCliente.cAgenciaConta, "0") == 0)	// digitou 0 ?
						break;
				}while(strlen(stCliente.cAgenciaConta) > 12 || stCliente.cAgenciaConta == NULL);

				if(strcmp(stCliente.cAgenciaConta, "0") == 0)	// digitou 0 ?
						break;
				if(!objAgConta.VerificaSeExiste(stCliente.cAgenciaConta, &stCliente))
				{
					cout << "Cliente não cadastrado !" << endl;
					PAUSA;
					break;
				}

				if(!objAgConta.ExcluirCliente(stCliente.cAgenciaConta))
				{
					cout << "Erro na exclusão do cliente: " << stCliente.cAgenciaConta << endl
						<< "Nome: " << stCliente.szNome << endl;
				}
				PAUSA;
				break;
			case MOSTRAR_CLIENTE:
				do
				{
					cout << "Digite o código do cliente com até 12 carácteres(ou 0 para cancelar a ação): ";
					cin >> stCliente.cAgenciaConta;
					if(strcmp(stCliente.cAgenciaConta, "0") == 0)	// digitou 0 ?
						break;
				}while(strlen(stCliente.cAgenciaConta) > 12 || stCliente.cAgenciaConta == NULL);

				if(strcmp(stCliente.cAgenciaConta, "0") == 0)	// digitou 0 ?
						break;
				if(!objAgConta.VerificaSeExiste(stCliente.cAgenciaConta, &stCliente))
				{
					cout << "Cliente não cadastrado !" << endl;
					PAUSA;
					break;
				}

				cout << "Cod Agência: " << stCliente.cAgenciaConta << endl
					<< "Nome: " << stCliente.szNome << endl
					<< "Saldo: " << stCliente.dSaldo << endl;
				PAUSA;
				break;
			case SAIR_DO_PROGRAMA:
				cout << "Sair realmente? (S ou N): ";
				cin >> cOpcao;					
				if(cOpcao == 's' || cOpcao == 'S')
				{	// vai sair realmente
					objAgConta.FecharArquivo();
					return;							// volta ao sistema operacional
				}
				break;
			default:
				cout << "\nOpção errada!" << endl;
				PAUSA;
		} // switch
	} // while
} // main
