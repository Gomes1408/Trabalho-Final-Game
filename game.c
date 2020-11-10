#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>

#define KEY_SEEN     1
#define KEY_RELEASED 2
#define N_LINHAS 11
#define N_COLUNAS 27
#define TEMPO_ESPERA 20
#define ESCALA 70

typedef struct 
{
        int body;
        int shiftable;
        int state;
        int colectable;
        int player;
        int cd;
        char identity;
}obj;


void inicializa(bool teste, const char *descricao)
{
    if(teste) return;

    printf("Não foi possivel inicializar %s\n", descricao);
    exit(1);
}

void loadMap(char src[N_LINHAS][N_COLUNAS], obj objImage[N_LINHAS][N_COLUNAS], float* x, float* y)
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
                    *y = j*ESCALA;
                    *x = i*ESCALA;
                    break;
                case 'M':
                    objImage[j][i].body = 0;
                    objImage[j][i].colectable = 1;
                    objImage[j][i].player = 0;
                    objImage[j][i].shiftable = 0;
                    objImage[j][i].identity = 'M';
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

int collectItem(int counter,obj objImage[N_LINHAS][N_COLUNAS], int x, int y, int* imortalCooldown)
{
    if(objImage[(int)y/ESCALA][(int)x/ESCALA].identity == 'M')
    {
        counter += 10;
    }
    if(counter % 100 == 0)
    {
        *imortalCooldown = (15 * 5);
    }
    objImage[(int)y/ESCALA][(int)x/ESCALA].identity = ' ';
    objImage[(int)y/ESCALA][(int)x/ESCALA].body = 0;
    objImage[(int)y/ESCALA][(int)x/ESCALA].colectable = 0;
    objImage[(int)y/ESCALA][(int)x/ESCALA].player = 0;
    objImage[(int)y/ESCALA][(int)x/ESCALA].shiftable = 0; 
    
    return counter;
}

void leverActivate(int* cd,obj objImage[N_LINHAS][N_COLUNAS])
{
    int i,j;
    for(i=0;i<N_COLUNAS;i++)
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
}

void updateOgre(obj objImage[N_LINHAS][N_COLUNAS], int randomItem)
{  
    int i,j;
    for(i=0;i<N_COLUNAS;i++)
    {
        for(j=0;j<N_LINHAS;j++)
        {
            if((objImage[j][i].identity == 'O')&&(objImage[j][i].cd == 0))
            {
                switch (randomItem)
                {
                    case 0:
                        if((objImage[j][i-1].body == 0) && (objImage[j][i-1].identity != 'A') && (objImage[j][i-1].identity != 'D') && (objImage[j][i-1].identity != 'B') && ((i-1)*ESCALA > 00))
                        {
                            objImage[j][i].body = objImage[j][i-1].body;
                            objImage[j][i].colectable = objImage[j][i-1].colectable;
                            objImage[j][i].player = objImage[j][i-1].player;
                            objImage[j][i].shiftable =  objImage[j][i-1].shiftable;
                            objImage[j][i].identity = objImage[j][i-1].identity;

                            objImage[j][i-1].body = 0;
                            objImage[j][i-1].colectable = 0;
                            objImage[j][i-1].player = 0;
                            objImage[j][i-1].shiftable = 0;
                            objImage[j][i-1].identity = 'O';
                            objImage[j][i-1].cd = TEMPO_ESPERA;
                        }
                        break; 
                    case 1:
                        if((objImage[j-1][i].body == 0) && (objImage[j-1][i].identity != 'A') && (objImage[j-1][i].identity != 'D') && (objImage[j-1][i].identity != 'B')  && ((j-1)*ESCALA > 00))
                        {
                            objImage[j][i].body = objImage[j-1][i].body;
                            objImage[j][i].colectable = objImage[j-1][i].colectable;
                            objImage[j][i].player = objImage[j-1][i].player;
                            objImage[j][i].shiftable =  objImage[j-1][i].shiftable;
                            objImage[j][i].identity = objImage[j-1][i].identity;

                            objImage[j-1][i].body = 0;
                            objImage[j-1][i].colectable = 0;
                            objImage[j-1][i].player = 0;
                            objImage[j-1][i].shiftable = 0;
                            objImage[j-1][i].identity = 'O';
                            objImage[j-1][i].cd = TEMPO_ESPERA;
                        }
                        break; 
                    case 2:
                        if((objImage[j][i+1].body == 0 ) && (objImage[j][i+1].identity != 'A') && (objImage[j][i+1].identity != 'D') && (objImage[j][i+1].identity != 'B') && ((i+1)*ESCALA < N_COLUNAS*ESCALA))
                        {
                            objImage[j][i].body = objImage[j][i+1].body;
                            objImage[j][i].colectable = objImage[j][i+1].colectable;
                            objImage[j][i].player = objImage[j][i+1].player;
                            objImage[j][i].shiftable =  objImage[j][i+1].shiftable;
                            objImage[j][i].identity = objImage[j][i+1].identity;

                            objImage[j][i+1].body = 0;
                            objImage[j][i+1].colectable = 0;
                            objImage[j][i+1].player = 0;
                            objImage[j][i+1].shiftable = 0;
                            objImage[j][i+1].identity = 'O';
                            objImage[j][i+1].cd = TEMPO_ESPERA;
                        }
                        break; 
                    case 3:
                        if((objImage[j+1][i].body == 0) && (objImage[j+1][i].identity != 'A') && (objImage[j+1][i].identity != 'D') && (objImage[j+1][i].identity != 'B') && ((j+1)*ESCALA < N_LINHAS*ESCALA))
                        {
                            objImage[j][i].body = objImage[j+1][i].body;
                            objImage[j][i].colectable = objImage[j+1][i].colectable;
                            objImage[j][i].player = objImage[j+1][i].player;
                            objImage[j][i].shiftable =  objImage[j+1][i].shiftable;
                            objImage[j][i].identity = objImage[j+1][i].identity;

                            objImage[j+1][i].body = 0;
                            objImage[j+1][i].colectable = 0;
                            objImage[j+1][i].player = 0;
                            objImage[j+1][i].shiftable = 0;
                            objImage[j+1][i].identity = 'O';
                            objImage[j+1][i].cd = TEMPO_ESPERA;
                        }
                        break;                           
                }
            }else if(objImage[j][i].cd > 0)
            {
                objImage[j][i].cd--;
            }
        }                
    }
}

