#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "registro.h"
#include "cabecalho.h"
#include "funcoesFornecidas.h"

 #define PAGINA_TAMANHO 1600

void CREATE_TABLE(char *nomeCSV, char *nomearqbin, Cabecalho *cabecalho){
    // ler o arquivo csv (cria a página)
    //abertura do arquivo csv para leitur
    int qtt =0;
            FILE *arquivo_csv = fopen(nomeCSV, "r");
            if (arquivo_csv == NULL) {
                printf("Falha ao abrir o arquivo \n");
                return ;
            }
            FILE *arquivo_binario = fopen(nomearqbin, "wb");
            if (arquivo_binario == NULL) {
                printf("Falha ao abrir o arquivo binário.\n");
                return ;
            }

            //nome das colunas
            cabecalho_readcsv(arquivo_csv);


            //escrita
            cabecalho_writebin(arquivo_binario,cabecalho);
            //printf("Status: %c\n", cabecalho->status);
            while (!feof(arquivo_csv)) { //salva todos os dados em dino
                Registro *registro = registro_readcsv(arquivo_csv);

                // Verifica se leu corretamente o registro
                if (registro == NULL) {
                    printf("Erro ao ler o registro do CSV\n");
                    break;
                }
                
                // Exibir as informações do registro lido
                //registro_print(registro);
                registro_setEncadeamento(registro, -1);
                //escreve no arquivo binario
                registro_writebin(arquivo_binario, registro);

                //qtt de registros
                qtt++;
                // Liberar a memória alocada para o registro
                //free(registro->nome);
            }
            
            // Verificação do cabeçalho
            cabecalho_getStatus(cabecalho);
            
            
            //calculo da quantidade de paginas de disco         
            int total = qtt * 160; //qtt de registro * numero de bytes
            total = (total/1600) ; //quantidade de bytes / numeros de butes que equivale uma pagina + 1 pagina do cabelaho
            int aux = round(total) +2;
            cabecalho_setNroPagDisco(cabecalho, aux);//atualizando o valor de nro de paginas de disco
            cabecalho_setProxRRN(cabecalho, qtt); //proximo rrn
            fseek(arquivo_binario, 0, SEEK_SET);
            // Escreve o cabeçalho no arquivo binário
            cabecalho_writebin(arquivo_binario, cabecalho);

            // chamada pra função de criar a tabela
            fclose(arquivo_csv);
            fclose(arquivo_binario);
}   

void SELECT_TABLE(char *nomearqbin) {
    FILE *arquivo_binario = fopen(nomearqbin, "rb");
    if (arquivo_binario == NULL) {
        printf("Falha ao abrir o arquivo \n");
        return;
    }
    
    Registro *registro;  // Estrutura para armazenar um registro

    Cabecalho *cabecalho = cabecalho_readbin(arquivo_binario);
    fseek (arquivo_binario, 1600, SEEK_SET);
    // Lê registros do arquivo binário
    int cont_registro=0;
    while (1) {
        
        //sabemos que ele morre no while, mas passa uma vez so aqui
        registro = registro_readbin(arquivo_binario);
        cont_registro++;

        //ele nao passa do primeiro registro_readbin (local do problema )
        // Verifica se a lei/sabemo que o arquivo abretura foi bem-sucedida
        if (registro->removido == 'E') {
            break;  // Sai do loop se não houver mais registros para ler
        }
        if(registro_isValid(registro)==false){
            fseek (arquivo_binario, 1600+REGISTRO_SIZE*(cont_registro), SEEK_SET);
            continue;
        }
        
        registro_print(registro);  // Imprime o registro lido
    }
    printf("Numero de paginas de disco: %d\n", cabecalho->nroPagDisco);
    printf("\n");
    free(registro);
    free(cabecalho);
    fclose(arquivo_binario); 
}

//função que busca registros pelo campo e valor
int SELECT_WHERE(char *nomearquivo, char *campo, int quant) {
    FILE *arquivo_binario = fopen(nomearquivo, "rb");
    if (arquivo_binario == NULL) {
        printf("Falha no processamento do arquivo.\n");
        return 0; // Retorna 0 se não conseguiu abrir o arquivo
    }

    Cabecalho *cabecalho = cabecalho_readbin(arquivo_binario);
    if (cabecalho == NULL) {
        printf("Falha ao ler o cabeçalho do arquivo.\n");
        fclose(arquivo_binario);
        return 0;
    }

    Registro *registro ;
    fseek(arquivo_binario,1600, SEEK_SET); // Setando a leitura para a primeira página de dados

    int cont_registro = 0;
    int teste = verificacaoString(campo); // Verifica o tipo do campo

    // Verificação do tipo da variável que deve-se buscar no arquivo binário
    if (teste == -1) {
        printf("Campo inválido.\n");
        free(cabecalho);
        fclose(arquivo_binario);
        return 0; // Campo inválido
    }

    // Leitura do valor a ser buscado
    char valorString[100];
    int valorInt = -1;
    float valorFloat = -1.0f;
   
    if (teste <= 5) {
        scan_quote_string(valorString); // Leitura de valor string
    } else if (teste <= 7) {
        scanf("%d", &valorInt); // Leitura de valor int
    } else if (teste == 8) {
        scanf("%f", &valorFloat); // Leitura de valor float
    }

    // Itera sobre todos os registros até o final do arquivo
    // Itera sobre todos os registros até o final do arquivo
    while (!feof(arquivo_binario)) {
       registro = registro_readbin(arquivo_binario);
        if (registro_isValid(registro)) {
            // Verifica se o registro corresponde ao valor buscado
            if (teste <= 5) {
                registro_busca_elemento(valorString, -1, -1.0f, registro);
                    
                
            } else if (teste <= 7) {
                registro_busca_elemento(NULL, valorInt, -1.0f, registro) ;
                    
                
            } else if (teste == 8) {
                registro_busca_elemento(NULL, -1, valorFloat, registro) ;
                   
            }
        }
      
        free(registro); // Libera a memória do registro lido
        cont_registro++;
    }
    
    printf("Numero de paginas de disco: %d\n\n", cabecalho->nroPagDisco);
    free(cabecalho); // Libera o cabeçalho lido
    
    fclose(arquivo_binario); // Fecha o arquivo binário
    return 0;
}


