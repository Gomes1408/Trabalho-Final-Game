// Trabalho Final ERE - Gabriel Castelo Branco Gomes e Tiago Torres Schmidt.

#include <stdio.h> //Bibliotecas utilizadas no projeto. Projeto desenvolvido utilizando Allegro.
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>

#define KEY_SEEN     1//Constantes utilizadas no projeto.
#define KEY_RELEASED 2
#define N_LINHAS 11
#define N_COLUNAS 27
#define TEMPO_ESPERA 20
#define ESCALA 50
#define SAVEFILE_ID "currentSave.bin"
#define MAP "mapa"
#define MAPENDFILE_ID ".txt"
#define MAX_LEVEL 99

typedef struct//Estrutura obj, base do jogo. 'obj' serve como um molde para cada elemento do jogo. Ou seja, em determinado ponto da execução, a matriz de caracteres é convertida em uma matriz de obj.
{
        int body;//Propriedades dos objetos do jogo. Nem todos os elementos tem todas propriedades definidas.
        int shiftable;
        int state;
        int colectable;
        int player;
        int cd;
        char identity;
        char lastStep;
}obj;

typedef struct//Estrutura mapState, utilizada para salvar o jogo.
{
        obj objImage[N_LINHAS][N_COLUNAS];// Salva todas as propriedades interessantes para o jogo.
        char src[N_LINHAS][N_COLUNAS];
        int cScoreCounter;
        int cHp;
        int cLeverCooldown;
        int cImortalCooldown;
        int cHasSword;
        int cLevel;
        int cLeverS;
        int cPlayerS;
        float cX, cY;
}mapState;

void inicializa(bool teste, const char *descricao)//Função de inicialização geral, para os adicionais do Allegro.
{
    if(teste) return;

    printf("Não foi possivel inicializar %s\n", descricao);
    exit(1);
}

void loadMap(char src[N_LINHAS][N_COLUNAS], obj objImage[N_LINHAS][N_COLUNAS], float* x, float* y)//Função loadMap, converte uma matriz de caracteres em uma matriz de obj.
{
    int i,j;
    for(i=0;i<N_COLUNAS;i++)
    {
        for(j=0;j<N_LINHAS;j++)
        {
            switch(src[j][i])
            {
                case '#':
                    objImage[j][i].body = 1;
                    objImage[j][i].colectable = 0;
                    objImage[j][i].player = 0;
                    objImage[j][i].shiftable = 0;
                    objImage[j][i].identity = '#';
                    break;
                case 'C':
                    objImage[j][i].body = 0;
                    objImage[j][i].colectable = 1;
                    objImage[j][i].player = 0;
                    objImage[j][i].shiftable = 0;
                    objImage[j][i].identity = 'C';
                    objImage[j][i].lastStep = 'C';
                    break;
                case 'A':
                    objImage[j][i].body = 0;
                    objImage[j][i].colectable = 0;
                    objImage[j][i].player = 0;
                    objImage[j][i].shiftable = 1;
                    objImage[j][i].identity = 'A';
                    break;
                case 'D':
                    objImage[j][i].body = 1;
                    objImage[j][i].colectable = 0;
                    objImage[j][i].player = 0;
                    objImage[j][i].shiftable = 1;
                    objImage[j][i].identity = 'D';
                    break;
                case 'J':
                    objImage[j][i].body = 0;
                    objImage[j][i].colectable = 0;
                    objImage[j][i].player = 1;
                    objImage[j][i].shiftable = 0;
                    objImage[j][i].identity = 'J';
                    objImage[j][i].lastStep = 'J';
                    *y = j*ESCALA;
                    *x = i*ESCALA;
                    break;
                case 'M':
                    objImage[j][i].body = 0;
                    objImage[j][i].colectable = 1;
                    objImage[j][i].player = 0;
                    objImage[j][i].shiftable = 0;
                    objImage[j][i].identity = 'M';
                    objImage[j][i].lastStep = 'M';
                    break;
                case 'B':
                    objImage[j][i].body = 0;
                    objImage[j][i].colectable = 0;
                    objImage[j][i].player = 0;
                    objImage[j][i].shiftable = 1;
                    objImage[j][i].identity = 'B';
                    break;
                case 'O':
                    objImage[j][i].body = 0 ;
                    objImage[j][i].colectable = 0;
                    objImage[j][i].player = 0;
                    objImage[j][i].shiftable = 0;
                    objImage[j][i].identity = 'O';
                    objImage[j][i].cd = 0;
                    break;
            }
        }
    }
}

