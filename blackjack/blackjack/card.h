#ifndef card_h
#define card_h

class Card {
    
    char suit; // 'O' ouros, 'C' copas, 'P' paus, 'E' espadas
    
    char value; // '2', '3' ... 'T' ten, 'V' valete, 'D' dama, 'R' rei, 'A' as
    
    bool faceUp;
    
public:
    Card (char, char, bool);
    Card (char, char);
    char getValue();
    char getSuit();
    bool isUp();
    void flip();
    
};

#endif