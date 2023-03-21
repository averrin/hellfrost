data = gm:getData()
rt = utils.makeTemplate(function(location)
  r = utils.makeBlob(location, 10, 5, 10, 5, CellType.EMPTY, CellType.UNKNOWN, true, 2)
  for _, c in ipairs(r.cells) do
    c:addFeature(CellFeature.FROST);
  end
  return r
end)
templates.ICE_FIELD = rt
