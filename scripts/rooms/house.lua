spec = {
  maps = {
    {
    "### ",
    "#.+ ",
    "###",
    },
    {
    "##### ",
    "#...+ ",
    "#####*",
    },
    {
    "#####*",
    "#...+ ",
    "#*..# ",
    "##### ",
    },
    {
    "      ",
    "##+## ",
    "#...# ",
    "#..*# ",
    "##+###",
    "#....#",
    "######",
    },
  };
  floor = CellType.EMPTY;
  rotate = true;
  cells = {
    ["."] = function(location, room, cell)
      cell.type = CellType.FLOOR;
      cell.tags:add("WIPE")
    end;
    ["*"] = function(location, room, cell)
      cell.type = CellType.FLOOR;
      if (Random.bool(0.4)) then
        o = gm:create(location, "TORCH_STAND", cell.x, cell.y, 0)
        room:addEntity(o.entity)
      end
    end;
    [" "] = function(location, room, cell)
      cell.type = CellType.GROUND;
    end;
    ["#"] = function(location, room, cell)
      cell.type = CellType.WALL;
      cell.passThrough = false;
      cell.seeThrough = false;
      cell.tags:add("BUILDING")
      -- cell.features = {CellFeature.DUNGEON}
    end;
    ["+"] = function(location, room, cell)
      o = gm:create(location, "DOOR", cell.x, cell.y, 0)
      room:addEntity(o.entity)
      -- cell.features = {CellFeature.WIPE}
    end
  };
  postprocess = function(location, room)
  end;
}
room = utils.makeTemplateFromSpec(spec)
templates.HOUSE = room


