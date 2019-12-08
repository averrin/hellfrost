#ifndef __GAMEMANAGER_H_
#define __GAMEMANAGER_H_

#include "ui/state.hpp"
#include "lss/game/magic.hpp"
#include "lss/game/player.hpp"
#include "lss/commands.hpp"
#include "lss/animation.hpp"

class GameManager {
    public:
    GameManager();
    void startGame();

  void initStates();

  static const int MAX_LEVELS = 10;
  int currentLevel = 0;
  std::vector<std::shared_ptr<Animation>> animations;
  std::map<int, std::shared_ptr<Location>> locations;

  std::shared_ptr<State> state;
  std::shared_ptr<State> statusState;
  std::shared_ptr<State> objectSelectState;
  std::shared_ptr<State> helpState;
  std::shared_ptr<State> heroLineState;
  std::shared_ptr<State> inventoryState;
  std::shared_ptr<State> heroState;
  std::shared_ptr<State> gameOverState;
  std::shared_ptr<State> inspectState;
  std::shared_ptr<State> logState;

  std::shared_ptr<Player> hero;
  std::shared_ptr<Magic> magic;

};

#endif // __GAMEMANAGER_H_
