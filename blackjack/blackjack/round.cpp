//
//  round.cpp
//
//  Projeto 2 PROG
//
//  Turma 6 Grupo 4 
//
//  Diogo Eiras - ei11087
//  IDE - Eclipse, Visual Studio
//
//  Pedro Fernandes - ei11122
//  IDE - Xcode
//

#include "round.h"

Round::Round(vector<Player*> vec_players, House* h, Dealer* d, float* b){
    this->active_players = vec_players;
    this->current_player = active_players.begin();
    this->roundOver = false;
    gameDeck = h;
    dealer = d;
    minimBet = b;
    
    for (unsigned int i = 0; i < 2; i++) {
        
        for (vector<Player*>::iterator player = active_players.begin(); player < active_players.end(); player++ ) {
            (*player)->getHand()->getCard(gameDeck); 
        }
        
        if (i < 1) {
           d->getHand()->getCard(gameDeck);
        }
        else{
            d->getHand()->getCardFlipped(gameDeck);
        }
    }
    
    dealer->setStatus(1); // not playing yet
    
    char dealer_first_card = dealer->getHand()->fetchCard(1)->getValue();
    
    if (dealer_first_card == 'A' ||  dealer_first_card == 'T') {  // if dealer has ace or 10, flip his 2nd card
        dealer->getHand()->fetchCard(2)->flip();
    }

}


Player* Round::getNextPlayer(){
	current_player++;
	if(current_player == active_players.end())
	{
		current_player = active_players.begin();
		return NULL;  // all current players have played
	}
	
	return *current_player;
}

Player* Round::getCurrentPlayer(){
    return *current_player;
}

bool Round::allPlayersAreDone(){
    if ((*this->current_player)->getStatus() != 1){
        if( this->getNextPlayer() == NULL ) // every current player is done
        {
            dealer->getStatus(); // dealer is now playing
            return true;
        }
        else
            return false;
    }
    else
        return false;
}


bool Round::allPlayersHaveBetted(){    
    Player* player_betting = getCurrentPlayer();
    
    if (player_betting == active_players.back() && player_betting->getBet() > 0){
        current_player = active_players.begin();
        return true; 
    }
    else if (player_betting->getBet() > 0) { // current player already betted
        getNextPlayer();
        return false;
    }
    else
        return false;
}



bool Round::over(){
    
    return roundOver;
}

bool Round::doAction(int choice){
    switch (choice) {
        case 1:
            (*current_player)->stands();
            break;
        case 2:
            (*current_player)->hits(gameDeck);
            break;
        case 3:
            (*current_player)->doubles(gameDeck);
            break;
            
        default:
            return false;
            break;
    }
    
    return true;
}

Dealer* Round::getDealer(){
    return dealer;
}


float Round::getMinimBet(){
    return *minimBet;
}


bool Round::dealerIsDone(){
    if (dealer->getStatus() == 2 && !dealer->getHand()->fetchCard(2)->isUp() ) // hand under 17 and not flipped 2nd card yet
    {
        dealer->flipCard();
        dealer->setLastAction(1); // flipped 2nd card
        dealer->getStatus();
        return false;
    }
    else if (dealer->getStatus() == 2){
        dealer->hits(gameDeck);
        dealer->setLastAction(2); // dealer hits
        dealer->getStatus();
        return false;
    }
    else{
        dealer->setLastAction(3); // dealer stands
        current_player = active_players.begin(); // to start distributing the money
        return true;
    }
}

bool Round::moneyIsDistributed(){
    Player* player = getCurrentPlayer();
    
    if (player->getBet() == 0 && player == active_players.back() ) {   // last player already got his money
        this->roundOver = true; // round is now over;
        return true;
    }
    else if (player->getBet() == 0) { // player already got his money
        player = getNextPlayer();
    }
    
    int player_score = player->getHand()->getTotal();
    int dealer_score = this->dealer->getHand()->getTotal();
    int player_status = player->getStatus();
    int dealer_status = this->dealer->getStatus();
    float player_bet = player->getBet();
    
    
    if (player_status == 6 && dealer_status != 4) { 
        // player has doubled and has blackjack AND dealer did not blackjack
        
        player->updateBalance(player_bet*2*2*1.5);
    }
    else if (player_status == 4 && (dealer_status != 5 && dealer_score < player_score) ) {  
        // player has doubled AND dealer is not bust AND dealer has lesser score 
        
        player->updateBalance(player_bet*2*2);
    }
    else if (player_status == 5 && (dealer_status != 5 && dealer_score < player_score) ) { 
        // player has blackjack AND dealer is not bust AND dealer has lesser score 
        
        player->updateBalance(player_bet*2*1.5);
    }
    else if ( (player_status != 3 && player_score > dealer_score ) || (player_status != 3 && dealer_status == 5) ){
        // player isnt bust AND player has better hand OR player inst bust AND dealer is bust
        
        player->updateBalance(player_bet*2);
    }
    else if ( player_status == 3 || ( dealer_score > player_score && dealer_status != 5) ){
        // player is bust OR dealer has better hand AND dealer isnt bust
        
        player->updateBalance(0);
    }
    else if ( player_score == dealer_score ){
        // player hand equals dealers hand
        
        player->updateBalance(player_bet);
    }
    
    return false;
}

Round::~Round(){
    for (int i = 0; i < active_players.size(); i++ ) {
        (active_players.at(i))->getHand()->putCards(gameDeck);
        if (active_players.at(i)->getBalance() >= *minimBet) {
            active_players.at(i)->changeStatus(1);  // is playing
        }
        else{
            active_players.at(i)->changeStatus(7);// player does not have enough money, lost
        }
    }
    dealer->getHand()->putCards(gameDeck);
    dealer->setStatus(1); // not playing yet
}

