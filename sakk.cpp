#include "sakk.h"
#include <string>
#include <iostream>

enum {BLACK, DARKBLUE, GREEN, LIGHTBLUE, RED, PURPLE, BROWN, GRAY, DARKGRAY, BLUE, LIGHTGREEN, CYAN, ORANGE, PINK, YELLOW, WHITE};

extern BITMAP* puffer;

using namespace std;

//---Babu---------

Babu::Babu(bool color, char type, char x, unsigned char y):focus(false),color(color){

        string blackstr="./img/B",whitestr="./img/W";

        pos.x=x;
        pos.y=y;

        if(color){
            blackstr+="B";
            whitestr+="B";
        }
        else{
            blackstr+="W";
            whitestr+="W";
        }

        blackstr+=type;
        whitestr+=type;

        blackstr+=".bmp";
        whitestr+=".bmp";

        blackbg=load_bmp(blackstr.c_str(),0);
        if(!blackbg) cerr<<"Hiba: "<<blackstr<<endl;
        whitebg=load_bmp(whitestr.c_str(),0);
        if(!whitebg) cerr<<"Hiba: "<<whitestr<<endl;

}

void Babu::draw(){

        if((((pos.x-'A')&1) && !((pos.y-1)&1)) || (!((pos.x-'A')&1) && ((pos.y-1)&1))) blit(whitebg,puffer,0,0,(pos.x-'A')*45+50,(pos.y-1)*45+50,45,45);
        else        blit(blackbg,puffer,0,0,(pos.x-'A')*45+50,(pos.y-1)*45+50,45,45);

        if(!focus) return;

        rect(puffer,(pos.x-'A')*45+50,(pos.y-1)*45+50,(pos.x-'A')*45+95,(pos.y-1)*45+95,GREEN);
        rect(puffer,(pos.x-'A')*45+51,(pos.y-1)*45+51,(pos.x-'A')*45+94,(pos.y-1)*45+94,GREEN);
        rect(puffer,(pos.x-'A')*45+52,(pos.y-1)*45+52,(pos.x-'A')*45+93,(pos.y-1)*45+93,GREEN);
}

void Babu::click(){

    if((mouse_x>(pos.x-'A')*45+50) && (mouse_x<(pos.x-'A')*45+95) && (mouse_y>(pos.y-1)*45+50) && (mouse_y<(pos.y-1)*45+95)){
       if(mouse_b &1)
            focus=true;
    }



}

void Babu::die(){}

bool Babu::getFocus()const{
    return focus;
}

void Babu::resetFocus(){
    focus=false;
}

bool Babu::getColor()const{
    return color;
}

void Babu::setXY(char x, unsigned char y){
        pos.x=x;
        pos.y=y;
}

Babu::~Babu(){

    destroy_bitmap(blackbg);
    destroy_bitmap(whitebg);
}


void Babu::initTabla(){

    int k=0;

    for(unsigned char y=1;y<9;++y){

        for(char x='A';x<'I';++x){

            Babu::tabla_iface[k].x=x;
            Babu::tabla_iface[k].y=y;
            Babu::tabla_iface[k++].Pbabu=0;

        }

    }

}


//----Bábuk---------------

Bastya::Bastya(bool color, bool side):Babu(color,'B',side?'H':'A',color?1:8){}


Lo::Lo(bool color, bool side):Babu(color,'L',side?'G':'B',color?1:8){}


Futo::Futo(bool color, bool side):Babu(color,'F',side?'F':'C',color?1:8){}


Kiraly::Kiraly(bool color):Babu(color,'K','D',color?1:8){}


Kiralyne::Kiralyne(bool color):Babu(color,'N','E',color?1:8){}


Paraszt::Paraszt(bool color, char x):Babu(color,'P',x,color?2:7){}
