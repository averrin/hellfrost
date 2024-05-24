f = utils.makeFeature(function(location)
  cells = utils.scatter(location, "", 10, 50, false)
  print(cells)
  for _, c in ipairs(cells) do
      c.tags:add("BLOOD");
  end
  print("bp finished")
end)
features.BLOOD_PONDS = f



