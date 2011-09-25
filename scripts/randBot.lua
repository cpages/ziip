local opts = {moveUp, moveDown, moveLeft, moveRight, shoot}
local slot = -1

local debug = false

math.randomseed(os.time())
math.random(); math.random(); math.random()

function getRandOpt()
    local rval = math.random(5)
    if (debug) then
        print(rval)
    end
    return rval
end

function OnInit(nslot)
    slot = nslot
end

function OnMove()
    assert (slot ~= -1)
    opts[getRandOpt()](slot)
end
