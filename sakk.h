#ifndef SAKK_H
#define SAKK_H

#ifdef WIN32

    #define WIN

#endif

#ifndef WIN32

    #ifdef __WIN32__

        #define WIN

    #endif
#endif


#ifdef WIN

    #define ALLEGRO_NO_MAGIC_MAIN

#endif

#include <allegro.h>

#ifdef WIN


    #include <winalleg.h>

#endif





class Babu{

    struct{char x; unsigned char y;} pos,newpos; //poziciók
    bool focus; // rá kattintottak-e?
    static bool globalFocus; // mindekit letilt


  private:
    bool color; // szín
    bool health; // élet: leütött vagy játszik
    BITMAP* blackbg;
    BITMAP* whitebg;

  protected:
    Babu(bool color, char type, char x, unsigned char y);

  public:

    static struct iface_t{
        char x;
        unsigned char y;
        Babu* Pbabu;
    }tabla_iface[64];

    static void initTabla();

  public:
    void draw();
    void click();
    void step();
    void die();

    bool getFocus()const;
    void resetFocus();
    bool getColor()const;
    void setXY(char x, unsigned char y);

    ~Babu();
};


//---Bábuk--------

class Bastya:public Babu{

  public:
    Bastya(bool color, bool side);
    void step();


};

class Lo:public Babu{

  public:
    Lo(bool color, bool side);
};

class Futo:public Babu{

  public:
    Futo(bool color, bool side);
};

class Kiraly:public Babu{

  public:
    Kiraly(bool color);

};

class Kiralyne:public Babu{

  public:
    Kiralyne(bool color);

};

class Paraszt:public Babu{

  public:
    Paraszt(bool color, char x);

};

#endif
