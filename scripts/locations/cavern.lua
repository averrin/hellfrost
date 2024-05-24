locations.CAVERN = utils.makeLocation(
  "CAVERN", CellType.GROUND, CellType.WALL,
  {"CAVE"},
  100, 100,
  function(location)
    -- utils.placeRooms(location, 10, 30, 100, 100, 0, 0)
    utils.placeCaves(location, 10, 30, 100, 100, 0, 0)
    utils.cleanWalls(location)
    -- utils.makePassages(location)
    -- utils.placeStairs(location)
  end
);

