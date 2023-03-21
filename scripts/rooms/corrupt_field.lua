data = gm:getData()
rt = utils.makeTemplate(function(location)
  r = utils.makeBlob(location, 13, 5, 13, 5, CellType.EMPTY, CellType.UNKNOWN, false, 2)
  for _, c in ipairs(r.cells) do
    c:addFeature(CellFeature.CORRUPT);
  end
  return r
end)
templates.CORRUPT_FIELD = rt

