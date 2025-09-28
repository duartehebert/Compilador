#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//NOME TOKENS
#define IF  256
#define THEN  257
#define ELSE  258
#define RELOP  259
#define ID  260
#define NUM  261

//ATRIBUTOS
#define LT 262
#define LE 263
#define EQ 264
#define NE 265
#define GT 266
#define GE 267

#define MAX_TABELA 1000

typedef struct {
	char lexema[50];    // nome do símbolo
	char tipo[20];      // ex: "int", "float"
	char categoria[20]; // ex: "variável", "função"
	int endereco;       // posição na memória (opcional, pra código objeto)
} Simbolo;

Simbolo tabela[MAX_TABELA];
int qtd_simbolos = 0;

int buscarOuInserirID(char* lexema) {
    // Verifica se já existe
    for(int i = 0; i < qtd_simbolos; i++) {
        if(strcmp(tabela[i].lexema, lexema) == 0) {
            return i; // retorna posição existente
        }
    }
    // Se não existe, adiciona
    if(qtd_simbolos < MAX_TABELA) {
        strcpy(tabela[qtd_simbolos].lexema, lexema);
        // tabela[qtd_simbolos].tipo = "int"; // opcional: definir tipo
        return qtd_simbolos++;
    } else {
        printf("Erro: tabela de símbolos cheia!\n");
        return -1;
    }
}

int inserirNumero(char* lexema) {
    if(qtd_simbolos < MAX_TABELA) {
        strcpy(tabela[qtd_simbolos].lexema, lexema);
        strcpy(tabela[qtd_simbolos].tipo, strchr(lexema, '.') ? "float" : "int");
        return qtd_simbolos++;
    } else {
        printf("Erro: tabela cheia!\n");
        return -1;
    }
}

typedef struct{
 	int nome_token;
	int atributo;
} Token;

int estado = 0;
int partida = 0;
int cont_sim_lido = 0;
int valor_lexico;
char *code;


char *readFile(char *fileName)
{
	FILE *file = fopen(fileName, "r");
	char *code;
	int n = 0;
	int c;

	if(file == NULL) return NULL;

	fseek(file, 0, SEEK_END);
	long f_size = ftell(file);
	fseek(file, 0, SEEK_SET);
	code = (char *)malloc(f_size + 1);
	if(code == NULL) return NULL;

	while ((c = fgetc(file))!= EOF)
	{
		code[n++] = (char) c;
	}
	code[n] = '\0';
	return code;
}




int falhar()
{
	switch(estado)
	{
	case 0: partida = 9; break;

	case 9: partida = 12; break;

	case 12: partida = 20; break;

	case 20: partida = 25; break;

	case 25:
		//retornar msg de erro
		printf("Erro encontrado no código\n");
		cont_sim_lido++;
		break;

	default: printf("Erro do compilador");
	}
	return(partida);
}