void ogreHit(obj objImage[N_LINHAS][N_COLUNAS],float* pX,float* pY,char src[N_LINHAS][N_COLUNAS],int* playerHp,int* counter,int* menu, int* imortalCooldown, int hasSword)
{  
    int i,j,k,l;
    for(i=0;i<N_COLUNAS;i++)
    {
        for(j=0;j<N_LINHAS;j++)
        {
            if((objImage[j][i].identity == 'O')&&(*pX/ESCALA == i)&&(*pY/ESCALA == j))
            {
                if(hasSword || *imortalCooldown > 0)
                {
                    objImage[(int)*pY/ESCALA][(int)*pX/ESCALA].identity = ' ';
                    objImage[(int)*pY/ESCALA][(int)*pX/ESCALA].body = 0;
                    objImage[(int)*pY/ESCALA][(int)*pX/ESCALA].colectable = 0;
                    objImage[(int)*pY/ESCALA][(int)*pX/ESCALA].player = 0;
                    objImage[(int)*pY/ESCALA][(int)*pX/ESCALA].shiftable = 0;
                }else
                {
                    if(*imortalCooldown == 0)
                    {   
                        for(k=0;k<N_COLUNAS;k++)
                        {
                            for(l=0;l<N_LINHAS;l++)
                            {
                                if(src[l][k] == 'J')
                                {
                                    *pX = k * ESCALA;
                                    *pY = l * ESCALA;
                                }
                            }
                        } 

                        *playerHp -= 1;

                        if(*playerHp == 0)
                        {
                            *menu = 1;
                            loadMap(src,objImage,pX,pY);                
                            *playerHp = 3;
                            *counter = 0;
                        }
                    }    
                }              
            }            
        }                
    }
}

void giveSword(obj objImage[N_LINHAS][N_COLUNAS], int* hasSword)
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

