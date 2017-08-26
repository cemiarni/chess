/**************************************************************************
**************                                        *********************
**************  Multiplayer Sakk - alpha prerelease 1  *********************
**************                                        *********************
**************************************************************************/


#include "sakk.h"

#ifdef WIN

    #include <windows.h>
    #include <winsock2.h>


#else

    #include <sys/socket.h>
    #include <arpa/inet.h>

#endif





#include <iostream>
#include "include/frame.h"

#ifdef DEBUG
  #define DEVEL 1 // switch to the developmenting, turn off the network methods
#else
  #define DEVEL 0
#endif


using namespace std;

enum{BAL,JOBB};
enum{FEHER,FEKETE};

BITMAP* puffer;

int ssock; // szerver socket
int csock; // kliens socket




void bezar(){

        key[KEY_ESC]=true;

}

void init(){
    allegro_init();
    set_gfx_mode(GFX_AUTODETECT_WINDOWED, 450, 450, 0, 0);
    puffer=create_bitmap(SCREEN_W,SCREEN_H);

    install_keyboard();
    install_mouse();
    install_sound(DIGI_AUTODETECT, MIDI_AUTODETECT, 0);
    install_timer();

    set_window_title("Sakk - multiplayer alpha prerelease1");

    set_close_button_callback(bezar);
}

void destruct(){
    destroy_bitmap(puffer);
    for(int i=0;i<64;++i)
        delete Babu::tabla_iface[i].Pbabu;

    #ifdef WIN

        if(!DEVEL)
            WSACleanup();
    #endif
}




void mouse(BITMAP* puffer){
        triangle(puffer,mouse_x,mouse_y,mouse_x+6,mouse_y+12,mouse_x+12,mouse_y+6,GREEN);
        line(puffer,mouse_x+10,mouse_y+9,mouse_x+16,mouse_y+14,GREEN);
        line(puffer,mouse_x+11,mouse_y+9,mouse_x+15,mouse_y+14,GREEN);
        line(puffer,mouse_x+9,mouse_y+9,mouse_x+15,mouse_y+15,GREEN);
        line(puffer,mouse_x+8,mouse_y+9,mouse_x+14,mouse_y+15,GREEN);
}



void tabla(bool reverse){

        bool change=false;

        //---Keret-----------
        rect(puffer,49,49,50+8*45,51+8*45,RED);

        //----Felirat------------------------
        for(int i=0;i<8;++i){

            if(reverse){
                textprintf_ex(puffer, font, 40, 65+i*45, BLACK, WHITE, "%d",8-i); //betük
                textprintf_ex(puffer, font, 9*46, 65+i*45, BLACK, WHITE, "%d",8-i);//betük
                textprintf_ex(puffer, font,  65+i*45, 40, BLACK, WHITE, "%c",'H'-i); //szamok
                textprintf_ex(puffer, font,  65+i*45, 9*46, BLACK, WHITE, "%c",'H'-i); //szamok
            }
            else{
                textprintf_ex(puffer, font, 40, 65+i*45, BLACK, WHITE, "%d",i+1);//betük
                textprintf_ex(puffer, font, 9*46, 65+i*45, BLACK, WHITE, "%d",i+1);//betük
                textprintf_ex(puffer, font,  65+i*45, 40, BLACK, WHITE, "%c",'A'+i); //szamok
                textprintf_ex(puffer, font,  65+i*45, 9*46, BLACK, WHITE, "%c",'A'+i); //szamok
            }
        }


        //-----Pepita----------------------
        for(int i=0;i<8;++i){

            for(int j=0;j<8;++j){

                if(change) change=false;
                    else change=true;

                if(change)
                    rectfill(puffer,j*45+50,i*45+50,(j+1)*45+50,(i+1)*45+50,BLACK);
            }


                    if(change) change=false;
                    else change=true;

        }

}

void draw(){
    for(int i=0;i<64;++i){
        if(Babu::tabla_iface[i].Pbabu) Babu::tabla_iface[i].Pbabu->draw();
    }
}

void click(bool color){
    for(int i=0;i<64;++i){
        if(Babu::tabla_iface[i].Pbabu && Babu::tabla_iface[i].Pbabu->getColor()==color) Babu::tabla_iface[i].Pbabu->click();
    }
}

