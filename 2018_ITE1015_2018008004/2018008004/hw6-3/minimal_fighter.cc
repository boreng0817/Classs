#include <iostream>
#include "minimal_fighter.h"
using namespace std;

//CONSTRUCTOR
MinimalFighter::MinimalFighter() {mHp = 0, mPower = 0, mStatus = Invalid;}
MinimalFighter::MinimalFighter(int _hp, int _power){mHp = _hp , mPower = _power;
setStatus(); }

//GET
int MinimalFighter::hp() const{ return mHp; }
int MinimalFighter::power() const{ return mPower; }
FighterStatus MinimalFighter::status() const{ return mStatus; }

//SET
void MinimalFighter::setHp(int _hp){ 
mHp = _hp;
if(mHp<0) mHp = 0; }
void MinimalFighter::setStatus(){ if(mHp>0) mStatus = Alive; else mStatus = Dead; }

//ACTION
void MinimalFighter::hit(MinimalFighter*_enemy){
if( (status() == Alive) && (_enemy->status() == Alive) ){
setHp(hp() - _enemy->mPower);
_enemy->setHp(_enemy->hp() - power());
setStatus();
_enemy->setStatus();
	}
}

void MinimalFighter::attack(MinimalFighter*_target){
if( (status() == Alive) && (_target->status() == Alive) ){
_target->setHp(_target->hp() - power());
mPower = 0;
_target->setStatus();
	}
}

void MinimalFighter::fight(MinimalFighter*_enemy){
while( (status() == Alive) && (_enemy->status() == Alive) ){
setHp(hp() - _enemy->mPower);
_enemy->setHp(_enemy->hp() - power());
setStatus();
_enemy->setStatus();
	}
}