// funcao de inserir

void INSERE(char *nomearqbin, int qnt) {
    // Abre o arquivo binário em modo leitura e escrita ("rb+")
    FILE* arq = fopen(nomearqbin, "rb+");

    // Verifica se o arquivo foi aberto corretamente
    if (arq == NULL) {
        perror("Erro ao abrir o arquivo binário");
        return;
    }

    // Lê o cabeçalho do arquivo para obter metadados (como RRN e registros removidos)
    Cabecalho* cab = cabecalho_readbin(arq);

    // Aloca memória para uma string auxiliar usada para entrada de dados
    char* aux = calloc(200, sizeof(char));
    if (aux == NULL) {
        perror("Erro ao alocar memória para string auxiliar");
        fclose(arq);
        return;
    }

    // Cria um registro vazio para ser preenchido e inserido
    Registro *reg = cria_registro();

    // Loop para inserir `qnt` registros no arquivo
    for (int i = 0; i < qnt; i++) {
        // Loop para ler e atribuir os 10 atributos do registro atual
        for (int j = 0; j < 10; j++) {
            // Lê o valor do atributo usando `scan_quote_string` (provavelmente trata aspas)
            scan_quote_string(aux);

            // Se o valor lido é uma string vazia, o campo é considerado NULO e é ignorado
            if (strcmp(aux, "") == 0) continue;

            // Atribui o valor lido ao campo correspondente, com base em `j`
            switch (j) {
                case 0: registro_setNome(reg, strdup(aux)); break;
                case 1: registro_setDieta(reg, strdup(aux)); break;
                case 2: registro_setHabitat(reg, strdup(aux)); break;
                case 3: registro_setPop(reg, atoi(aux)); break;  // Converte para int
                case 4: registro_setTipo(reg, strdup(aux)); break;
                case 5: registro_setVelocidade(reg, atoi(aux)); break;  // Converte para int
                case 6: registro_setUnimedida(reg, aux[0]); break;  // Usa o primeiro caractere
                case 7: registro_setTam(reg, atof(aux)); break;  // Converte para float
                case 8: registro_setNEspecie(reg, strdup(aux)); break;
                case 9: registro_setAlimenta(reg, strdup(aux)); break;
            }
        }

        // Verifica se há registros removidos logicamente para reaproveitar espaço
        if (cab->nroRegRem > 0) {
            // Posiciona o ponteiro no registro removido mais recente (topo da lista de removidos)
            fseek(arq, 1600 + 160 * cab->topo + 1, SEEK_SET);

            // Lê o próximo RRN disponível no encadeamento da lista de removidos
            fread(&cab->topo, sizeof(int), 1, arq);

            // Retorna para o início do registro removido para sobrescrevê-lo
            fseek(arq, -5, SEEK_CUR);

            // Atualiza o contador de registros removidos
            cab->nroRegRem--;
        } else {
            // Posiciona o ponteiro no próximo registro disponível no fim do arquivo
            fseek(arq, 1600 + 160 * cab->proxRRN, SEEK_SET);

            // Se o próximo registro cria uma nova página (multiplo de 10)
            if (cab->proxRRN % 10 == 0) {
                cab->nroPagDisco++;  // Incrementa o número de páginas usadas
            }

            // Incrementa o próximo RRN disponível
            cab->proxRRN++;
        }

        // Escreve o registro preenchido no arquivo binário
        registro_writebin(arq, reg);

        // Libera a memória dos campos de string do registro após a gravação
        free(registro_getNome(reg));
        free(registro_getDieta(reg));
        free(registro_getHabitat(reg));
        free(registro_getTipo(reg));
        free(registro_getNEspecie(reg));
        free(registro_getAlimenta(reg));
    }

    // Libera a memória da string auxiliar e do registro
    free(aux);
    free(reg);

    // Atualiza o cabeçalho do arquivo com as mudanças realizadas
    cabecalho_writebin(arq, cab);

    // Fecha o arquivo binário
    fclose(arq);

    // Função de verificação do projeto (exibe o conteúdo do arquivo na tela)
    binarioNaTela(nomearqbin);
}