int step(bool& color,bool& receive){
    int i,j;

    for(i=0;i<64;++i){ //focusos keres

        if(Babu::tabla_iface[i].Pbabu && Babu::tabla_iface[i].Pbabu->getFocus()) break;
    }
    if(i==64) return 0; // nem talált

    for(j=0;j<64;++j){
        if((mouse_b&1) && (mouse_x>(Babu::tabla_iface[j].x-'A')*45+50) && (mouse_x<(Babu::tabla_iface[j].x-'A')*45+95) && (mouse_y>(Babu::tabla_iface[j].y-1)*45+50) && (mouse_y<(Babu::tabla_iface[j].y-1)*45+95)) break;
    }

    if(j==64) return 0; // nem volt kattintás

    Babu::tabla_iface[i].Pbabu->resetFocus();

    //leütés
    if(Babu::tabla_iface[j].Pbabu){
        if((Babu::tabla_iface[i].Pbabu!=Babu::tabla_iface[j].Pbabu) && (Babu::tabla_iface[i].Pbabu->getColor()!=Babu::tabla_iface[j].Pbabu->getColor())){

                delete Babu::tabla_iface[j].Pbabu;
                Babu::tabla_iface[j].Pbabu=0;

        }
    }

    //lépés
    if(!Babu::tabla_iface[j].Pbabu){

            Babu::tabla_iface[j].Pbabu=Babu::tabla_iface[i].Pbabu;
            Babu::tabla_iface[i].Pbabu=0;
            Babu::tabla_iface[j].Pbabu->setXY(Babu::tabla_iface[j].x,Babu::tabla_iface[j].y);



            unsigned char msg[2];
            msg[0]=i;
            msg[1]=j;

            if(!DEVEL)
                if(send(csock,(char*)msg,2,0)<0) return -1;



            if(color) color=false;
            else color=true;
            receive=true;
    }


    return 0;

}



int connect(bool server, const char* ip, unsigned short int port){



    struct sockaddr_in addr;


    #ifdef WIN

        // Winsock inicializálás
        WSADATA	wsd;
        if(WSAStartup(0x0202, &wsd) != 0){
            cerr<<"WSAStartup"<<endl;
            return 1;
        }


    #endif




//    int reuse = 1;

        if((ssock = socket(PF_INET, SOCK_STREAM, 0)) < 0)
        {
            cerr<<"Err: socket"<<endl;
            return 1;
        }




    addr.sin_family=AF_INET;
    addr.sin_addr.s_addr=INADDR_ANY;
    addr.sin_port=htons(port);

    if(server){


       // setsockopt(ssock,SOL_SOCKET,SO_REUSEADDR,&reuse,sizeof(reuse));

        if(bind(ssock,(struct sockaddr*)&addr,sizeof(addr))<0){

            cerr<<"Err: bind"<<endl;
            return 1;
        }

        if(listen(ssock,5)<0){
            cerr<<"Err: listen"<<endl;
            return 1;
        }

        if((csock=accept(ssock,0,0))<2){
            cerr<<"Err: accept"<<endl;
            return 1;
        }
    }
    else{
        addr.sin_addr.s_addr=inet_addr(ip);
        csock=ssock;
        connect(csock,(struct sockaddr*)&addr,sizeof(addr));
    }

    return 0;
}

void disconnect(bool server){
    if(server){
       #ifdef WIN
            closesocket(ssock);
       #else
            close(ssock);
       #endif
    }
       #ifdef WIN
            closesocket(csock);
       #else
            close(csock);
       #endif
}

// TODO: ezen gondolkodj
/*int do_menu(BITMAP* puffer, int (menu_func*)(BITMAP*)){

    int ret=1;
    while(ret){

        clear_to_color(puffer,WHITE);

        menu_func(puffer);

        mouse(puffer);
        blit(puffer,screen,0,0,0,0,SCREEN_W, SCREEN_H);

        rest(1);
    }

}*/

// TODO: és ezen is (gondolkodj)
int join_menu(BITMAP* puffer){


}

Babu::iface_t Babu::tabla_iface[64];


void init_babu(){


        //Fekete
    Babu::tabla_iface[0].Pbabu=new Bastya(FEKETE,BAL);
    Babu::tabla_iface[1].Pbabu=new Lo(FEKETE,BAL);
    Babu::tabla_iface[2].Pbabu=new Futo(FEKETE,BAL);
    Babu::tabla_iface[3].Pbabu=new Kiraly(FEKETE);
    Babu::tabla_iface[4].Pbabu=new Kiralyne(FEKETE);
    Babu::tabla_iface[5].Pbabu=new Futo(FEKETE,JOBB);
    Babu::tabla_iface[6].Pbabu=new Lo(FEKETE,JOBB);
    Babu::tabla_iface[7].Pbabu=new Bastya(FEKETE,JOBB);

    for(int i='A';i<'I';++i)
        Babu::tabla_iface[8+i-'A'].Pbabu=new Paraszt(FEKETE,i);


    //Fehér
    Babu::tabla_iface[56].Pbabu=new Bastya(FEHER,BAL);
    Babu::tabla_iface[57].Pbabu=new Lo(FEHER,BAL);
    Babu::tabla_iface[58].Pbabu=new Futo(FEHER,BAL);
    Babu::tabla_iface[59].Pbabu=new Kiraly(FEHER);
    Babu::tabla_iface[60].Pbabu=new Kiralyne(FEHER);
    Babu::tabla_iface[61].Pbabu=new Futo(FEHER,JOBB);
    Babu::tabla_iface[62].Pbabu=new Lo(FEHER,JOBB);
    Babu::tabla_iface[63].Pbabu=new Bastya(FEHER,JOBB);

    for(int i='A';i<'I';++i)
        Babu::tabla_iface[48+i-'A'].Pbabu=new Paraszt(FEHER,i);

}


