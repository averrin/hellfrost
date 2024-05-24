rt = utils.makeTemplate(function(location)
  r = utils.makeBlob(location, 13, 5, 13, 5, CellType.EMPTY, CellType.UNKNOWN, false, 2)
  for _, cell in ipairs(r.cells) do
    if (cell ~= nil) then cell.tags:add("CORRUPT") end
  end
  return r
end)
templates.CORRUPT_FIELD = rt