Token proximo_token()
{
	char lexema[50];
	int lex_len = 0;
	Token token;
	char c;
	while(code[cont_sim_lido] != '\0')
	{
		switch(estado)
		{
			case 0:
				c = code[cont_sim_lido];
				if((c == ' ')||(c == '\n'))
				{
					estado = 0;
					cont_sim_lido++;
				}
				else if(c == 'i') estado = 1; // possível início do "if"
				else if(c == 't') estado = 4; // possível início do "then"
				else if(c == 'e') estado = 9; // possível início do "else"	
				else if((c >= 'a' && c <= 'z') ||
						(c >= 'A' && c <= 'Z') ||
						(c == '_')) estado = 14; // possível início de um id
				else if(c == '+' || c == '-') estado = 16; // possível início de um número
				else if((c >= '0') && (c <= '9')) estado = 16; // possível início de um número
				else if(c == '<') estado = 22; // possível início de um operador relacional
				else if(c == '=') estado = 26; 
				else if(c == '>') estado = 28;
				else
					{
					 estado = falhar();
					}
				break;

			case 1: // possível início do "if"
				cont_sim_lido++;
				c = code[cont_sim_lido];
				if(c == 'f') estado = 2; // possível continuação do "if"
				else if ((c >= 'a' && c <= 'z') ||
						(c >= 'A' && c <= 'Z') ||
						(c == '_') ||
						(c >= '0' && c <= '9'))
				{
					lexema[0] = 'i';
					lex_len = 1;
					estado = 14; // possível início de um id
				}
				else estado = falhar();
				break;

			case 2: // continuação do "if"
				cont_sim_lido++;
				c = code[cont_sim_lido];
				if((c == ' ')||(c == '\n')) estado = 3; // fim do "if"
				else if ((c >= 'a' && c <= 'z') ||
						(c >= 'A' && c <= 'Z') ||
						(c == '_') ||
						(c >= '0' && c <= '9')) 
				{
					lexema[0] = 'i';
					lexema[1] = 'f';
					lex_len = 2;	
					estado = 14; // possível início de um id
				}
				else estado = falhar();
				break;

			case 3: // fim do "if"
				printf("<if, >\n");
				token.nome_token = IF;
				token.atributo = -1;
				estado = 0;
				return(token);
				break;

			case 4: // possível início do "then"
				cont_sim_lido++;
				c = code[cont_sim_lido];
				if(c == 'h') estado = 5; // possível continuação do "then"
				else if ((c >= 'a' && c <= 'z') ||
						(c >= 'A' && c <= 'Z') ||
						(c == '_') ||
						(c >= '0' && c <= '9'))
				{
					lexema[0] = 't';
					lex_len = 1;
					estado = 14; // possível início de um id
				} 
				else estado = falhar();
				break;

			case 5: // continuação do "then"
				cont_sim_lido++;
				c = code[cont_sim_lido];
				if(c == 'e') estado = 6; // possível continuação do "then"
				else if ((c >= 'a' && c <= 'z') ||
						(c >= 'A' && c <= 'Z') ||
						(c == '_') ||
						(c >= '0' && c <= '9'))
				{
					lexema[0] = 't';
					lexema[1] = 'h';
					lex_len = 2;
					estado = 14; // possível início de um id
				}
				else estado = falhar();
				break;

			case 6: // continuação do "then"
				cont_sim_lido++;
				c = code[cont_sim_lido];
				if(c == 'n') estado = 7; // possível continuação do "then"
				else if ((c >= 'a' && c <= 'z') ||
						(c >= 'A' && c <= 'Z') ||
						(c == '_') ||
						(c >= '0' && c <= '9'))
				{
					lexema[0] = 't';
					lexema[1] = 'h';
					lexema[2] = 'e';
					lex_len = 3;
					estado = 14; // possível início de um id
				} 
				else estado = falhar();
				break;

			case 7: // continução do "then"
				cont_sim_lido++;
				c = code[cont_sim_lido];
				if((c == ' ')||(c == '\n')) estado = 8; // fim do "then"
				else if ((c >= 'a' && c <= 'z') ||
						(c >= 'A' && c <= 'Z') ||
						(c == '_') ||
						(c >= '0' && c <= '9'))
				{
					lexema[0] = 't';
					lexema[1] = 'h';
					lexema[2] = 'e';
					lexema[3] = 'n';
					lex_len = 4;
					estado = 14; // possível início de um id
				} 
				else estado = falhar();
				break;

			case 8: // fim do "then"
				printf("<then, >\n");
				token.nome_token = THEN;
				token.atributo = -1;
				estado = 0;
				return(token);
				break;

			case 9: // possível início do "else"
				cont_sim_lido++;
				c = code[cont_sim_lido];
				if(c == 'l') estado = 10; // possível continuação do "else"
				else if ((c >= 'a' && c <= 'z') ||
						(c >= 'A' && c <= 'Z') ||
						(c == '_') ||
						(c >= '0' && c <= '9'))
				{
					lexema[0] = 'e';
					lex_len = 1;
					estado = 14; // possível início de um id
				}
				else estado = falhar();
				break;

			case 10: // continuação do "else"
				cont_sim_lido++;
				c = code[cont_sim_lido];
				if(c == 's') estado = 11; // possível continuação do "else"
				else if ((c >= 'a' && c <= 'z') ||
						(c >= 'A' && c <= 'Z') ||
						(c == '_') ||
						(c >= '0' && c <= '9'))
				{
					lexema[0] = 'e';
					lexema[1] = 'l';
					lex_len = 2;
					estado = 14; // possível início de um id
				}
				else estado = falhar();
				break;

			case 11: // continuação do "else"
				cont_sim_lido++;
				c = code[cont_sim_lido];
				if(c == 'e') estado = 12; // possível continuação do "else"
				else if ((c >= 'a' && c <= 'z') ||
						(c >= 'A' && c <= 'Z') ||
						(c == '_') ||
						(c >= '0' && c <= '9'))
				{
					lexema[0] = 'e';
					lexema[1] = 'l';
					lexema[2] = 's';
					lex_len = 3;
					estado = 14; // possível início de um id
				}
				else estado = falhar();
				break;

			case 12: // fim do "else"
				cont_sim_lido++;
				c = code[cont_sim_lido];
				if((c == ' ')||(c == '\n')) estado = 13; // fim do "else"
				else if ((c >= 'a' && c <= 'z') ||
						(c >= 'A' && c <= 'Z') ||
						(c == '_') ||
						(c >= '0' && c <= '9'))
				{
					lexema[0] = 'e';
					lexema[1] = 'l';
					lexema[2] = 's';
					lexema[3] = 'e';
					lex_len = 4;
					estado = 14; // possível início de um id
				} 
				else estado = falhar();
				break;

			case 13: // fim do "else"
				printf("<else, >\n");
				token.nome_token = ELSE;
				token.atributo = -1;
				estado = 0;
				return(token);
				break;

			case 14: // início do id
				lexema[lex_len++] = code[cont_sim_lido];
				cont_sim_lido++;
				c = code[cont_sim_lido];
				if((c == ' ')||(c == '\n')) estado = 15; // fim do id
				else if ((c >= 'a' && c <= 'z') ||
						(c >= 'A' && c <= 'Z') ||
						(c == '_') ||
						(c >= '0' && c <= '9')) estado = 14; // continuação do id
				else estado = falhar();
				break;

			case 15: // fim do id
				lexema[lex_len] = '\0'; // Finaliza a string do lexema
				int posicao = buscarOuInserirID(lexema); 
				printf("<id, (posição %d: %s)>\n", posicao, lexema);
				token.nome_token = ID;
				token.atributo = posicao;
				estado = 0;
				lex_len = 0;
				return(token);
				break;

			case 16: // início do número
				lex_len = 0;
				lexema[lex_len++] = code[cont_sim_lido];
				cont_sim_lido++;
				c = code[cont_sim_lido];
				if((c >= '0') && (c <= '9')) estado = 17; // parte inteira
				else estado = falhar();
				break;

			case 17: // parte inteira
				lexema[lex_len++] = code[cont_sim_lido];
				cont_sim_lido++;
				c = code[cont_sim_lido];
				if((c >= '0') && (c <= '9')) estado = 17; // continua parte inteira
				else if(c == '.') estado = 18; // possível parte decimal
				else if ((c == ' ')||(c == '\n')) estado = 20; // número inteiro
				else estado = falhar();
				break;

			case 18: // parte decimal
				lexema[lex_len++] = c; // adiciona o '.'
				cont_sim_lido++;
				c = code[cont_sim_lido];
				if((c >= '0') && (c <= '9')) estado = 19; // parte decimal
				else if ((c == ' ')||(c == '\n')) estado = 20;
				else estado = falhar();
				break;

			case 20: // fim do número
				lexema[lex_len] = '\0'; // termina a string
				int posicao = inserirNumero(lexema);
				printf("<NUM, %s>\n", lexema);
				token.nome_token = NUM;
				token.atributo = posicao;
				estado = 0;
				return token;
				break;
				
			case 22: // possível início de um operador relacional
				cont_sim_lido++;
				c = code[cont_sim_lido];

				if(c == '=') estado = 23; // possível continuação do '<='
				else if(c == '>') estado = 24; // possível continuação do '<>'
				else estado = 25; // operador '<' sozinho
				break;

			case 23: // continuação do '<='
				printf("<relop, LE>\n");
				token.nome_token = RELOP;
				token.atributo = LE;
				estado = 0;
				return(token);
				break;

			case 24: // continuação do '<>'
				printf("<relop, NE>\n");
				token.nome_token = RELOP;
				token.atributo = NE;
				estado = 0;
				return(token);
				break;

			case 25: // operador '<' sozinho
				printf("<relop, LT>\n");
				token.nome_token = RELOP;
				token.atributo = LT;
				estado = 0;
				return(token);
				break;

			// o caso 26 foi adicionado para verificar um segundo '='
			// para formar o operador de igualdade '=='
			// antes, o caso 26 não existia e o '=' sozinho ja validava como se fosse o operador '=='	
			
			case 26: // possível início de um operador relacional
				cont_sim_lido++;
				c = code[cont_sim_lido];
				if(c == '=') estado = 27; // possível continuação do '=='
				break;

			case 27: // continuação do '=='
				printf("<relop, EQ>\n");
				token.nome_token = RELOP;
				token.atributo = EQ;
				estado = 0;
				return(token);
				break;

			case 28: // possível início de um operador relacional
				cont_sim_lido++;
				c = code[cont_sim_lido];
				if(c == '=') estado = 29; // possível continuação do '>='
				else estado = 30; // operador '>' sozinho
				break;

			case 29: // continuação do '>='
				printf("<relop, GE>\n");
				token.nome_token = RELOP;
				token.atributo = GE;
				estado = 0;
				return(token);
				break;

			case 30: // operador '>' sozinho
				printf("<relop, GT>\n");
				token.nome_token = RELOP;
				token.atributo = GT;
				estado = 0;
				return(token);
				break;

/* 				
			case 9:
				c = code[cont_sim_lido];
				if((c == ' ')||(c == '\n'))
				{
					estado = 0;
					cont_sim_lido++;
				}
				else
				{
					//implementar ações referentes aos estado 
					estado = falhar();
				}

				break;

				//implementar ações para os estados 10, 11, 12

			case 12:
				c = code[cont_sim_lido];
				if((c == ' ')||(c == '\n'))
				{
					estado = 0;
					cont_sim_lido++;
				}
				else
				{
					//implementar ações referentes aos estado 
					estado = falhar();
				}
				break;

				//implementar ações para os estados 13-19

			case 20:
				c = code[cont_sim_lido];
				if((c == ' ')||(c == '\n'))
				{
					estado = 0;
					cont_sim_lido++;
				}
				else
				{
					//implementar ações referentes aos estado 
					estado = falhar();
				}
				break;

				//implementar ações para os estados 21-24

			case 25:
				c = code[cont_sim_lido];
				if((c == ' ')||(c == '\n'))
				{
					estado = 0;
					cont_sim_lido++;
				}
				else
				{
					//implementar ações referentes aos estado
					estado = falhar();
					token.nome_token = -1;
					token.atributo = -1;
					return(token);
				} 
				break; */
		}

	}
	token.nome_token = EOF;
	token.atributo = -1;
	return(token);
}




int main ()
{
	Token token;
    code = readFile("programa.txt");
    if (code == NULL) {
        printf("Erro ao ler o arquivo programa.txt\n");
        return 1;
    }

    printf("Iniciando analise lexica...\n\n");

    do {
        token = proximo_token();
    } while (token.nome_token != EOF);

    printf("\nAnalise lexica concluida.\n");

    free(code); // Libera a memória alocada para o código
    return 0;
}