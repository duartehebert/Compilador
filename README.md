# Compilador
projeto de criação de um compilador

LEXEMAS / expressões regulares

IF = if
THEN = then
ELSE = else
RELOP = "<" | "<=" | ">" | ">=" | "==" | "<>" 
ID = ( a-z | A-Z | _ )( a-z | A-Z | 0-9 | _ )*
NUM = ( + | - ) ( 0-9 ) ( 0-9 )* (( . ( 0-9 ) ( 0-9 )* ) | epslon )
ESPAÇADORES = ( " " | \t | \n | \r | \v ) ( " " | \t | \n | \r | \v )*
COMENTARIO = ( "//" ... (\n | EOF ) | "/*"... "*/" )



Autômatos Finitos

IF:    [0] ------- 'i' ---=----> [1] ----------- 'f' --------------> [2] ---------------------------------------------- (final) ----------------------------------------------> [3] (estado final, emite token IF)
        |                         |                                  |
        |                          '_'|'a-z'|'A-Z'| '0-9'--> [14]     ---------------------------------------- '_'|'a-z'|'A-Z'| '0-9' ----------------------------------------> [14]
        |
THEN:    --------- 't' --------> [4] ------------- 'h' -----------> [5] ------------ 'e' -------------> [6] ------------- 'n' -----------------> [7] ------- (final) ---------> [8] (estado final, emite token THEN)
        |                         |                                  |                                   |                                        |
        |                          '_'|'a-z'|'A-Z'| '0-9'--> [14]     -- '_'|'a-z'|'A-Z'| '0-9' -> [14]   -- '_'|'a-z'|'A-Z'| '0-9' -> [14]        -- '_'|'a-z'|'A-Z'| '0-9' -> [14]
        |
ELSE:    --------- 'e' --------> [9] ------------- 'l' ----------> [10] ------------- 's' -----------> [11] -------------- 'e' ---------------> [12] ------- (final) ---------> [13] (estado final, emite token ELSE)
        |                         |                                  |                                   |                                        |
        |                          '_'|'a-z'|'A-Z'| '0-9'--> [14]     -- '_'|'a-z'|'A-Z'| '0-9' -> [14]   -- '_'|'a-z'|'A-Z'| '0-9' -> [14]        -- '_'|'a-z'|'A-Z'| '0-9' -> [14]
        |
ID:      --- '_'|'a-z'|'A-Z' --> [14]  --------- (final) ------> [15] (estado final, emite token <ID,posição> )
        |                         |
        |                          ---- '_'|'a-z'|'A-Z| 0-9' ---> [14]
        |
NUM:     -------- '+'|'-'------> [16] ---------- '0-9' --------> [17] -- '.' --> [18] -- '0-9' --> [19] -------- (final) -------> [20] (estado final, emite token <NUM,(valor)> )
        |                                                        |                                 |                              
        |                                                        |                                  ---------- '0-9' ---------> [19]
        |                                                        |
        |                                                         ------------ '0-9' ----------> [17] 
        |                                                        |
        |                                                         ---------- (final) ----------> [20] (estado final, emite token <NUM,(valor)> )
        |
RELOP:   --------- '<' --------> [22] -- '=' --> [23] (estado final, emite token <RELOP,LE> )
        |                         |
        |                          ---- '>' ---> [24] (estado final, emite token <RELOP,NE> )
        |                         |
        |                          ------------> [25] (estado final, emite token <RELOP,LT> )
        |
         -------- '=' ---------> [26] -- '=' --> [27] (estado final, emite token <RELOP,EQ> )
        |
         -------- '>' ---------> [28] -- '=' --> [29] (estado final, emite token <RELOP,GE> )
                                  |
                                   ------------> [30] (estado final, emite token <RELOP,GT> )

* “Quando marco (final) no diagrama, significa que o autômato reconheceu um lexema completo e deve emitir o token correspondente. A aceitação acontece quando 
o próximo caractere de entrada é EOF ou não pertence ao alfabeto desse token (isto é, um separador, delimitador ou símbolo que inicia outro token).”



Relatório do Analisador Léxico

1. Descrição Teórica do Programa

O programa funciona reproduzindo a primeira fase do processo de compilação, utilizando a análise Léxica. O objetivo principal é identificar os lexemas no código-fonte e os classificar-los em tokens. Para certos tipos de tokens, ele utiliza a Tabela de Símbolos para armazenar o lexema original de forma única.

O modelo teórico fundamental por trás de qualquer analisador léxico é o de Autômatos Finitos e Expressões Regulares. Cada tipo de token na linguagem (identificadores, números, palavras-chave, operadores) pode ser descrito formalmente por uma expressão regular. 