void drawMap(obj objImage[N_LINHAS][N_COLUNAS])
{
    int i,j;
    for(i=0;i<N_COLUNAS;i++)
    {
        for(j=0;j<N_LINHAS;j++)
        {
            switch(objImage[j][i].identity)
            {
                case '#':
                    al_draw_filled_rectangle(i*ESCALA, j*ESCALA, i*ESCALA + ESCALA-1 , j*ESCALA + ESCALA-1 , al_map_rgb(172, 172, 172));
                    break;
                case 'J':
                    al_draw_filled_rectangle(i*ESCALA, j*ESCALA, i*ESCALA + ESCALA-1 , j*ESCALA + ESCALA-1 , al_map_rgb(0, 0, 0));
                    break;
                case 'C':
                    al_draw_filled_rectangle(i*ESCALA, j*ESCALA, i*ESCALA + ESCALA-1 , j*ESCALA + ESCALA-1 , al_map_rgb(217,217,25));
                    break;
                case 'M':
                    al_draw_filled_rectangle(i*ESCALA, j*ESCALA, i*ESCALA + ESCALA-1 , j*ESCALA + ESCALA-1 , al_map_rgb(255, 247, 0));
                    break;
                case 'B':
                    al_draw_filled_rectangle(i*ESCALA, j*ESCALA, i*ESCALA + ESCALA-1 , j*ESCALA + ESCALA-1 , al_map_rgb(202, 135, 0));
                    break;
                case 'O':
                    al_draw_filled_rectangle(i*ESCALA, j*ESCALA, i*ESCALA + ESCALA-1 , j*ESCALA + ESCALA-1 , al_map_rgb(0,179,30));
                    break;
                case 'A':
                    al_draw_filled_rectangle(i*ESCALA, j*ESCALA, i*ESCALA + ESCALA-1 , j*ESCALA + ESCALA-1 , al_map_rgb(56 , 176, 222));
                    break;
                case 'D':
                    al_draw_filled_rectangle(i*ESCALA, j*ESCALA, i*ESCALA + ESCALA-1 , j*ESCALA + ESCALA-1 , al_map_rgb(255 , 0, 0));
                    break;
                case ' ':
                    al_draw_filled_rectangle(i*ESCALA, j*ESCALA, i*ESCALA + ESCALA-1 , j*ESCALA + ESCALA-1 , al_map_rgb(0, 0, 0));
                    break;
            }
        }                
    }
}

