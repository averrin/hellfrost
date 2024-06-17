entity = {
  ["init"] = function()
    -- print("init")
  end,
  ["interact"] = function()
    print("interact!")
    gm:destroyEntity(instance)
    print("after interact")
  end,
  ["destroy"] = function(location)
    print("destroy")
    cell = location:getCellByEntity(instance)
    print(location:addEntity("GRASS", cell))
  end,
}
