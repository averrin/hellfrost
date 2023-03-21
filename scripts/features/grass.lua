f = utils.makeFeature(function(location)
  utils.scatter(location, "HERB_GREEN", 200, 500, false)

  cells = utils.scatter(location, "", 0, 10, false)
  for _, c in ipairs(cells) do
    r = utils.makeBlob(location, 10, 3, 10, 3, CellType.EMPTY, CellType.UNKNOWN, true, 2)
    for _, cell in ipairs(r.cells) do
      if (cell.type == CellType.EMPTY) then
        cell.x = c.x+cell.x
        cell.y = c.y+cell.y
        location:addEntity("HERB_GREEN", cell)
      end
    end
  end
end)
features.GRASS = f


