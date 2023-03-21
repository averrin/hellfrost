spec = {
  maps = {
    {
    "##### ",
    "#...+ ",
    "#####*",
    },
    {
    "#####*",
    "#...+ ",
    "#...# ",
    "##### ",
    },
    {
    "      ",
    "##+## ",
    "#...# ",
    "#...# ",
    "##+###",
    "#....#",
    "######",
    },
  };
  floor = CellType.FLOOR;
  rotate = true;
  cells = {
    ["."] = function(location, room, cell)
      cell.features = {CellFeature.WIPE}
    end;
    ["*"] = function(location, room, cell)
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
      cell.features = {CellFeature.DUNGEON}
    end;
    ["+"] = function(location, room, cell)
      location:placeDoor(cell)
      cell.features = {CellFeature.WIPE}
    end
  };
  postprocess = function(location, room)
  end;
}
room = utils.makeTemplateFromSpec(spec)
templates.HOUSE = room


