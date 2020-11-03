#include <stdio.h>
#include <stdlib.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>

#define KEY_SEEN     1
#define KEY_RELEASED 2

void inicializa(bool teste, const char *descricao)
{
    if(teste) return;

    printf("Não foi possivel inicializar %s\n", descricao);
    exit(1);
}

int main()
{
    int i,j;
    int scoreCounter = 00;
    bool done = false;
    bool redraw = true;
    ALLEGRO_EVENT event;
    float x, y;
    x = 0;
    y = 0;    
    unsigned char tecla[ALLEGRO_KEY_MAX];
    char map[11][27] = {{'#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#'},
                        {'J','M','M','M','#','M','M','M','M','#','M','M','M','M','#','M','#','#','#','#','#','#','#','M','M','C','#'},
                        {'#','#','M','#','#','M','#','#','M','#','M','#','#','#','#','M','#','#','#','#','#','#','#','#','#','#','#'},
                        {'#','#','M','#','#','M','#','#','M','#','M','M','M','#','#','M','#','#','#','#','#','#','#','#','#','#','#'},
                        {'#','#','M','#','#','M','#','#','M','#','M','#','#','#','#','M','#','#','#','#','#','#','#','#','#','#','#'},
                        {'#','M','M','#','#','M','M','M','M','#','M','M','M','M','#','M','M','M','M','#','#','#','#','#','#','#','#'},
                        {'#','#','#','M','M','M','#','M','M','M','#','#','M','#','#','M','#','M','M','M','M','#','M','#','#','#','#'},
                        {'#','#','#','M','#','#','#','M','#','M','#','#','M','#','#','M','#','M','#','#','#','#','M','#','#','#','#'},
                        {'#','#','#','M','#','#','#','M','M','#','#','#','M','#','#','M','#','M','M','M','#','#','M','#','#','#','#'},
                        {'#','#','#','M','#','#','#','M','#','M','#','#','M','#','#','M','#','M','#','#','#','#','M','#','#','#','#'},
                        {'#','#','#','M','M','M','#','M','#','#','M','#','M','M','M','M','#','M','M','M','M','#','M','M','M','M','#'}}; 
    char mapImage[11][27];
    memcpy(mapImage,map, 11*27*sizeof(char));

    inicializa(al_init(), "Allegro.");
    inicializa(al_install_keyboard(), "Teclado.-+");
    ALLEGRO_TIMER* timer = al_create_timer(1.0 / 30.0);
    inicializa(timer, "Temporizador.");
    ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();
    inicializa(queue, "Fila de eventos");

    al_set_new_display_option(ALLEGRO_SAMPLE_BUFFERS, 1, ALLEGRO_SUGGEST);
    al_set_new_display_option(ALLEGRO_SAMPLES, 8, ALLEGRO_SUGGEST);
    al_set_new_bitmap_flags(ALLEGRO_MIN_LINEAR | ALLEGRO_MAG_LINEAR);

    ALLEGRO_DISPLAY* disp = al_create_display(50*27 , 11*50);
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
        al_wait_for_event(queue, &event);
                
        if(mapImage[(int)y/50][(int)x/50] == 'M'){
            mapImage[(int)y/50][(int)x/50] = '#';
            scoreCounter++;
        }

        switch(event.type)
        {
            case ALLEGRO_EVENT_TIMER:
                if(tecla[ALLEGRO_KEY_UP]){
                    if(y == 0){}else{
                        y-=50;
                    }
                }
                   
                if(tecla[ALLEGRO_KEY_DOWN]){
                    if(y == 10*50){}else{
                        y+=50;
                    }
                }
            
                if(tecla[ALLEGRO_KEY_LEFT]){
                    if(x == 0){}else{
                        x-=50;
                    }
                }
                
                if(tecla[ALLEGRO_KEY_RIGHT]){
                    if(x == 26*50){}else{
                        x+=50;
                    }
                }
                
                if(tecla[ALLEGRO_KEY_ESCAPE]){
                    done = true;
                }
           

                for(int i = 0; i < ALLEGRO_KEY_MAX; i++)
                    tecla[i] &= KEY_SEEN;

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

        if(done)
            break;

        if(redraw && al_is_event_queue_empty(queue))
        {
            al_clear_to_color(al_map_rgb(0, 0, 0));
            for(i=0;i<27;i++){
                for(j=0;j<11;j++){
                    switch(mapImage[j][i]){
                        case '#':
                            al_draw_filled_rectangle(i*50, j*50, i*50 + 49 , j*50 + 49 , al_map_rgb(172, 172, 172));
                            break;
                        case 'J':
                             al_draw_filled_rectangle(i*50, j*50, i*50 + 49 , j*50 + 49 , al_map_rgb(137, 137, 137));
                            break;
                        case 'M':
                            al_draw_filled_rectangle(i*50, j*50, i*50 + 49 , j*50 + 49 , al_map_rgb(255, 247, 0));
                            break;
                        case 'B':
                            al_draw_filled_rectangle(i*50, j*50, i*50 + 49 , j*50 + 49 , al_map_rgb(202, 135, 0));
                            break;
                    }
                }                
            }
            al_draw_filled_rectangle(x, y, x + 50, y + 50, al_map_rgb(255, 0, 0));
            al_draw_textf(font, al_map_rgb(255, 255, 255), 0, 0, 0, "X: %.1f Y: %.1f", x, y);
            al_draw_textf(font, al_map_rgb(255, 255, 255), 1200, 0, 0, "Pontuação: %d",scoreCounter);
            al_flip_display();

            redraw = false;
        }
    }

    al_destroy_font(font);
    al_destroy_display(disp);
    al_destroy_timer(timer);
    al_destroy_event_queue(queue);

    return 0;
}