int main(int argc, char* argv[]){

    bool color=true;
    bool server=true;
    bool recive=true;
    bool start=true;
    char* ip=0;
    unsigned short port=1234;

    if(argc==3){ // kliens lesz
        server=false;
        color=false;
        recive=false;
        ip=argv[1];
        port=atoi(argv[2]);
    }

    if(argc==2){
        port=atoi(argv[1]);
    }

    if(!DEVEL)

        connect(server,ip,port);

    init();




    BITMAP* wait_icon=load_bmp("./img/sand_clock.bmp",0);
    SAMPLE *signal = load_wav("./img/pop.wav");





//---Bábuk fel sorakoztatása----------

    Babu::initTabla();


    init_babu();



//---Menü-------------------------


    frame menu(puffer,"menu");
    frame host(puffer,"host");

    menu.new_block("menu");
    host.new_block("host");

    host["host"].new_element("textfield","ip",100,200);
    host["host"].new_element("textfield","port",100,220);
    host["host"].new_element("button","play",100,240);



    menu["menu"].new_element("button","host",100,200);
    menu["menu"].new_element("button","join",100,220);
    menu["menu"].new_element("button","about",100,240);
    menu["menu"].new_element("button","exit",100,260);

    ((button*)&menu["menu"]["host"])->set_label("host");
    ((button*)&menu["menu"]["join"])->set_label("join");
    ((button*)&menu["menu"]["about"])->set_label("about");
    ((button*)&menu["menu"]["exit"])->set_label("exit");

// TODO: fejezd be a menüt
    while(!key[KEY_ESC]){



        bool almenu=false;

        if( ((button*)&menu["menu"]["host"])->get_value() ){ // hosztolás

            host.draw();
            almenu=true;

        }

        if( ((button*)&menu["menu"]["join"])->get_value() ){ // csatlakozás

            almenu=true;
        }

        if( ((button*)&menu["menu"]["about"])->get_value() ){ // about

            almenu=true;
        }

        if( ((button*)&menu["menu"]["exit"])->get_value() ){ // kilépés


            almenu=true;
        }
        if(!almenu)
            menu.draw();


    }

//---Főprogram--------------------
    while(!key[KEY_ESC]){

        clear_to_color(puffer,WHITE);



        tabla(false);// tabla rajzol

        if(recive && !DEVEL){//bérkező adatok feldolgozása

            unsigned char msg[2];

            if(recv(csock,(char*)msg,2,0)<0){
                break;
            }

            delete Babu::tabla_iface[msg[1]].Pbabu;
            Babu::tabla_iface[msg[1]].Pbabu=Babu::tabla_iface[msg[0]].Pbabu;
            Babu::tabla_iface[msg[0]].Pbabu=0;
            if(Babu::tabla_iface[msg[1]].Pbabu) Babu::tabla_iface[msg[1]].Pbabu->setXY(Babu::tabla_iface[msg[1]].x,Babu::tabla_iface[msg[1]].y);

            recive=false;

            if(!server || !start){
                if(color) color=false;
                else color=true;
            }
            start=false;

            play_sample(signal, 255, 127, 1000, 0);

        }


        if((server && color) || (!server && !color) || DEVEL){// adatok küldése, lépés, egyéb esemeény kezelés
            if(step(color,recive)<0) break;
            click(color);

        }

        draw(); // bábuk rajzolása

        if((server && color) || (!server && !color) || DEVEL) mouse(puffer); // egér rajzolása
        else blit(wait_icon,puffer,0,0,10,10,25,36);

        blit(puffer,screen,0,0,0,0,SCREEN_W, SCREEN_H);

        rest(1);

    }


    destroy_sample(signal);
    destroy_bitmap(wait_icon);
    disconnect(server);
    destruct();


    return 0;
}
END_OF_MAIN()