int collectItem(int counter,obj objImage[N_LINHAS][N_COLUNAS], int x, int y, int* imortalCooldown)//Função collectItem, ativada para coletar moedas ou chaves.
{
    if(objImage[(int)y/ESCALA][(int)x/ESCALA].identity == 'M')
    {
        counter += 10;//Aumenta a pontuação.
    }
    if(counter % 100 == 0)
    {
        *imortalCooldown = (15 * 5);//Sistema de imortalidade.
    }
    objImage[(int)y/ESCALA][(int)x/ESCALA].identity = ' ';//Transforma a posição da moeda em nada, "excluindo-a".
    objImage[(int)y/ESCALA][(int)x/ESCALA].body = 0;
    objImage[(int)y/ESCALA][(int)x/ESCALA].colectable = 0;
    objImage[(int)y/ESCALA][(int)x/ESCALA].player = 0;
    objImage[(int)y/ESCALA][(int)x/ESCALA].shiftable = 0;
    objImage[(int)y/ESCALA][(int)x/ESCALA].lastStep = ' ';

    return counter;
}

void leverActivate(int* cd,obj objImage[N_LINHAS][N_COLUNAS], int* isLever)//Função leverActivate, realiza a mecânica de portas.
{
    int i,j;
    for(i=0;i<N_COLUNAS;i++)//Procura todas as portas do jogo, e inverte o estado de cada uma.
    {
        for(j=0;j<N_LINHAS;j++)
        {
            if(objImage[j][i].identity == 'A')
            {
                objImage[j][i].identity = 'D';
                objImage[j][i].body = 1;
                *cd = TEMPO_ESPERA;
            }
            else if(objImage[j][i].identity == 'D')
            {
                objImage[j][i].identity = 'A';
                objImage[j][i].body = 0;
                *cd = TEMPO_ESPERA;
            }
        }
    }

    if(*isLever)//Controle de animação da alavanca.
    {
        *isLever = 0;
    }else
    {
        *isLever = 1;
    }
    
}

void updateOgre(obj objImage[N_LINHAS][N_COLUNAS], int randomItem)//Função updateOgre, em geral, é a "inteligência artifical" dos ogros.
{
    int i,j;
    for(i=0;i<N_COLUNAS;i++)
    {
        for(j=0;j<N_LINHAS;j++)
        {
            if((objImage[j][i].identity == 'O')&&(objImage[j][i].cd == 0))//Procura todos os Ogros sem tempo de recarga do mapa.
            {
                switch (randomItem)//Através de uma variável aleatória para direção, executa.
                {
                    case 0:
                        if((objImage[j][i-1].body == 0) && (objImage[j][i-1].identity != 'A')&& (objImage[j][i-1].identity != 'O') && (objImage[j][i-1].identity != 'D') && (objImage[j][i-1].identity != 'B') && ((i-1)*ESCALA > 00))
                        {//Teste condicionais de passagem, ou seja, verifica se é possível passar pelo obj adjacente.
                            objImage[j][i-1].lastStep = objImage[j][i-1].identity;
                            

                            objImage[j][i].body = objImage[j][i-1].body;//A logística do movimento do Ogro é seguinte: Ele não anda verdadeiramente, ele transforma o bloco ao lado em ogro e retorna a antiga propriedade do bloco onde estava.
                            objImage[j][i].colectable = objImage[j][i-1].colectable;
                            objImage[j][i].player = objImage[j][i-1].player;
                            objImage[j][i].shiftable =  objImage[j][i-1].shiftable;                            
                            objImage[j][i].identity = objImage[j][i].lastStep;
                                                 

                            objImage[j][i-1].body = 0;
                            objImage[j][i-1].colectable = 0;
                            objImage[j][i-1].player = 0;
                            objImage[j][i-1].shiftable = 0;
                            objImage[j][i-1].identity = 'O';
                            objImage[j][i-1].cd = TEMPO_ESPERA;
                        }
                        break;
                    case 1:
                        if((objImage[j-1][i].body == 0) && (objImage[j-1][i].identity != 'A') && (objImage[j-1][i].identity != 'O') && (objImage[j-1][i].identity != 'D') && (objImage[j-1][i].identity != 'B')  && ((j-1)*ESCALA > 00))
                        {
                            objImage[j-1][i].lastStep = objImage[j-1][i].identity;

                            objImage[j][i].body = objImage[j-1][i].body;
                            objImage[j][i].colectable = objImage[j-1][i].colectable;
                            objImage[j][i].player = objImage[j-1][i].player;
                            objImage[j][i].shiftable =  objImage[j-1][i].shiftable;                          
                            objImage[j][i].identity = objImage[j][i].lastStep;
                          
                            objImage[j-1][i].body = 0;
                            objImage[j-1][i].colectable = 0;
                            objImage[j-1][i].player = 0;
                            objImage[j-1][i].shiftable = 0;
                            objImage[j-1][i].identity = 'O';
                            objImage[j-1][i].cd = TEMPO_ESPERA;
                        }
                        break;
                    case 2:
                        if((objImage[j][i+1].body == 0 ) && (objImage[j][i+1].identity != 'A') && (objImage[j][i+1].identity != 'D') && (objImage[j][i+1].identity != 'O') && (objImage[j][i+1].identity != 'B') && ((i+1)*ESCALA < N_COLUNAS*ESCALA))
                        {
                            objImage[j][i+1].lastStep = objImage[j][i+1].identity;

                            objImage[j][i].body = objImage[j][i+1].body;
                            objImage[j][i].colectable = objImage[j][i+1].colectable;
                            objImage[j][i].player = objImage[j][i+1].player;
                            objImage[j][i].shiftable =  objImage[j][i+1].shiftable;                           
                            objImage[j][i].identity = objImage[j][i].lastStep;
                            
                            
                            objImage[j][i+1].body = 0;
                            objImage[j][i+1].colectable = 0;
                            objImage[j][i+1].player = 0;
                            objImage[j][i+1].shiftable = 0;
                            objImage[j][i+1].identity = 'O';
                            objImage[j][i+1].cd = TEMPO_ESPERA;
                        }
                        break;
                    case 3:
                        if((objImage[j+1][i].body == 0) && (objImage[j+1][i].identity != 'A') && (objImage[j+1][i].identity != 'D') && (objImage[j+1][i].identity != 'O') &&(objImage[j+1][i].identity != 'B') && ((j+1)*ESCALA < N_LINHAS*ESCALA))
                        {
                            objImage[j+1][i].lastStep = objImage[j+1][i].identity;

                            objImage[j][i].body = objImage[j+1][i].body;
                            objImage[j][i].colectable = objImage[j+1][i].colectable;
                            objImage[j][i].player = objImage[j+1][i].player;
                            objImage[j][i].shiftable =  objImage[j+1][i].shiftable;                            
                            objImage[j][i].identity = objImage[j][i].lastStep;
                            
                            
                            objImage[j+1][i].body = 0;
                            objImage[j+1][i].colectable = 0;
                            objImage[j+1][i].player = 0;
                            objImage[j+1][i].shiftable = 0;
                            objImage[j+1][i].identity = 'O';
                            objImage[j+1][i].cd = TEMPO_ESPERA;
                        }
                        break;
                }
            }else if(objImage[j][i].cd > 0)//Reseta o tempo de recarga dos ogros.
            {
                objImage[j][i].cd--;
            }
        }
    }
}

