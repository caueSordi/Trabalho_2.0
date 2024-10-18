#include "cabecalho.h"

/*
#define CABECALHO_STATUS_OK 1
#define CABECALHO_STATUS_INCON 0
*/

Cabecalho* cabecalho_inicializa()
{
    //estrutura padrão do cabecalho
    Cabecalho *cabecalho  = malloc(sizeof(Cabecalho));
    cabecalho->status = CABECALHO_STATUS_INCON;
    cabecalho->topo = -1;
    cabecalho->proxRRN = 0;
    cabecalho->nroRegRem = 0;
    cabecalho->nroPagDisco = 0;
    cabecalho->qttCompacta = 0;

    return cabecalho;
}

// Função para ler o cabeçalho do arquivo binário
Cabecalho* cabecalho_readbin(FILE *entrada) {
    Cabecalho *cabecalho = cabecalho_inicializa();
    
    // Leitura do status do cabeçalho
    if (!fread(&cabecalho->status, sizeof(char), 1, entrada)) {
        cabecalho->status = 'E'; // Indica erro de leitura
        return cabecalho;
    }

    // Leitura dos campos de tamanho fixo do cabeçalho
    fread(&cabecalho->topo, sizeof(int), 1, entrada);
    fread(&cabecalho->proxRRN, sizeof(int), 1, entrada);
    fread(&cabecalho->nroRegRem, sizeof(int), 1, entrada);
    fread(&cabecalho->nroPagDisco, sizeof(int), 1, entrada);
    fread(&cabecalho->qttCompacta, sizeof(int), 1, entrada);
    
    /*/ Exibição dos dados lidos do cabeçalho
    printf("Status: %c\n", cabecalho->status);
    printf("Topo: %d\n", cabecalho->topo);
    printf("Próximo RRN: %d\n", cabecalho->proxRRN);
    printf("Número de Registros Removidos: %d\n", cabecalho->nroRegRem);
    printf("Número de Páginas de Disco: %d\n", cabecalho->nroPagDisco);
    printf("Quantidade de Compactações: %d\n", cabecalho->qttCompacta);
    */
    return cabecalho;
}
bool cabecalho_getStatus(Cabecalho *cabecalho) {
    // Verifica o valor do campo status
    /*if (cabecalho->status == CABECALHO_STATUS_OK) {
        return true; // Retorna verdadeiro se o status for '1'
    } else {
        return false; // Retorna falso caso contrário
    }*/
   return cabecalho->status == CABECALHO_STATUS_OK;
}

void cabecalho_writebin(FILE *file, Cabecalho *cabecalho) {
    // Escreve o cabeçalho no arquivo binário
    if (!cabecalho_getStatus(cabecalho)) {
        // Se o status não for OK, ajusta para '1'
        cabecalho_setStatus(cabecalho, '1');
    }
    fwrite(&cabecalho->status, sizeof(char), 1, file);
    fwrite(&cabecalho->topo, sizeof(int), 1, file);
    fwrite(&cabecalho->proxRRN, sizeof(int), 1, file);
    fwrite(&cabecalho->nroRegRem, sizeof(int), 1, file);
    fwrite(&cabecalho->nroPagDisco, sizeof(int), 1, file);
    fwrite(&cabecalho->qttCompacta, sizeof(int), 1, file);

    

    // Preencher o restante da página de disco com o caractere '$'
    int resto = 1600 - (5 * sizeof(int)) - sizeof(char); // Calcula o espaço restante
    char aux[resto];
    for (int i = 0; i < resto; i++) {
        aux[i] = '$'; // Preenche o array com '$'
    }
    fwrite(aux, sizeof(char), resto, file); // Escreve o preenchimento no arquivo
}

void cabecalho_readcsv(FILE *csv) {
    char buffer[300];
    fgets(buffer, sizeof(buffer), csv); // Pula a linha de cabeçalho
    
}


int cabecalho_getTopo(Cabecalho *cabecalho){
    // verifica o valor do campo topo
    return cabecalho->topo;
}
int cabecalho_getProxRegRem(Cabecalho *cabecalho){
    // verifica o valor do campo proximo a ser removido
    return cabecalho->proxRRN;
}

int cabecalho_getNroPagDisco(Cabecalho *cabecalho){
    // verifica a quantidade de paginas 
    return cabecalho->nroRegRem;
}

