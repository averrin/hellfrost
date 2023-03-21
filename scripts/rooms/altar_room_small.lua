spec = {
  map = {
    ".&.",
    "._.",
    ".k."
  };
  floor = CellType.FLOOR;
  rotate = false;
  cells = {
    ["."] = function(location, room, cell) end;
    ["&"] = function(location, room, cell)
      if (Random.bool(0.5)) then
      o = gm:create(location, "STATUE", cell.x, cell.y, 0)
      room:addEntity(o.entity)
      end
    end;
    ["_"] = function(location, room, cell)
      o = gm:create(location, "ALTAR", cell.x, cell.y, 0)
      if (Random.bool()) then
        cell:addFeature(CellFeature.BLOOD)
      end
      room:addEntity(o.entity)
    end;
    ["k"] = function(location, room, cell)
      if (Random.bool(0.3)) then
        o = gm:create(location, "CULTIST", cell.x, cell.y, 0)
        room:addEntity(o.entity)
      end
    end
  };
}
altar_room = utils.makeTemplateFromSpec(spec)
templates.ALTAR_ROOM_SMALL = altar_room