void ogreHit(obj objImage[N_LINHAS][N_COLUNAS],float* pX,float* pY,char src[N_LINHAS][N_COLUNAS],int* playerHp,int* counter,int* menu, int* imortalCooldown, int* currentLevel, int* leverCooldown, int *hasSword)
{//Função ogreHit, executa o sistema de ataque e vida do jogo. É responsável pelo ataque do ogro E do herói.
    int i,j,k,l;
    for(i=0;i<N_COLUNAS;i++)
    {
        for(j=0;j<N_LINHAS;j++)
        {
            if((objImage[j][i].identity == 'O')&&(*pX/ESCALA == i)&&(*pY/ESCALA == j))//Se os dois estão no mesmo local,
            {
                if(*hasSword || *imortalCooldown > 0)//Se o player tem a espada, ou está imortal.
                {
                    objImage[(int)*pY/ESCALA][(int)*pX/ESCALA].identity = ' ';//Mata ogro.
                    objImage[(int)*pY/ESCALA][(int)*pX/ESCALA].body = 0;
                    objImage[(int)*pY/ESCALA][(int)*pX/ESCALA].colectable = 0;
                    objImage[(int)*pY/ESCALA][(int)*pX/ESCALA].player = 0;
                    objImage[(int)*pY/ESCALA][(int)*pX/ESCALA].shiftable = 0;
                    *counter += 100;
                }else
                {//Se não, mata o player.
                    if(*imortalCooldown == 0)
                    {
                        for(k=0;k<N_COLUNAS;k++)
                        {
                            for(l=0;l<N_LINHAS;l++)
                            {
                                if(src[l][k] == 'J')
                                {
                                    *pX = k * ESCALA;//Reseta a posição através da src.
                                    *pY = l * ESCALA;
                                }
                            }
                        }

                        *playerHp -= 1;//Reduz o hp.

                        if(*playerHp == 0)
                        {
                            *menu = 1;//Reseta o game.
                            loadMap(src,objImage,pX,pY);
                            *playerHp = 3;
                            *counter = 0;
                            *currentLevel = 1;                            
                            *leverCooldown = 00;
                            *imortalCooldown = 00;
                            *hasSword = 0;                            
                        }
                    }
                }
            }
        }
    }
}

