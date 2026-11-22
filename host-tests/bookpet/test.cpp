#include "../../src/apps_local/bookpet/PetState.h"
#include <cassert>
#include <iostream>
int main(){
 using namespace bookpet;
 PetEngine p;p.begin();
 assert(p.state().level==1&&p.state().food==5);
 p.apply(PetAction::Feed);assert(p.state().food==4&&p.state().feedCount==1);
 p.apply(PetAction::Rest);assert(p.state().sleeping);p.wake();assert(!p.state().sleeping);
 const auto xp=p.state().experience;p.completePageCatch(false);assert(p.state().experience>=xp);
 for(int i=0;i<10;++i)p.completePageCatch(true);
 assert(p.state().pageBites>=10&&p.state().level>=2);
 assert(p.buyFood());
 for(int i=0;i<30;++i)p.apply(PetAction::Clean);
 assert(p.state().level>=3&&p.speciesUnlocked(1));assert(p.selectSpecies(1));
 assert(!p.selectSpecies(3));assert(!p.equipToy(4));
 assert(p.state().lifetimePages==0&&p.state().booksFinished==0&&p.state().readingSessions==0);
 const auto saved=p.state();assert(p.save());PetEngine q;q.begin();
 assert(q.state().species==saved.species&&q.state().pageBites==saved.pageBites&&q.state().level==saved.level);
 assert(q.state().lifetimePages==0);assert(q.state().cleanliness<=100);
 std::cout<<"BookPet care, minigame rewards, growth without reading, unlocks and save/restore passed\n";
}
