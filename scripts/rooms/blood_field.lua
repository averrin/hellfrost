data = gm:getData()
blood_field = utils.makeTemplate(function(location)
  r = utils.makeBlob(location, 18, 8, 18, 8, CellType.EMPTY, CellType.UNKNOWN, false, 3)

  for _, cell in ipairs(r.cells) do
    if (cell.type == CellType.EMPTY) then
      if (Random.bool(data.probability.BLOOD_FIELD_CELL)) then
        cell:addFeature(CellFeature.BLOOD)
        if (Random.bool(data.probability.BLOOD_FIELD_FEATURE)) then
          if (Random.bool(data.probability.BLOOD_FIELD_BONES)) then
            o = gm:create(location, "BONES", cell.x, cell.y, 0)
          else
            o = gm:create(location, "HERB_RED", cell.x, cell.y, 0)
          end
          r:addEntity(o.entity)
        end
      end
    end
  end

  cell = r:getRandomCell(CellType.EMPTY)
  o = gm:create(location, "CULTIST", cell.x, cell.y, 0)
  r:addEntity(o.entity)
  return r
end)
templates.BLOOD_FIELD = blood_field



