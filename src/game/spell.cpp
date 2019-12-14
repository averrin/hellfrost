#include "lss/game/spell.hpp"
// #include "lss/game/trigger.hpp"
#include "EventBus/EventBus.hpp"

void CellSpell::applyEffect(std::shared_ptr<Location> location,
                std::shared_ptr<Cell> c) {

if (spec == TerrainType::FROSTBALL) {
    auto fb = std::make_shared<Terrain>(spec, 8);
    fb->setCurrentCell(c);
    location->addObject(fb);
    c->addFeature(CellFeature::FROST);
    for (auto n : location->getNeighbors(c)) {
        n->addFeature(CellFeature::FROST);
    }
} else if (spec == TerrainType::FIREBALL) {
    auto fb = std::make_shared<Terrain>(spec, 8);
    fb->setCurrentCell(c);
    location->addObject(fb);
    c->removeFeature(CellFeature::FROST);
    for (auto n : location->getNeighbors(c)) {
        n->removeFeature(CellFeature::FROST);
    }
} else if (spec == TerrainType::ACIDPOOL) {
    auto fb = std::make_shared<Terrain>(spec, -5);
    fb->setCurrentCell(c);
    // fb->triggers.push_back(std::make_shared<EnterTrigger>([=](std::shared_ptr<Creature> actor){
    //     return Triggers::ACID_POOL_TRIGGER(actor, location);
    // }));
    location->addObject(fb);
    c->addFeature(CellFeature::ACID);
}

location->invalidate("apply spell effect");
}

void DamageSpell::applySpell(std::shared_ptr<Creature> caster,
                             std::shared_ptr<Location> location,
                             std::shared_ptr<Cell> c) {
  // auto objects = location->getObjects(c);
  // for (auto o : objects) {
  //   if (auto creature = std::dynamic_pointer_cast<Creature>(o)) {
  //     creature->applyDamage(
  //         std::dynamic_pointer_cast<Object>(location->player),
  //         damage.getDamage(caster->INTELLIGENCE(caster.get())));
  //   } else if (o->destructable && destroyObjects) {
  //     location->removeObject(o);
  //   }
  // }
  // if (c == location->player->currentCell) {
  //   location->player->applyDamage(
  //       std::dynamic_pointer_cast<Object>(location->player),
  //       damage.getDamage(caster->INTELLIGENCE(caster.get())));
  // }
  // applyEffect(location, c);
}

void DrillSpell::applySpell(std::shared_ptr<Creature> caster,
                            std::shared_ptr<Location> location,
                            std::shared_ptr<Cell> c) {
  // auto objects = location->getObjects(c);
  // for (auto o : objects) {
  //   location->removeObject(o);
  // }
  // DigEvent de(caster, c);
  // eb::EventBus::FireEvent(de);
  // applyEffect(location, c);
}