void giveSword(obj objImage[N_LINHAS][N_COLUNAS], int* hasSword)//Função giveSword, libera a espada para o guerreiro.
{
    int i,j;
    int totalCoins = 0;
    for(i=0;i<N_COLUNAS;i++)
    {
        for(j=0;j<N_LINHAS;j++)
        {
            if(objImage[j][i].identity == 'M')
            {
                totalCoins++;
            }
        }
    }
    if(totalCoins == 0)
    {
        *hasSword = 1;
    }
}

void drawMap(obj objImage[N_LINHAS][N_COLUNAS], int isLever)//Função drawMap, na verdade, é um apanhado dos recursos visuais da Allegro. Em geral, estão aqui todos os possíveis. Ou seja, os outros desenhos da Allegro não foram possíveis de ser retirado da main.
{
    int i,j;

    ALLEGRO_BITMAP* floorI = al_load_bitmap("images/floorF.png");
    ALLEGRO_BITMAP* wallI = al_load_bitmap("images/wallF.png");
    ALLEGRO_BITMAP* coinI = al_load_bitmap("images/coinF.png");
    ALLEGRO_BITMAP* leverI = al_load_bitmap("images/leverF.png");
    ALLEGRO_BITMAP* leverII = al_load_bitmap("images/leverIF.png");
    ALLEGRO_BITMAP* doorI = al_load_bitmap("images/doorF.png");  
    ALLEGRO_BITMAP* keyI = al_load_bitmap("images/keyF.png");
    ALLEGRO_BITMAP* ogreI = al_load_bitmap("images/ogreF.png");    

    for(i=0;i<N_COLUNAS;i++)//Carrega as imagens e desenha cada elemento.
    {
        for(j=0;j<N_LINHAS;j++)
        {
            switch(objImage[j][i].identity)
            {//Esses códigos comentados são, na verdade, a versão sem sprites.
                case '#':
                    //al_draw_filled_rectangle(i*ESCALA, j*ESCALA, i*ESCALA + ESCALA-1 , j*ESCALA + ESCALA-1 , al_map_rgb(172, 172, 172));
                    al_draw_bitmap(wallI, i*ESCALA, j*ESCALA, 0);
                    break;
                case 'J':
                    //al_draw_filled_rectangle(i*ESCALA, j*ESCALA, i*ESCALA + ESCALA-1 , j*ESCALA + ESCALA-1 , al_map_rgb(0, 0, 0));  
                    al_draw_bitmap(floorI, i*ESCALA, j*ESCALA, 0);                   
                    break;
                case 'C':
                    //al_draw_filled_rectangle(i*ESCALA, j*ESCALA, i*ESCALA + ESCALA-1 , j*ESCALA + ESCALA-1 , al_map_rgb(255,165,0));
                    al_draw_bitmap(keyI, i*ESCALA, j*ESCALA, 0);       
                    break;
                case 'M':
                    //al_draw_filled_rectangle(i*ESCALA, j*ESCALA, i*ESCALA + ESCALA-1 , j*ESCALA + ESCALA-1 , al_map_rgb(255, 247, 0));
                    al_draw_bitmap(coinI, i*ESCALA, j*ESCALA, 0);
                    break;
                case 'B':
                    //al_draw_filled_rectangle(i*ESCALA, j*ESCALA, i*ESCALA + ESCALA-1 , j*ESCALA + ESCALA-1 , al_map_rgb(139,69,19));
                    if(isLever)
                    {
                        al_draw_bitmap(leverII, i*ESCALA, j*ESCALA, 0);
                    }else
                    {
                        al_draw_bitmap(leverI, i*ESCALA, j*ESCALA, 0);    
                    }                   
                    break;
                case 'O':
                    //al_draw_filled_rectangle(i*ESCALA, j*ESCALA, i*ESCALA + ESCALA-1 , j*ESCALA + ESCALA-1 , al_map_rgb(0,179,30));           
                    al_draw_bitmap(ogreI, i*ESCALA, j*ESCALA, 0);          
                    break;
                case 'A':
                    //al_draw_filled_rectangle(i*ESCALA, j*ESCALA, i*ESCALA + ESCALA-1 , j*ESCALA + ESCALA-1 , al_map_rgb(56 , 176, 222));
                    al_draw_bitmap(floorI, i*ESCALA, j*ESCALA, 0);
                    break;
                case 'D':
                    //al_draw_filled_rectangle(i*ESCALA, j*ESCALA, i*ESCALA + ESCALA-1 , j*ESCALA + ESCALA-1 , al_map_rgb(255 , 0, 0));
                    al_draw_bitmap(doorI, i*ESCALA, j*ESCALA, 0);
                    break;
                case ' ':
                    //al_draw_filled_rectangle(i*ESCALA, j*ESCALA, i*ESCALA + ESCALA-1 , j*ESCALA + ESCALA-1 , al_map_rgb(0, 0, 0));
                    al_draw_bitmap(floorI, i*ESCALA, j*ESCALA, 0);  
                    break;
                default:
                    //al_draw_filled_rectangle(i*ESCALA, j*ESCALA, i*ESCALA + ESCALA-1 , j*ESCALA + ESCALA-1 , al_map_rgb(0, 0, 0));
                    al_draw_bitmap(floorI, i*ESCALA, j*ESCALA, 0);  
                    break;
            }
        }
    }
}

