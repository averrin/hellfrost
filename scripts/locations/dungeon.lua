locations.DUNGEON = utils.makeLocation(
  "DUNGEON", CellType.FLOOR, CellType.WALL,
  {"DUNGEON"},
  100, 100,
  function(location)
    -- utils.placeRooms(location, 5, 10, 50, 50, 25, 25)
    rooms = utils.placeRooms(location, 10, 30, 100, 100, 0, 0)
    utils.makePassages(location, rooms)
    utils.placeStairs(location)
  end
);