Adicionalmente, o programa utiliza uma Tabela de Símbolos para gerenciar lexemas que podem se repetir, como identificadores e números. Ao encontrar um novo identificador, ele é armazenado na tabela. Em ocorrências futuras do mesmo identificador, em vez de armazená-lo novamente, o analisador retorna um ponteiro ou índice para a entrada já existente na tabela, otimizando o uso de memória e facilitando as fases posteriores da compilação (análise sintática e semântica).

2. Descrição da Estrutura do Programa

Definições e Constantes (#define): No início do código, são definidas constantes para os nomes dos tokens (ex: IF, ID, NUM) e para os atributos de tokens compostos (ex: LT, LE para o token RELOP). Isso torna o código mais legível e fácil de manter.

Estruturas de Dados (struct):

Simbolo: Define a estrutura de uma entrada na Tabela de Símbolos. Armazena o lexema (a string do símbolo) e pode armazenar outros atributos como tipo e categoria para uso futuro.

Token: Define a estrutura de saída do analisador. Contém um nome_token (um inteiro que representa sua classe) e um atributo (um valor adicional, como o índice na tabela de símbolos).

Tabela de Símbolos: Implementada como um array global Simbolo tabela[MAX_TABELA] e um contador qtd_simbolos. É gerenciada pelas seguintes funções:

buscarOuInserirID(char* lexema): Procura por um identificador na tabela. Se o encontra, retorna sua posição. Se não, insere-o e retorna a nova posição.

inserirNumero(char* lexema): Adiciona uma constante numérica na tabela de símbolos, identificando se é int ou float.

Funções Auxiliares:

readFile(char* fileName): Responsável por ler o conteúdo de um arquivo texto ("programa.txt") e carregá-lo completamente em uma string na memória.

falhar(): Uma rotina simples de tratamento de erros que é chamada quando o autômato encontra uma transição inválida.

Analisador Léxico (Token proximo_token()): implementa um autômato finito determinístico (AFD) que reconhece cada token, caractere por caractere.

Programa Principal (int main()): É o "driver" do analisador. Suas responsabilidades são:

Chamar readFile() para carregar o arquivo.

Entrar em um loop que chama proximo_token() repetidamente.

O loop continua até que proximo_token() retorne um token especial de fim de arquivo (EOF).

Liberar a memória alocada para o código-fonte antes de terminar.

3. Descrição de seu Funcionamento

Inicialização: O programa é executado, e a função main chama readFile() para carregar todo o conteúdo do arquivo "programa.txt" em uma única string, que é armazenada na variável global code.

Início do Loop: A função main inicia um loop do-while e faz a primeira chamada à função proximo_token().

Processamento de um Token: Dentro de proximo_token(), o autômato (o switch) inicia no estado = 0.

Ele consome e ignora quaisquer caracteres de espaço em branco (' ', '\n').

Ao encontrar um caractere significativo, ele transita para um novo estado (ex: ao ler 'i', vai para o estado 1; ao ler um dígito, vai para o estado 16).

A máquina de estados continua a transitar de estado em estado, consumindo caracteres. Se estiver reconhecendo um ID ou número, esses caracteres são acumulados em um buffer local lexema.

O reconhecimento de um lexema termina quando um caractere que não pode continuar o lexema atual é encontrado (ex: um espaço após uma palavra-chave, ou um operador após um número).

Retorno do Token: Ao atingir um estado final, a função:

Identifica o nome_token correspondente.

Se for um ID ou número, chama a função apropriada (buscarOuInserirID ou inserirNumero) para obter o atributo (posição na tabela). Para tokens simples como palavras-chave, o atributo é -1.

Imprime na tela o token reconhecido.

Retorna a struct Token preenchida para a função main.

Continuação e Fim: O loop na main verifica se o token recebido é EOF. Se não for, ele continua, chamando proximo_token() novamente para encontrar o próximo lexema na sequência. Quando o final da string code (\0) é alcançado, proximo_token() retorna o token EOF, o loop se encerra e o programa termina sua execução.

4. Descrição dos Testes Realizados e Saídas Obtidas

Arquivo de Teste (programa.txt):

if x > 100 then
  y <= 25
else
  z <> ifx
endif

Execução do Programa e Saída Obtida:

<if, >
<id, (posição 0: x)>
<relop, GT>
<NUM, 100>
<then, >
<id, (posição 1: y)>
<relop, LE>
<NUM, 25>
<else, >
<id, (posição 2: z)>
<relop, NE>
<id, (posição 3: ifx)>
<id, (posição 4: endif)>