void saveMap(FILE *arq, obj objImage[N_LINHAS][N_COLUNAS], char src[N_LINHAS][N_COLUNAS], int cScoreCounter, int cHp, int cLeverCooldown, int cImortalCooldown, int cHasSword, float cX, float cY, int cLevel, int isLever, int isInverted)
{
    mapState cMapState;//Cria um objeto com estado do mapa, e o salva.

    cMapState.cHasSword = cHasSword;
    cMapState.cImortalCooldown = cImortalCooldown;
    cMapState.cLeverCooldown = cLeverCooldown;
    cMapState.cHp = cHp;
    cMapState.cScoreCounter = cScoreCounter;
    cMapState.cX = cX;
    cMapState.cY = cY;
    cMapState.cLevel = cLevel;
    cMapState.cLeverS = isLever;
    cMapState.cPlayerS = isInverted;

    memcpy(cMapState.src,src,N_LINHAS*N_COLUNAS*sizeof(char));
    memcpy(cMapState.objImage,objImage,N_LINHAS*N_COLUNAS*sizeof(obj));

    arq = fopen(SAVEFILE_ID,"wb");//Abertura de arquivo, gravação e sinalização.
    if(arq == NULL){
        printf("Erro na abertura do arquivo dos funcionários.");
    }else{
        if(fwrite(&cMapState,sizeof(mapState),1,arq) != 1){
            printf("Erro na escrita.");
        }
    }
    fclose(arq);
}

void loadSave(FILE *arq, obj objImage[N_LINHAS][N_COLUNAS], char src[N_LINHAS][N_COLUNAS], int* cScoreCounter, int* cHp, int* cLeverCooldown, int* cImortalCooldown, int* cHasSword, float* cX, float* cY, int* cLevel, int* cIsLever, int* cIsInverted)
{//Função loadSave, carrega o save binário do game, através de uma máscara mapState.
    mapState cMapState;

    arq = fopen(SAVEFILE_ID,"rb+");//Abre o stream do arquivo.

    if(fread(&cMapState,sizeof(mapState),1,arq)){
        *cHasSword = cMapState.cHasSword;
        *cImortalCooldown = cMapState.cImortalCooldown;
        *cLeverCooldown = cMapState.cLeverCooldown;
        *cHp = cMapState.cHp;
        *cScoreCounter = cMapState.cScoreCounter;
        *cX = cMapState.cX;
        *cY = cMapState.cY;
        *cLevel = cMapState.cLevel;
        *cIsLever = cMapState.cLeverS;
        *cIsInverted = cMapState.cPlayerS;

        memcpy(src,cMapState.src,N_LINHAS*N_COLUNAS*sizeof(char));
        memcpy(objImage,cMapState.objImage,N_LINHAS*N_COLUNAS*sizeof(obj));
    }

    fclose(arq);
}

void readMap(FILE *arq,  char src[N_LINHAS][N_COLUNAS], int* cLevel, char mapFile[10])
{//Função readMap, lê o arquivo .txt para converter em um src.
    int i,j;
    char mapNumber[3];   
    
       
    strcpy(mapFile,MAP);
    snprintf(mapNumber,sizeof(mapNumber),"%02d", *cLevel);
    strcat(mapFile,mapNumber);
    strcat(mapFile,MAPENDFILE_ID);//Diversas concatenações para definir o nome do arquivo de leitura.

    if(!(arq = fopen(mapFile,"rb+"))) //Abre o stream do arquivo.
    {
        *cLevel = 1;
        memset(mapFile, 0, 10);
        memset(mapNumber, 0, sizeof(mapNumber));        
        strcpy(mapFile,MAP);
        snprintf(mapNumber,sizeof(mapNumber),"%02d", *cLevel);
        strcat(mapFile,mapNumber);
        strcat(mapFile,MAPENDFILE_ID);
        arq = fopen(mapFile,"rb+");
    }

    for(i=0;i<N_LINHAS;i++)
    {
        for(j=0;j<N_COLUNAS;j++)
        {
            src[i][j] = fgetc(arq);//Define a src.
            if(j == 26){
                fseek(arq,2,SEEK_CUR);
            }
        }
    }
    fclose(arq);
}