int cabecalho_getQttCompacta(Cabecalho *cabecalho){
    // verifica quantas vezes foi compactado
    return cabecalho->qttCompacta;
}

void cabecalho_setStatus(Cabecalho *cabecalho, char status){
    cabecalho->status = status;
}

void cabecalho_setTopo(Cabecalho *cabecalho, int topo){
    cabecalho->topo = topo;
}

void cabecalho_setProxRRN(Cabecalho *cabecalho, int proxRRN){
    cabecalho->proxRRN = proxRRN;
}

void cabecalho_setNroPagDisco(Cabecalho *cabecalho, int nropag){
    cabecalho->nroPagDisco = nropag;
}

void cabecalho_setQttCompacta(Cabecalho *cabecalho, int quant){
    cabecalho->qttCompacta = quant;
}

void INSERE(char *nomearqbin, int qnt){
    // Abre o arquivo lendo o cabeçalho
    FILE* arq = fopen(nomearqbin, "rb+");

    // Se deu erro
    if (arq == NULL) {
        // Sai pois o erro foi avisado em AbrirArquivo
        return;
    }

    // Lê o cabeçalho do arquivo
    Cabecalho* cab = cabecalho_readbin(arq);

    // String auxiliar
    char* aux = calloc(200, sizeof(char));
    // Registro para armazenar os dados
    Registro *reg = cria_registro();

    // Para cada registro a ser inserido
    for (int i = 0; i < qnt; i++) {
        // Para cada atributo do registro
        for (int j = 0; j < 10; j++) {
            // Lê o valor do atributo
            scan_quote_string(aux);
            // Se foi digitado NULO
            if (strcmp(aux, "") == 0)
                // Avança para o próximo atributo
                continue;

            // Para escolher qual atributo definir
            switch (j) {
                case 0:
                    registro_setNome(reg, strdup(aux));
                    break;
                case 1:
                    registro_setDieta(reg, strdup(aux));
                    break;
                case 2:
                    registro_setHabitat(reg, strdup(aux));
                    break;
                case 3:
                    registro_setPop(reg, atoi(aux));
                    break;
                case 4:
                    registro_setTipo(reg, strdup(aux));
                    break;
                case 5:
                    registro_setVelocidade(reg, atoi(aux));
                    break;
                case 6:
                    registro_setUnimedida(reg, aux[0]);
                    break;
                case 7:
                    registro_setTam(reg, atof(aux));
                    break;
                case 8:
                    registro_setNEspecie(reg, strdup(aux));
                    break;
                case 9:
                    registro_setAlimenta(reg, strdup(aux));
                    break;
            }
        }

        // Se um registro já foi removido logicamente
        if (cab->nroRegRem > 0) {
            // Pega o começo do atributo de encadeamento do último registro removido logicamente
            fseek(arq, 1600 + 160 * cab->topo + 1, SEEK_SET);
            // Pega o RRN do registro removido logicamente anterior
            fread(&cab->topo, sizeof(int), 1, arq);
            // Vai para o começo do registro
            fseek(arq, -5, SEEK_CUR);
            // Define que há um registro removido logicamente a menos
            cab->nroRegRem--;
        } else {
            // Pega o começo do registro a ser inserido no fim
            fseek(arq, 1600 + 160 * cab->proxRRN, SEEK_SET);

            // Se começará uma nova página
            if (cab->proxRRN % 10 == 0)
                // Define que há uma nova página
                cab->nroPagDisco++;

            // Define que o último registro tem um RRN maior
            cab->proxRRN++;
        }

        // Salva o novo registro no local definido
        registro_writebin(arq, reg);

        // Libera o espaço alocado e cria um novo registro nulo
        free(registro_getNome(reg));
        free(registro_getDieta(reg));
        free(registro_getHabitat(reg));
        free(registro_getTipo(reg));
        free(registro_getNEspecie(reg));
        free(registro_getAlimenta(reg));
    }

    // Libera os espaços alocados
    free(aux);
    free(reg);

    // Fecha o arquivo, atualizando o cabeçalho
    cabecalho_writebin(arq, cab);
    fclose(arq);

    // Função de verificação do projeto
    binarioNaTela(nomearqbin);
}