int main()
{
    srand(time(NULL));
    int randomItem; 
    int i,j;
    int onMenu = 1;
    int scoreCounter = 00;
    int hp = 3;
    int leverCooldown; 
    int imortalCooldown;
    int hasSword = 0;
    bool done = false;
    bool redraw = true;    
    float x, y;    
    unsigned char tecla[ALLEGRO_KEY_MAX];
    ALLEGRO_EVENT event;   
    
    char map[N_LINHAS][N_COLUNAS] ={{'#','M','M','M','M','M','M','#','#','#','#','#','#','#','#','#','#','O','#','#','#','#','#','#','#','#','#'},
                                    {'#','J','M','M','M','M','M','B','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','C','#'},
                                    {'O','#','A','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#'},
                                    {'#','#','C','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#'},
                                    {'#','#','O','#','#','#','#','#','O','O','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','O'},
                                    {'#','#','D','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#'},
                                    {'#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#'},
                                    {'#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#'},
                                    {'#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#'},
                                    {'#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#'},
                                    {'#','O','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#'}}; 

    obj objImage[N_LINHAS][N_COLUNAS];    
    loadMap(map,objImage,&x,&y);

    inicializa(al_init(), "Allegro.");
    inicializa(al_install_keyboard(), "Teclado.");
    ALLEGRO_TIMER* timer = al_create_timer(1.0 / 15.0);
    inicializa(timer, "Temporizador.");
    ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();
    inicializa(queue, "Fila de eventos");

    al_set_new_display_option(ALLEGRO_SAMPLE_BUFFERS, 1, ALLEGRO_SUGGEST);
    al_set_new_display_option(ALLEGRO_SAMPLES, 8, ALLEGRO_SUGGEST);
    al_set_new_bitmap_flags(ALLEGRO_MIN_LINEAR | ALLEGRO_MAG_LINEAR);

    ALLEGRO_DISPLAY* disp = al_create_display(ESCALA*N_COLUNAS , N_LINHAS*ESCALA);
    inicializa(disp, "Tela.");
    ALLEGRO_FONT* font = al_create_builtin_font();
    inicializa(font, "Fonte.");
    inicializa(al_init_primitives_addon(), "Adicionais primitivos.");

    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_display_event_source(disp));
    al_register_event_source(queue, al_get_timer_event_source(timer));

    memset(tecla, 0, sizeof(tecla));

    al_start_timer(timer);
    while(1)
    {
        if(onMenu)
        {            
            al_wait_for_event(queue, &event);                   
              
            switch(event.type)
            {
                case ALLEGRO_EVENT_TIMER:
                    if(tecla[ALLEGRO_KEY_N])
                    {
                        loadMap(map,objImage,&x,&y);
                        onMenu = 0;
                    }
                       
                    if(tecla[ALLEGRO_KEY_C])
                    {
                        
                    }
                
                    if(tecla[ALLEGRO_KEY_S])
                    {
                        
                    }
                    
                    if(tecla[ALLEGRO_KEY_V])
                    {
                        onMenu = 0;
                    }
                    
                    if(tecla[ALLEGRO_KEY_Q])
                    {
                        done = true;
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
    
            if(redraw && al_is_event_queue_empty(queue))
            {
                al_clear_to_color(al_map_rgb(0, 0, 0));
                al_draw_textf(font, al_map_rgb(255, 255, 255), 8*ESCALA, 5*ESCALA, 0, "Novo Jogo(N)|");   
                al_draw_textf(font, al_map_rgb(255, 255, 255), 10.25*ESCALA, 5*ESCALA, 0, "Carregar Jogo(C)|"); 
                al_draw_textf(font, al_map_rgb(255, 255, 255), 13.25*ESCALA, 5*ESCALA, 0, "Salvar Jogo(S)|"); 
                al_draw_textf(font, al_map_rgb(255, 255, 255), 16*ESCALA, 5*ESCALA, 0, "Sair do Jogo(Q)|"); 
                al_draw_textf(font, al_map_rgb(255, 255, 255), 19*ESCALA, 5*ESCALA, 0, "Voltar(V)");
                al_draw_textf(font, al_map_rgb(255, 255, 255), 24*ESCALA, 0, 0, "Pontuação: %d",scoreCounter);             
                al_flip_display();
    
                redraw = false;
            }

        }else
        {
            randomItem =  rand() % 4;
            al_wait_for_event(queue, &event);

            giveSword(objImage,&hasSword);
                    
            if((objImage[(int)y/ESCALA][(int)x/ESCALA].identity == 'M') || (objImage[(int)y/ESCALA][(int)x/ESCALA].identity == 'C'))
            {
                scoreCounter = collectItem(scoreCounter,objImage, x, y, &imortalCooldown);    
            }
    
            if(leverCooldown > 0)
            {
                leverCooldown--;
            }

            if(imortalCooldown > 0)
            {
                imortalCooldown--;
            }
    
            switch(event.type)
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
                        }
                    }
                    
                    if(tecla[ALLEGRO_KEY_RIGHT])
                    {
                        if(x == 26*ESCALA || (objImage[(int)y/ESCALA][(int)(x+ESCALA)/ESCALA].body == 1)){}else
                        {
                            x+=ESCALA;
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
                        leverActivate(&leverCooldown, objImage);
                    }
    
                    updateOgre(objImage,randomItem);
               
                    ogreHit(objImage,&x,&y,map,&hp,&scoreCounter,&onMenu,&imortalCooldown,hasSword);
    
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
    
            if(redraw && al_is_event_queue_empty(queue))
            {
                al_clear_to_color(al_map_rgb(0, 0, 0));
                drawMap(objImage);
                if(hasSword)
                {
                    al_draw_textf(font, al_map_rgb(255, 255, 255), 12*ESCALA, 0, 0, "Excalibur ativada.");
                }
                if(imortalCooldown > 0)
                {
                    if(imortalCooldown % 2 == 0)
                    {
                        al_draw_filled_rectangle(x, y, x + ESCALA-1, y + ESCALA-1, al_map_rgb(  43,255,0));
                    }else
                    {
                        al_draw_filled_rectangle(x, y, x + ESCALA-1, y + ESCALA-1, al_map_rgb(255,0,0));
                    }
                    
                }else
                {
                    al_draw_filled_rectangle(x, y, x + ESCALA-1, y + ESCALA-1, al_map_rgb(153,102,255));
                }              
                al_draw_textf(font, al_map_rgb(255, 255, 255), 0, 0, 0, "X: %.1f Y: %.1f", x, y);
                al_draw_textf(font, al_map_rgb(255, 255, 255), 24*ESCALA, 0, 0, "Pontuação: %d",scoreCounter);
                al_draw_textf(font, al_map_rgb(255, 255, 255), 22*ESCALA, 0, 0, "Vida: %d",hp);
                al_flip_display();
    
                redraw = false;
            }
        }        
    }

    al_destroy_font(font);
    al_destroy_display(disp);
    al_destroy_timer(timer);
    al_destroy_event_queue(queue);

    return 0;
}