int checkLevel(obj objImage[N_LINHAS][N_COLUNAS], int* levelNumber)
{//Função checkLevel, encerra a fase e sinaliza o fim da fase.
    int totalKeys = 0;
    int i,j;
    int intReturn = 0;
    for(i=0;i<N_LINHAS;i++)
    {
        for(j=0;j<N_COLUNAS;j++)
        {
            if(objImage[i][j].identity == 'C')
            {
                totalKeys++;
            }
        }
    }

    if(totalKeys == 0)
    {
        *levelNumber += 1;
        intReturn = 1;
    }

    return intReturn;
}

int main()//Função main, executa o projeto.
{
    srand(time(NULL));//Variáveis principais do projeto.
    int randomItem;
    int i,j;
    int onMenu = 1;
    int scoreCounter = 00;
    int hp = 3;
    int leverCooldown;
    int imortalCooldown;
    int hasSword = 0;
    int currentLevel = 1;
    int isInverted = 0;
    int isLeverActivated = 0;
    bool done = false;
    bool redraw = true;
    float x, y;
    FILE *saveLocation;
    unsigned char tecla[ALLEGRO_KEY_MAX];    
    ALLEGRO_EVENT event;
    char map[N_LINHAS][N_COLUNAS];
    char mapFile[10];
    obj objImage[N_LINHAS][N_COLUNAS];

    inicializa(al_init(), "Allegro.");//Inicialização da Allegro...
    inicializa(al_install_keyboard(), "Teclado.");
    ALLEGRO_TIMER* timer = al_create_timer(1.0 / 15.0);//Framerate e velocidade do jogo.
    inicializa(timer, "Temporizador.");
    ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();
    inicializa(queue, "Fila de eventos");

    al_set_new_display_option(ALLEGRO_SAMPLE_BUFFERS, 1, ALLEGRO_SUGGEST);
    al_set_new_display_option(ALLEGRO_SAMPLES, 8, ALLEGRO_SUGGEST);
    al_set_new_bitmap_flags(ALLEGRO_MIN_LINEAR | ALLEGRO_MAG_LINEAR);

    ALLEGRO_DISPLAY* disp = al_create_display(ESCALA*N_COLUNAS , N_LINHAS*ESCALA);//Tamanho do display.
    inicializa(disp, "Tela.");
    ALLEGRO_FONT* font = al_create_builtin_font();
    inicializa(font, "Fonte.");
    inicializa(al_init_primitives_addon(), "Adicionais primitivos.");
    inicializa(al_init_image_addon(), "Adicional de imagem.");

    ALLEGRO_BITMAP* heroI = al_load_bitmap("images/heroF.png");//Inicialização de sprites gerais do jogo.
    ALLEGRO_BITMAP* heroGI = al_load_bitmap("images/heroGF.png");
    ALLEGRO_BITMAP* heroRI = al_load_bitmap("images/heroRF.png");

    ALLEGRO_BITMAP* heroII = al_load_bitmap("images/heroIF.png");
    ALLEGRO_BITMAP* heroGII = al_load_bitmap("images/heroIGF.png");
    ALLEGRO_BITMAP* heroRII = al_load_bitmap("images/heroIRF.png");

    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_display_event_source(disp));
    al_register_event_source(queue, al_get_timer_event_source(timer));

    memset(tecla, 0, sizeof(tecla));

    al_start_timer(timer);
    while(1) //Loop do game.
    {
        if(onMenu)//If para uso de menu.
        {
            al_wait_for_event(queue, &event);

            switch(event.type)//Switch para recepção do evento acionado pelo usuário.
            {
                case ALLEGRO_EVENT_TIMER:
                    if(tecla[ALLEGRO_KEY_N])//Opção de novo jogo.
                    {
                        currentLevel = 1;                        
                        readMap(saveLocation,map,&currentLevel,mapFile);
                        loadMap(map,objImage,&x,&y);
                        scoreCounter = 00;
                        hp = 3;
                        leverCooldown = 00;
                        imortalCooldown = 00;
                        hasSword = 0;
                        onMenu = 0;
                    }

                    if(tecla[ALLEGRO_KEY_C])//Opção de carregar o jogo.
                    {
                        loadSave(saveLocation,objImage,map,&scoreCounter,&hp,&leverCooldown,&imortalCooldown,&hasSword,&x,&y,&currentLevel, &isLeverActivated, &isInverted);
                        onMenu = 0;
                    }

                    if(tecla[ALLEGRO_KEY_S])//Opção de salvar jogo.
                    {
                        saveMap(saveLocation,objImage,map,scoreCounter,hp,leverCooldown,imortalCooldown,hasSword,x,y,currentLevel,isLeverActivated,isInverted);
                    }

                    if(tecla[ALLEGRO_KEY_V])//Opção de voltar para o jogo.
                    {
                        onMenu = 0;
                    }

                    if(tecla[ALLEGRO_KEY_Q])//Opção de sair do jogo.
                    {
                        done = true;
                    }

                    for(int i = 0; i < ALLEGRO_KEY_MAX; i++)//Sistema de leitura para duas teclas.
                    {
                        tecla[i] &= KEY_SEEN;
                    }


                    redraw = true;
                    break;

                case ALLEGRO_EVENT_KEY_DOWN:
                    tecla[event.keyboard.keycode] = KEY_SEEN | KEY_RELEASED;
                    break;

                case ALLEGRO_EVENT_KEY_UP:
                    tecla[event.keyboard.keycode] &= KEY_RELEASED;
                    break;

                case ALLEGRO_EVENT_DISPLAY_CLOSE:
                    done = true;
                    break;
            }

            if(done){
                break;
            }

            if(redraw && al_is_event_queue_empty(queue))
            {
                al_clear_to_color(al_map_rgb(0, 0, 0));//Visualização do menu.
                al_draw_textf(font, al_map_rgb(255, 255, 255), 8*ESCALA, 5*ESCALA, 0, "Novo Jogo(N)|");
                al_draw_textf(font, al_map_rgb(255, 255, 255), 10.25*ESCALA, 5*ESCALA, 0, "Carregar Jogo(C)|");
                al_draw_textf(font, al_map_rgb(255, 255, 255), 13.25*ESCALA, 5*ESCALA, 0, "Salvar Jogo(S)|");
                al_draw_textf(font, al_map_rgb(255, 255, 255), 16*ESCALA, 5*ESCALA, 0, "Sair do Jogo(Q)|");
                al_draw_textf(font, al_map_rgb(255, 255, 255), 19*ESCALA, 5*ESCALA, 0, "Voltar(V)");
                al_draw_textf(font, al_map_rgb(255, 255, 255), 24*ESCALA, 0, 0, "Pontuação: %d",scoreCounter);
                al_draw_textf(font, al_map_rgb(255, 255, 255), 5*ESCALA, 0, 0, "%s",mapFile);
                al_flip_display();

                redraw = false;
            }

        }else
        {//Loop do próprio jogo.
            randomItem =  rand() % 4;//Item randômico para os ogros.
            al_wait_for_event(queue, &event);

            if(checkLevel(objImage, &currentLevel))//Primeira atividade: checkar o nivel do jogo, se sim, avança o mapa.
            {
                if(currentLevel > MAX_LEVEL)
                {
                    currentLevel = 1;//Mecanismo para 99 níveis.
                }                                
                readMap(saveLocation,map,&currentLevel, mapFile);
                loadMap(map,objImage,&x,&y);
                hp = 3;
                leverCooldown = 00;
                imortalCooldown = 00;
                hasSword = 0;
            }

            updateOgre(objImage,randomItem);//Realiza a inteligênia dos ogros.

            ogreHit(objImage,&x,&y,map,&hp,&scoreCounter,&onMenu,&imortalCooldown,&currentLevel,&leverCooldown,&hasSword);//Verifica o sistema de hits.

            giveSword(objImage,&hasSword);//Verifica o sistema de espadas.

            if((objImage[(int)y/ESCALA][(int)x/ESCALA].identity == 'M') || (objImage[(int)y/ESCALA][(int)x/ESCALA].identity == 'C'))
            {
                scoreCounter = collectItem(scoreCounter,objImage, x, y, &imortalCooldown);//Sistema para coletar itens.
            }

            if(leverCooldown > 0)
            {
                leverCooldown--;//Reseta o tempo de recarga das alavancas.
            }

            if(imortalCooldown > 0)
            {
                imortalCooldown--;//Reseta o tempo de imortalidade.
            }

            switch(event.type)//Sistema de leitura de movimentação de ativação de alavancas.
            {
                case ALLEGRO_EVENT_TIMER:
                    if(tecla[ALLEGRO_KEY_UP])
                    {
                        if(y == 0 || (objImage[(int)(y-ESCALA)/ESCALA][(int)x/ESCALA].body == 1)){}else
                        {
                            y-=ESCALA;
                        }
                    }

                    if(tecla[ALLEGRO_KEY_DOWN])
                    {
                        if(y == 10*ESCALA || (objImage[(int)(y+ESCALA)/ESCALA][(int)x/ESCALA].body == 1)){}else
                        {
                            y+=ESCALA;
                        }
                    }

                    if(tecla[ALLEGRO_KEY_LEFT])
                    {
                        if(x == 0 || (objImage[(int)y/ESCALA][(int)(x-ESCALA)/ESCALA].body == 1)){}else
                        {
                            x-=ESCALA;
                            isInverted = 1;
                        }
                    }

                    if(tecla[ALLEGRO_KEY_RIGHT])
                    {
                        if(x == 26*ESCALA || (objImage[(int)y/ESCALA][(int)(x+ESCALA)/ESCALA].body == 1)){}else
                        {
                            x+=ESCALA;
                            isInverted = 0;
                        }
                    }

                    if(tecla[ALLEGRO_KEY_ESCAPE])
                    {
                        done = true;
                    }

                    if(tecla[ALLEGRO_KEY_TAB])
                    {
                        onMenu = 1;
                    }

                    if((tecla[ALLEGRO_KEY_B] && ((objImage[(int)y/ESCALA][(int)x/ESCALA].identity == 'B') || (objImage[(int)(y-ESCALA)/ESCALA][(int)x/ESCALA].identity == 'B') || (objImage[(int)(y+ESCALA)/ESCALA][(int)x/ESCALA].identity == 'B') || (objImage[(int)y/ESCALA][(int)(x-ESCALA)/ESCALA].identity == 'B')|| (objImage[(int)y/ESCALA][(int)(x+ESCALA)/ESCALA].identity == 'B'))) && (leverCooldown == 0))
                    {
                        leverActivate(&leverCooldown, objImage, &isLeverActivated);
                    }

                    for(int i = 0; i < ALLEGRO_KEY_MAX; i++)
                    {
                        tecla[i] &= KEY_SEEN;
                    }


                    redraw = true;
                    break;

                case ALLEGRO_EVENT_KEY_DOWN:
                    tecla[event.keyboard.keycode] = KEY_SEEN | KEY_RELEASED;
                    break;

                case ALLEGRO_EVENT_KEY_UP:
                    tecla[event.keyboard.keycode] &= KEY_RELEASED;
                    break;

                case ALLEGRO_EVENT_DISPLAY_CLOSE:
                    done = true;
                    break;
            }

            if(done){
                break;
            }

            if(redraw && al_is_event_queue_empty(queue))//Sistema para projeção dos gráficos.
            {
                al_clear_to_color(al_map_rgb(0, 0, 0));
                drawMap(objImage, isLeverActivated);
                if(hasSword)
                {
                    al_draw_textf(font, al_map_rgb(255, 255, 255), 12*ESCALA, 0, 0, "Excalibur ativada.");
                }
                if(imortalCooldown > 0)
                {
                    if(imortalCooldown % 2 == 0)
                    {
                        if(isInverted)
                        {
                             al_draw_bitmap(heroGII, x,y, 0);   
                        }else
                        {
                            al_draw_bitmap(heroGI, x,y, 0);  
                        }
                        //al_draw_filled_rectangle(x, y, x + ESCALA-1, y + ESCALA-1, al_map_rgb(  43,255,0));
                       
                    }else
                    {
                        if(isInverted)
                        {
                            al_draw_bitmap(heroRII, x,y, 0); 
                        }else
                        {
                            al_draw_bitmap(heroRI, x,y, 0); 
                        }                        
                        //al_draw_filled_rectangle(x, y, x + ESCALA-1, y + ESCALA-1, al_map_rgb(255,0,0));                         
                    }

                }else
                {
                    if(isInverted)
                    {
                        al_draw_bitmap(heroII, x,y, 0);
                    }else
                    {
                        al_draw_bitmap(heroI, x,y, 0);
                    } 
                    //al_draw_filled_rectangle(x, y, x + ESCALA-1, y + ESCALA-1, al_map_rgb(153,102,255));                      
                }
                //al_draw_textf(font, al_map_rgb(255, 255, 255), 0, 0, 0, "X: %.1f Y: %.1f", x, y);
                al_draw_textf(font, al_map_rgb(255, 255, 255), 24*ESCALA, 0, 0, "Pontuação: %d",scoreCounter);
                al_draw_textf(font, al_map_rgb(255, 255, 255), 22*ESCALA, 0, 0, "Vida: %d",hp);
                al_flip_display();

                redraw = false;
            }
        }
    }

    al_destroy_font(font);//Encerra os addos quando o while é quebrado.
    al_destroy_display(disp);
    al_destroy_timer(timer);
    al_destroy_event_queue(queue);

    return 0;
}
